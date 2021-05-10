#ifndef HR_EDL_TYPES_H_
#define HR_EDL_TYPES_H_

#include <string>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/container/node_hash_map.h"
#include "open_spiel/spiel.h"
#include "hr_edl/math.h"

namespace open_spiel {
using StatePtr = std::unique_ptr<open_spiel::State>;
}

namespace hr_edl {
using ActionAndProb = std::pair<open_spiel::Action, double>;
using Cfv = double;
using CfActionValues = std::vector<Cfv>;
struct CfValues {
  CfValues() = default;
  CfValues(CfValues&&) = default;
  CfValues(const CfValues&) = default;
  CfValues(std::pair<CfActionValues, Cfv>&& pair) : v_(std::move(pair.first)), ev_(pair.second) {}
  CfValues(CfActionValues&& v, Cfv ev) : v_(std::move(v)), ev_(ev) {}
  CfValues(const CfActionValues& v, Cfv ev) : v_(v), ev_(ev) {}
  CfValues(size_t num_actions)
      : v_(std::vector<double>(num_actions, 0)), ev_(0) {}

  void Reset() {
    v_.assign(v_.size(), 0);
    ev_ = 0;
  }
  size_t Size() const { return v_.size(); }
  double operator()(const std::vector<double>& policy) const {
    return InnerProduct(v_, policy);
  }
  double operator()() const { return ev_; }
  double Regret(const std::vector<double>& policy) const {
    return this->operator()(policy) - ev_;
  }
  Cfv& operator[](size_t i) { return v_[i]; }
  Cfv operator[](size_t i) const { return v_[i]; }

  CfActionValues v_;
  Cfv ev_;
};

template <class... Args>
using UVolMap = absl::flat_hash_map<Args...>;
template <class... Args>
using UnorderedMap = absl::node_hash_map<Args...>;

template <class T>
using InfoStateUvm = UVolMap<std::string, T>;
template <class T>
using InfoStateUm = UnorderedMap<std::string, T>;
template <class T>
using ActionMap = std::vector<T>;
template <class T>
using PlayerMap = std::vector<T>;

template <class T>
inline ActionMap<T> NewActionMap(const open_spiel::Game& game) {
  return ActionMap<T>(game.NumDistinctActions());
}

template <class T>
inline PlayerMap<T> NewPlayerMap(const open_spiel::Game& game) {
  return PlayerMap<T>(game.NumPlayers());
}

template <class T>
inline PlayerMap<ActionMap<T>> NewPlayerActionMap(
    const open_spiel::Game& game) {
  return PlayerMap<ActionMap<T>>(game.NumPlayers(), NewActionMap<T>(game));
}

using ActionIdx = size_t;
using LegalActions = std::vector<open_spiel::Action>;
using LegalActionValues = std::vector<double>;

template <class T>
using ActionsAndValues = std::vector<std::pair<open_spiel::Action, T>>;

struct CfValueTreeNode {
  CfValueTreeNode(size_t num_actions)
      : cf_values_(num_actions),
        child_keys_(num_actions, std::vector<std::string>()) {}

  void Reset() { cf_values_.Reset(); }
  CfValues cf_values_;
  std::vector<std::vector<std::string>> child_keys_;
};
}  // namespace hr_edl

#endif  // HR_EDL_TYPES_H_
