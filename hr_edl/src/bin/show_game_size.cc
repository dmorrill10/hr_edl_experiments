#include <memory>
#include <string>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/strings/match.h"
#include "absl/strings/str_format.h"
#include "open_spiel/game_transforms/turn_based_simultaneous_game.h"
#include "open_spiel/spiel.h"
#include "hr_edl/spiel_extra.h"

ABSL_FLAG(std::string, game, "leduc_poker", "The game.");

namespace osp = hr_edl;

void run_experiment() {
  const std::string game_name = absl::GetFlag(FLAGS_game);
  std::cout << "Loading " << game_name << std::endl;
  fflush(NULL);
  const std::shared_ptr<const open_spiel::Game> game =
      open_spiel::LoadGameAsTurnBased(game_name);
  std::cout << "Loaded" << std::endl;
  fflush(NULL);
  const auto root = game->NewInitialState();
  std::cout << "Created new initial state" << std::endl;
  fflush(NULL);

  std::vector<size_t> num_states;
  size_t total_num_states = 0;
  std::vector<size_t> min_depth;
  size_t total_min_depth;
  std::vector<size_t> max_depth;
  size_t total_max_depth;
  std::vector<size_t> min_num_actions;
  size_t total_min_num_actions;
  std::vector<size_t> max_num_actions;
  size_t total_max_num_actions;
  for (int player = 0; player < game->NumPlayers(); ++player) {
    size_t my_num_states = 0;
    osp::ForEachState(
        *root,
        [&my_num_states, &min_num_actions, &max_num_actions,
         player](const open_spiel::State& s) {
          const size_t num_actions = s.LegalActions().size();
          if (num_actions < 2) {
            return;
          }
          ++my_num_states;
          if (min_num_actions.size() < player + 1) {
            min_num_actions.push_back(num_actions);
            max_num_actions.push_back(num_actions);
          } else {
            if (min_num_actions[player] > num_actions) {
              min_num_actions[player] = num_actions;
            } else if (max_num_actions[player] < num_actions) {
              max_num_actions[player] = num_actions;
            }
          }
        },
        player,
        [&min_depth, &max_depth, player](const open_spiel::State& s) {
          size_t depth = 0;
          for (const auto& [acting_player, a] : s.FullHistory()) {
            if (acting_player == player) {
              ++depth;
            }
          }
          if (min_depth.size() < player + 1) {
            min_depth.push_back(depth);
            max_depth.push_back(depth);
          } else {
            if (min_depth[player] > depth) {
              min_depth[player] = depth;
            } else if (max_depth[player] < depth) {
              max_depth[player] = depth;
            }
          }
        });
    num_states.push_back(my_num_states);
    total_num_states += my_num_states;
    if (player == 0) {
      total_min_num_actions = min_num_actions[player];
      total_max_num_actions = max_num_actions[player];
      total_min_depth = min_depth[player];
      total_max_depth = max_depth[player];
    } else {
      if (min_num_actions[player] < total_min_num_actions) {
        total_min_num_actions = min_num_actions[player];
      }
      if (max_num_actions[player] > total_max_num_actions) {
        total_max_num_actions = max_num_actions[player];
      }
      if (min_depth[player] < total_min_depth) {
        total_min_depth = min_depth[player];
      }
      if (max_depth[player] > total_max_depth) {
        total_max_depth = max_depth[player];
      }
    }
  }
  std::cout << "# " << game_name << std::endl;
  std::cout << "# player  num_info_sets  min_|A|  max_|A|  min_depth  max_depth"
            << std::endl;

  for (int player = 0; player < game->NumPlayers(); ++player) {
    std::cout << absl::StrFormat("%d  %u  %u  %u  %u  %u", player + 1,
                                 num_states[player], min_num_actions[player],
                                 max_num_actions[player], min_depth[player],
                                 max_depth[player])
              << std::endl;
  }
  std::cout << absl::StrFormat("%d  %u  %u  %u  %u  %u", 0, total_num_states,
                               total_min_num_actions, total_max_num_actions,
                               total_min_depth, total_max_depth)
            << std::endl;
}

int main(int argc, char** argv) {
  absl::SetProgramUsageMessage("Show the size of the game for each player.");
  absl::ParseCommandLine(argc, argv);
  run_experiment();
}
