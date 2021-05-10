#include <memory>
#include <queue>
#include <string>
#include <tuple>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/strings/match.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_join.h"
#include "bin/ltbr.h"
#include "eigen/Eigen/Dense"
#include "hr_edl/adaptive_profile.h"
#include "hr_edl/best_response.h"
#include "hr_edl/decision_point.h"
#include "hr_edl/policy_evaluation.h"
#include "hr_edl/spiel_extra.h"
#include "open_spiel/algorithms/corr_dev_builder.h"
#include "open_spiel/algorithms/corr_dist.h"
#include "open_spiel/game_transforms/turn_based_simultaneous_game.h"
#include "open_spiel/spiel.h"

// Environment
ABSL_FLAG(std::string, efg_file, "", "The EFG file representing the game.");
ABSL_FLAG(int32_t, t, 1000, "The number of iterations to run.");

// Reporting
ABSL_FLAG(
    double, report_gap_factor, 2.0,
    "The factor increasing the number of iterations to finish between "
    "reporting exploitability. E.g. 1 prints after each iteration, 2 prints "
    "after 2^i iterations, i >= 1.");
ABSL_FLAG(int32_t, report_skip, 0,
          "Number of initial iterations for which to skip reporting.");

void run_experiment() {
  const std::shared_ptr<const open_spiel::Game> game =
      open_spiel::LoadGameAsTurnBased(
          "efg_game", {{"filename", open_spiel::GameParameter(
                                        absl::GetFlag(FLAGS_efg_file))}});
  const int iterations = absl::GetFlag(FLAGS_t);
  hr_edl::CachedDecisionPoint root(game->NewInitialState());

  const hr_edl::MultiplicativeCheckpointMarker report_marker(
      absl::GetFlag(FLAGS_report_gap_factor), absl::GetFlag(FLAGS_report_skip));

  auto print_gaps_for_learner = [&report_marker, &game, &root_template = root,
                                 iterations](
                                    hr_edl::AdaptiveProfile&& learner) {
    hr_edl::CachedDecisionPoint root(root_template);
    hr_edl::NullSampler sampler;
    open_spiel::algorithms::CorrDevBuilder cd_builder;
    const open_spiel::algorithms::CorrDistConfig config;

    cd_builder.AddMixedJointPolicy(open_spiel::TabularPolicy(
        hr_edl::ActionsAndProbsTable(root, learner.Frozen())));

    size_t num_checkpoints = 0;
    for (int t = 1; t <= iterations; ++t) {
      // Simultaneous updates
      const auto profile = learner.Frozen();
      learner.UpdateAndReturnEv(root, sampler, profile);
      const auto next_profile = open_spiel::TabularPolicy(
          hr_edl::ActionsAndProbsTable(root, learner.Frozen()));

      cd_builder.AddMixedJointPolicy(next_profile);

      if (report_marker.IsCheckpoint(t, num_checkpoints) || t == iterations) {
        ++num_checkpoints;
        const open_spiel::algorithms::CorrelationDevice mu =
            cd_builder.GetCorrelationDevice();
        const double afcce_dist = AFCCEDist(*game, config, mu);
        const double afce_dist = AFCEDist(*game, config, mu);
        const double efcce_dist = EFCCEDist(*game, config, mu);
        const double efce_dist = EFCEDist(*game, config, mu);
        std::cout << absl::StrFormat("%d %2.10lf %2.10lf %2.10lf %2.10lf", t,
                                     afcce_dist, afce_dist, efcce_dist,
                                     efce_dist)
                  << std::endl;
      }
    }
  };

  std::cout << "# t  AFCCE  AFCE  EFCCE  EFCE" << std::endl;
  std::cout << "Algorithm: CSPS EFR" << std::endl;
  print_gaps_for_learner(hr_edl::CfTreeLearnerProfile(
      hr_edl::BehavioralDeviationTabularCfvLearner<
          hr_edl::CausalPartialSequencePredecessors>::
          NewList(game->NumPlayers(), RmUpdate, RmLink)));

  std::cout << "Algorithm: TIPS EFR" << std::endl;
  print_gaps_for_learner(hr_edl::CfTreeLearnerProfile(
      hr_edl::BehavioralDeviationTabularCfvLearner<
          hr_edl::TwiceInformedPartialSequencePredecessors>::
          NewList(game->NumPlayers(), RmUpdate, RmLink)));

  std::cout << "Algorithm: BPS EFR" << std::endl;
  print_gaps_for_learner(hr_edl::CfTreeLearnerProfile(
      hr_edl::BehavioralDeviationTabularCfvLearner<
          hr_edl::BlindPartialSequencePredecessors>::NewList(game->NumPlayers(),
                                                             RmUpdate,
                                                             RmLink)));

  std::cout << "Algorithm: CFR" << std::endl;
  print_gaps_for_learner(hr_edl::CfTreeLearnerProfile(
      hr_edl::BehavioralDeviationTabularCfvLearner<
          hr_edl::ImmediateExternalSequencePredecessors>::
          NewList(game->NumPlayers(), RmUpdate, RmLink)));

  std::cout << "Algorithm: CFR_A" << std::endl;
  print_gaps_for_learner(hr_edl::CfTreeLearnerProfile(
      hr_edl::BehavioralDeviationTabularCfvLearner<
          hr_edl::BlindActionSequencePredecessors>::NewList(game->NumPlayers(),
                                                            RmUpdate, RmLink)));

  std::cout << "Algorithm: BHV EFR" << std::endl;
  print_gaps_for_learner(hr_edl::CfTreeLearnerProfile(
      hr_edl::BehavioralDeviationTabularCfvLearner<
          hr_edl::BehavioralPredecessors>::NewList(game->NumPlayers(), RmUpdate,
                                                   RmLink)));
}

int main(int argc, char** argv) {
  absl::SetProgramUsageMessage(
      "Shows mediated (corr) distribution convergence.");
  absl::ParseCommandLine(argc, argv);
  run_experiment();
}
