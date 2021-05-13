#include "hr_edl/spiel_extra.h"

#include "absl/container/flat_hash_set.h"
#include "hr_edl/containers.h"
#include "hr_edl/samplers.h"

namespace hr_edl {

static void _ForEachState(
    absl::flat_hash_set<std::string>& already_observed,
    const open_spiel::State& state,
    const std::function<void(const open_spiel::State&)>& f, int player,
    const std::function<void(const open_spiel::State&)>& at_terminals) {
  if (state.IsTerminal()) {
    at_terminals(state);
    return;
  } else if (state.IsChanceNode()) {
    SampleAllChanceOutcomes(
        state, [&already_observed, &state, &f, player, &at_terminals](
                   const ActionAndProb& outcome, double importance_weight) {
          _ForEachState(already_observed, *state.Child(outcome.first), f,
                        player, at_terminals);
        });
    return;
  }
  const std::string info_state = state.InformationStateString();
  if (PlayerInSet(state.CurrentPlayer(), player)) {
    if (!already_observed.contains(info_state)) {
      f(state);
      already_observed.emplace(std::move(info_state));
    };
  }
  for (auto action : state.LegalActions()) {
    _ForEachState(already_observed, *state.Child(action), f, player,
                  at_terminals);
  }
}

void ForEachState(
    const open_spiel::State& state,
    const std::function<void(const open_spiel::State&)>& f, int player,
    const std::function<void(const open_spiel::State&)>& at_terminals) {
  absl::flat_hash_set<std::string> already_observed;
  _ForEachState(already_observed, state, f, player, at_terminals);
}

int NumStates(const open_spiel::State& state, int player) {
  int count = 0;
  ForEachState(
      state, [&count](const open_spiel::State& successor) { ++count; }, player);
  return count;
}

ActionMap<int> NumStatesWithAction(const open_spiel::State& state, int player) {
  ActionMap<int> counts(state.NumDistinctActions(), 0);
  ForEachState(
      state,
      [&counts](const open_spiel::State& successor) {
        for (open_spiel::Action a : successor.LegalActions()) {
          ++counts[a];
        }
      },
      player);
  return counts;
}

std::vector<open_spiel::Player> Observers(const open_spiel::State& prev,
                                          const open_spiel::State& next) {
  std::vector<open_spiel::Player> observers;
  for (open_spiel::Player player = 0; player < prev.NumPlayers(); ++player) {
    if (!DoesObserve(player, prev, next)) {
      observers.push_back(player);
    }
  }
  return observers;
}
}  // namespace hr_edl
