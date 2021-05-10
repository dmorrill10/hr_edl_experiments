#include <memory>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/strings/match.h"
#include "absl/strings/str_format.h"
#include "open_spiel/spiel.h"

ABSL_FLAG(std::string, game, "leduc_poker", "The game to solve.");

void run_experiment() {
  const std::string game_name = absl::GetFlag(FLAGS_game);
  const std::shared_ptr<const open_spiel::Game> game =
      open_spiel::LoadGame(game_name);

  const double diameter = game->MaxUtility() - game->MinUtility();
  const double radius = diameter / 2.0;

  std::cout << absl::StrFormat("[%0.2g, %0.2g] or [-%0.2g, +%0.2g]\n",
                               game->MinUtility(), game->MaxUtility(), radius,
                               radius);
}

int main(int argc, char** argv) {
  absl::SetProgramUsageMessage(
      "Show the minimum and maximum utility for the game in both natural and "
      "centered form.");
  absl::ParseCommandLine(argc, argv);
  run_experiment();
}
