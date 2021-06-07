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
#include "open_spiel/algorithms/cfr.h"
#include "open_spiel/algorithms/corr_dev_builder.h"
#include "open_spiel/algorithms/corr_dist.h"
#include "open_spiel/game_transforms/turn_based_simultaneous_game.h"
#include "open_spiel/spiel.h"

// Environment
ABSL_FLAG(std::string, efg_file, "efg/extended_shapleys.efg",
          "The EFG file representing the game.");
ABSL_FLAG(int32_t, t, 10000000, "The number of iterations to run.");
ABSL_FLAG(int32_t, report_freq, 100, "Number of iterations between reports.");
ABSL_FLAG(bool, random_initial_regrets, false, 
          "Initialize CFR tables with random initial regret?");
ABSL_FLAG(int32_t, seed, 23894971, "Seed for the random initial regrets.");
ABSL_FLAG(bool, alternating_updates, false,
          "Use alternating regret updates in CFR?");

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
  const int report_freq = absl::GetFlag(FLAGS_report_freq);
  const bool alternating_updates = absl::GetFlag(FLAGS_alternating_updates);
  const bool random_initial_regrets =
      absl::GetFlag(FLAGS_random_initial_regrets);
  const int seed = absl::GetFlag(FLAGS_seed);

  CorrDevBuilder cd_builder;
  CFRSolverBase solver(*game, alternating_updates, /*linear_averaging=*/false,
                       /*regret_matching_plus=*/false, random_initial_regrets,
                       seed);
  CorrDistConfig config;
  for (int i = 0; i < iterations; i++) {
    solver.EvaluateAndUpdatePolicy();
    TabularPolicy current_policy =
        static_cast<CFRCurrentPolicy*>(solver.CurrentPolicy().get())
            ->AsTabular();
    cd_builder.AddMixedJointPolicy(current_policy);
    if (i < 100 || i % report_freq == 0) {
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
      "Shows distance of CFR average joint to various correlated eq. classes.");
  absl::ParseCommandLine(argc, argv);
  run_experiment();
}
