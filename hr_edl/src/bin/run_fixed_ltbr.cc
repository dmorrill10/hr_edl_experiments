#include <memory>
#include <optional>
#include <queue>
#include <string>
#include <tuple>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/strings/match.h"
#include "absl/strings/str_format.h"
#include "bin/ltbr.h"
#include "eigen/Eigen/Dense"
#include "hr_edl/adaptive_profile.h"
#include "hr_edl/best_response.h"
#include "hr_edl/decision_point.h"
#include "hr_edl/policy_evaluation.h"
#include "hr_edl/spiel_extra.h"
#include "hr_edl/stopwatch.h"
#include "open_spiel/game_transforms/turn_based_simultaneous_game.h"
#include "open_spiel/spiel.h"

// Environment
ABSL_FLAG(std::string, game, "leduc_poker", "The game to play.");
ABSL_FLAG(std::string, sampler, "null", "The sampler to use.");
ABSL_FLAG(size_t, t, 500, "The number of iterations to run.");
ABSL_FLAG(size_t, random_seed, 0, "Seed for a sampler's random engine.");

// Modes
ABSL_FLAG(bool, show_num, false,
          "Show the number of table entries that would be produced without "
          "computing them.");
ABSL_FLAG(size_t, alg_group, 0,
          "0 => efficient EFR instances only, 1 => efficient and behavioral "
          "deviation EFR instances.");

// Parallelism
ABSL_FLAG(size_t, threads, 1, "The number of threads to use.");

void run_experiment() {
  const bool show_num = absl::GetFlag(FLAGS_show_num);
  const size_t alg_group = absl::GetFlag(FLAGS_alg_group);
  const auto labeled_algs = AlgsInGroup(alg_group);
  const auto alg_labels = LearnerProfileLabels(labeled_algs);
  if (show_num) {
    std::cout << alg_labels.size() * (alg_labels.size() + 1) << std::endl;
    return;
  }

  const std::string game_name = absl::GetFlag(FLAGS_game);
  const std::shared_ptr<const open_spiel::Game> game =
      open_spiel::LoadGameAsTurnBased(game_name);

  const std::unique_ptr<open_spiel::State> root_state = game->NewInitialState();

  const int random_seed = absl::GetFlag(FLAGS_random_seed);
  const size_t iterations = absl::GetFlag(FLAGS_t);

  const int num_threads = absl::GetFlag(FLAGS_threads);

  const std::string sampler_name = absl::GetFlag(FLAGS_sampler);
  std::unique_ptr<hr_edl::MccfrSampler> sampler =
      hr_edl::NewSampler(sampler_name, random_seed);

  std::vector<Eigen::MatrixXd> expected_values;

  std::cout << "learner/env  ";
  for (const auto& label : alg_labels) {
    std::cout << label << "  ";
    expected_values.push_back(
        Eigen::MatrixXd::Zero(alg_labels.size(), iterations));
  }
  expected_values.push_back(
      Eigen::MatrixXd::Zero(alg_labels.size(), iterations));  // For BR row
  std::cout << std::endl;

  hr_edl::Stopwatch stop_watch;
  std::vector<Eigen::MatrixXd> milliseconds;
  for (const auto& label : alg_labels) {
    milliseconds.push_back(
        Eigen::MatrixXd::Zero(alg_labels.size(), iterations));
  }
  milliseconds.push_back(
      Eigen::MatrixXd::Zero(alg_labels.size(), iterations));  // For BR row

  const double utility_diameter = game->MaxUtility() - game->MinUtility();
  std::vector<hr_edl::AdaptiveProfilePtr> col_learner_profiles =
      NewLearnerProfiles(labeled_algs, game->NumPlayers(), utility_diameter);

  std::queue<std::thread> threads;

  hr_edl::CachedDecisionPoint root(root_state->Clone(), false, true);
  for (size_t col_alg = 0; col_alg < col_learner_profiles.size(); ++col_alg) {
    const auto f = [&stop_watch, &milliseconds, &expected_values,
                    &col_learner_profiles, &root_template = root, &sampler,
                    &labeled_algs, col_alg, iterations, utility_diameter] {
      hr_edl::CachedDecisionPoint root(root_template);
      std::vector<hr_edl::AdaptiveProfilePtr> row_algs;
      for (size_t row_alg = 0; row_alg < col_learner_profiles.size() + 1;
           ++row_alg) {
        row_algs.emplace_back(
            row_alg < col_learner_profiles.size()
                ? labeled_algs[row_alg].New(root.NumPlayers(), utility_diameter)
                : hr_edl::AdaptiveProfilePtr(
                      new hr_edl::BestResponseProfile(root.NumPlayers())));
      }
      for (size_t t = 0; t < iterations; ++t) {
        const auto col_profile = col_learner_profiles[col_alg]->Frozen();
        for (size_t row_alg = 0; row_alg < col_learner_profiles.size() + 1;
             ++row_alg) {
          stop_watch.reset();
          const auto v =
              t < iterations - 1
                  ? row_algs[row_alg]->UpdateAndReturnEv(root, *sampler,
                                                         col_profile)
                  : row_algs[row_alg]->Ev(root, *sampler, col_profile);
          milliseconds[row_alg](col_alg, t) = stop_watch.milliseconds();
          expected_values[row_alg](col_alg, t) = v;
        }
        if (t < iterations - 1) {
          col_learner_profiles[col_alg]->UpdateAndReturnEv(
              root, *sampler, col_learner_profiles[col_alg]->Frozen());
        }
      }
    };
    if (num_threads > 1) {
      threads.emplace(f);

      if (threads.size() >= num_threads) {
        hr_edl::Wait(threads.front());
        threads.pop();
      }
    } else {
      f();
    }
  }
  hr_edl::Wait(threads);

  for (size_t t = 0; t < iterations; ++t) {
    std::cout << "t = " << t << std::endl;
    for (size_t row_alg = 0; row_alg < col_learner_profiles.size() + 1;
         ++row_alg) {
      const std::string row_label =
          row_alg < col_learner_profiles.size() ? alg_labels[row_alg] : "BR";
      std::cout << row_label << "  ";
      for (size_t col_alg = 0; col_alg < col_learner_profiles.size();
           ++col_alg) {
        std::cout << absl::StrFormat("(%g, %g) ",
                                     expected_values[row_alg](col_alg, t),
                                     milliseconds[row_alg](col_alg, t));
      }
      std::cout << std::endl;
    }
  }
}

int main(int argc, char** argv) {
  absl::SetProgramUsageMessage(
      "Run a learning tournament with fixed compatriots.");
  absl::ParseCommandLine(argc, argv);
  run_experiment();
}
