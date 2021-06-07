// This file runs standard CFR on a .efg game.
//
// The graph from Sec 6.1 of Morrill et al. 2020, Hindsight and Sequential
// Rationality of Correlated Play (https://arxiv.org/abs/2012.05874) can be
// reproduced by running this code on efg/extended_shapleys.efg.

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
#include "hr_edl/adaptive_profile.h"
#include "hr_edl/best_response.h"
#include "hr_edl/decision_point.h"
#include "hr_edl/policy_evaluation.h"
#include "hr_edl/spiel_extra.h"
#include "open_spiel/algorithms/cfr.h"
#include "open_spiel/algorithms/corr_dev_builder.h"
#include "open_spiel/algorithms/corr_dist.h"
#include "open_spiel/game_transforms/turn_based_simultaneous_game.h"
#include "open_spiel/spiel.h"

// Environment
ABSL_FLAG(std::string, efg_file, "efg/extended_shapleys.efg",
          "The EFG file representing the game.");
ABSL_FLAG(int32_t, t, 1000, "The number of iterations to run.");

inline constexpr int kSeed = 23894982;

// Reporting
ABSL_FLAG(
    double, report_gap_factor, 2.0,
    "The factor increasing the number of iterations to finish between "
    "reporting exploitability. E.g. 1 prints after each iteration, 2 prints "
    "after 2^i iterations, i >= 1.");
ABSL_FLAG(int32_t, report_skip, 0,
          "Number of initial iterations for which to skip reporting.");

using open_spiel::algorithms::CorrDevBuilder;
using open_spiel::algorithms::CFRSolverBase;
using open_spiel::algorithms::CorrDistConfig;
using open_spiel::TabularPolicy;
using open_spiel::algorithms::CorrelationDevice;
using open_spiel::algorithms::AFCCEDist;
using open_spiel::algorithms::AFCEDist;
using open_spiel::algorithms::EFCCEDist;
using open_spiel::algorithms::EFCEDist;
using open_spiel::algorithms::ExpectedValues;
using open_spiel::algorithms::CFRCurrentPolicy;

void run_experiment() {
  const std::shared_ptr<const open_spiel::Game> game =
      open_spiel::LoadGameAsTurnBased(
          "efg_game", {{"filename", open_spiel::GameParameter(
                                        absl::GetFlag(FLAGS_efg_file))}});
  const int iterations = absl::GetFlag(FLAGS_t);

  CorrDevBuilder cd_builder;
  CFRSolverBase solver(*game,
                       /*alternating_updates=*/false,
                       /*linear_averaging=*/false,
                       /*regret_matching_plus=*/false,
                       /*random_initial_regrets*/ true,
                       /*seed*/kSeed);
  CorrDistConfig config;
  for (int i = 0; i < 10000000; i++) {
    solver.EvaluateAndUpdatePolicy();
    TabularPolicy current_policy =
        static_cast<CFRCurrentPolicy*>(solver.CurrentPolicy().get())
            ->AsTabular();
    cd_builder.AddMixedJointPolicy(current_policy);
    if (i < 100 || i % 100 == 0) {
      CorrelationDevice mu = cd_builder.GetCorrelationDevice();
      double afcce_dist = AFCCEDist(*game, config, mu);
      double afce_dist = AFCEDist(*game, config, mu);
      double efcce_dist = EFCCEDist(*game, config, mu);
      double efce_dist = EFCEDist(*game, config, mu);
      std::vector<double> values = ExpectedValues(*game, mu);
      std::cout << absl::StrFormat(
                       "%d %2.10lf %2.10lf %2.10lf, %2.10lf %2.3lf %2.3lf",
                       i, afcce_dist, afce_dist, efcce_dist, efce_dist,
                       values[0], values[1])
                << std::endl;
    }
  }
}

int main(int argc, char** argv) {
  absl::SetProgramUsageMessage(
      "Shows mediated (corr) distribution convergence.");
  absl::ParseCommandLine(argc, argv);
  run_experiment();
}
