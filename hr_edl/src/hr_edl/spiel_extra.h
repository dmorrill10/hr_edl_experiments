#ifndef HR_EDL_SPIEL_EXTRA_H_
#define HR_EDL_SPIEL_EXTRA_H_

#include <algorithm>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/container/node_hash_map.h"
#include "open_spiel/spiel.h"
#include "hr_edl/types.h"

namespace hr_edl {

// A player_set_indicator of i >= 0 indicates {i} while -i indicates the set of
// players that excludes i - 1.
inline bool PlayerInSet(open_spiel::Player player, int player_set_indicator) {
  return player_set_indicator < 0 ? player != (-player_set_indicator - 1)
                                  : player == player_set_indicator;
}

constexpr int ALL_PLAYERS = std::numeric_limits<int>::lowest();

void ForEachState(
    const open_spiel::State& state,
    const std::function<void(const open_spiel::State&)>& f,
    int player = ALL_PLAYERS,
    const std::function<void(const open_spiel::State&)>& at_terminals =
        [](const open_spiel::State& _) {});

int NumStates(const open_spiel::State& state, int player = ALL_PLAYERS);
ActionMap<int> NumStatesWithAction(const open_spiel::State& state,
                                   int player = ALL_PLAYERS);

template <typename T>
std::ostream& Print(const std::vector<T>& v, std::ostream& stream = std::cout) {
  stream << "[";
  for (const auto& element : v) {
    stream << element << " ";
  }
  stream << "]";
  return stream;
}

inline bool DoesObserve(const open_spiel::Player player,
                        const open_spiel::State& prev,
                        const open_spiel::State& next) {
  return prev.InformationStateString(player) ==
         next.InformationStateString(player);
};
std::vector<open_spiel::Player> Observers(const open_spiel::State& prev,
                                          const open_spiel::State& next);

inline void Wait(std::thread& thread) {
  SPIEL_CHECK_TRUE(thread.joinable());
  thread.join();
}

inline void Wait(std::vector<std::thread>& threads) {
  for (auto& thread : threads) {
    Wait(thread);
  }
  threads.clear();
}

inline void Wait(std::queue<std::thread>& threads) {
  while (!threads.empty()) {
    Wait(threads.front());
    threads.pop();
  }
}

template <class Clonable>
inline std::vector<std::unique_ptr<Clonable>> Clone(
    const std::vector<std::unique_ptr<Clonable>>& profile) {
  std::vector<std::unique_ptr<Clonable>> clone;
  clone.reserve(profile.size());
  for (const auto& learner : profile) {
    clone.push_back(learner->Clone());
  }
  return clone;
}

template <class TNew, class TOld, class... Args>
std::unique_ptr<TNew> Upgrade(std::unique_ptr<TOld>&& instance, Args... args) {
  return std::make_unique<TNew>(std::move(instance), args...);
}

class MultiplicativeCheckpointMarker {
 public:
  MultiplicativeCheckpointMarker(double factor, size_t skip = 0)
      : factor_(factor), skip_(skip) {}

  bool IsCheckpoint(size_t t, size_t num_checkpoints) const {
    return (!(factor_ > 1.0)) ||
           ((t > skip_) &&
            (std::log(t - skip_) / std::log(factor_) >= num_checkpoints));
  }

 private:
  const double factor_;
  const size_t skip_;
};
}  // namespace hr_edl

#endif  // HR_EDL_SPIEL_EXTRA_H_
