#include "hr_edl/decision_point.h"

#include "hr_edl/containers.h"
#include "hr_edl/samplers.h"

namespace hr_edl {

double DecisionPoint::ApplySampledOutcome(size_t action, double random_number) {
  const std::vector<double>& pi = OutcomeProbabilities(action);
  const size_t outcome = SampleActionIndex(pi, random_number);
  const double prob = pi[outcome];
  Apply(action, outcome);
  return prob;
}

CachedDecisionPoint::CachedDecisionPoint(open_spiel::StatePtr&& root,
                                         bool save_root, bool save_terminals)
    : DecisionPoint(root->NumPlayers(), root->NumDistinctActions()),
      idx_(0),
      histories_(),
      save_terminals_(save_terminals) {
  if (root->IsTerminal()) {
    if (save_terminals_) {
      histories_.emplace_back(std::move(root), idx_);
    } else {
      histories_.emplace_back(root->Returns(), idx_);
    }
  } else if (root->IsChanceNode() && save_root) {
    histories_.emplace_back(std::move(root));
    histories_[idx_].outcomes_.emplace_back();
    RecursiveCache(*(histories_[idx_].os_state_), 1.0, 0);
  } else {
    histories_.emplace_back();
    histories_[idx_].outcomes_.emplace_back();
    RecursiveCache(std::move(root), 1.0, 0);
  }
}
void CachedDecisionPoint::RecursiveCache(const open_spiel::State& child,
                                         double prob, size_t aidx) {
  for (const auto [outcome, next_prob] : child.ChanceOutcomes()) {
    RecursiveCache(child.Child(outcome), prob * next_prob, aidx);
  }
}
void CachedDecisionPoint::RecursiveCache(open_spiel::StatePtr&& child,
                                         double prob, size_t aidx) {
  if (child->IsChanceNode()) {
    RecursiveCache(*child, prob, aidx);
    return;
  } else if (!child->IsTerminal()) {
    const auto actions = child->LegalActions();
    if (actions.size() < 2) {
      RecursiveCache(child->Child(actions[0]), prob, aidx);
      return;
    }
  } else if (!save_terminals_) {
    histories_[idx_].outcomes_[aidx].PushBack(histories_.size(), prob);
    histories_.emplace_back(child->Returns(), idx_);
    return;
  }
  // Non-trivial decision node or terminal to be saved.
  histories_[idx_].outcomes_[aidx].PushBack(histories_.size(), prob);
  histories_.emplace_back(std::move(child), idx_);
}
void CachedDecisionPoint::CacheOutcomes() {
  if (IsTerminal() || histories_[idx_].outcomes_.size() > 0) {
    return;
  }
  const auto actions = histories_[idx_].os_state_->LegalActions();
  for (size_t aidx = 0; aidx < actions.size(); ++aidx) {
    histories_[idx_].outcomes_.emplace_back();
    RecursiveCache(histories_[idx_].os_state_->Child(actions[aidx]), 1.0, aidx);
  }
}

void _ForEachState(std::unordered_set<std::string>& already_observed,
                   DecisionPoint& decision_point,
                   const std::function<void(const DecisionPoint&)>& f,
                   int player) {
  if (decision_point.IsTerminal()) {
    return;
  }
  const std::string info_state = decision_point.InformationStateString();
  if (player < 0 || player == decision_point.PlayerToAct()) {
    if (!Contains(already_observed, info_state)) {
      f(decision_point);
      already_observed.insert(std::move(info_state));
    };
  }
  for (size_t a = 0; a < decision_point.NumActions(); ++a) {
    for (size_t outcome = 0; outcome < decision_point.NumOutcomes(a);
         ++outcome) {
      decision_point.Apply(a, outcome);
      _ForEachState(already_observed, decision_point, f, player);
      decision_point.Undo();
    }
  }
}

void ForEachState(DecisionPoint& root,
                  const std::function<void(const DecisionPoint&)>& f,
                  int player) {
  std::unordered_set<std::string> already_observed;
  if (root.NumActions() < 2) {
    for (size_t outcome = 0; outcome < root.NumOutcomes(0); ++outcome) {
      root.Apply(0, outcome);
      _ForEachState(already_observed, root, f, player);
      root.Undo();
    }
  } else {
    _ForEachState(already_observed, root, f, player);
  }
}

int NumStates(DecisionPoint& root, int player) {
  int count = 0;
  ForEachState(
      root, [&count](const DecisionPoint& _) { ++count; }, player);
  return count;
}

ActionMap<int> NumStatesWithAction(DecisionPoint& root, int player) {
  ActionMap<int> counts(root.NumDistinctActions(), 0);
  ForEachState(
      root,
      [&counts](const DecisionPoint& successor) {
        for (const open_spiel::Action a :
             successor.OpenSpielStatePtr()->LegalActions()) {
          ++counts[a];
        }
      },
      player);
  return counts;
}

}  // namespace hr_edl
