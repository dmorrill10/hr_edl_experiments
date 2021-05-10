#include "hr_edl/best_response.h"

namespace hr_edl {
double BestResponse::CfReturn(const std::vector<double>& utils, double prob,
                              int policy_player) {
  double v = 0;
  for (open_spiel::Player compatriot = 0; compatriot < utils.size();
       ++compatriot) {
    if (!UsePolicy(compatriot, policy_player)) {
      v += utils.at(compatriot);
    }
  }
  return v * prob;
}

double BestResponse::Value(DecisionPoint& decision_point, int player) {
  double br_value = 0;
  Dfs(decision_point, &br_value, player);
  BackwardPassValue();
  return br_value;
}

double BestResponse::Value(const open_spiel::State& history, int player) {
  double br_value = 0;
  Dfs(history, &br_value, player);
  BackwardPassValue();
  return br_value;
}

std::pair<MapPolicy, double> BestResponse::Policy(DecisionPoint& decision_point,
                                                  int player) {
  double br_value = 0;
  Dfs(decision_point, &br_value, player);
  MapPolicy policy = BackwardPassPolicy();
  return {std::move(policy), br_value};
}

std::pair<MapPolicy, double> BestResponse::Policy(
    const open_spiel::State& history, int player) {
  double br_value = 0;
  Dfs(history, &br_value, player);
  MapPolicy policy = BackwardPassPolicy();
  return {std::move(policy), br_value};
}

void BestResponse::BackwardPassValue() const {
  for (int depth = iss_by_depth_.size() - 1; depth > -1; --depth) {
    for (const auto& iss : iss_by_depth_[depth]) {
      const auto& info_set = info_set_values_.at(iss);

      size_t br_action = 0;
      double max_value = std::numeric_limits<double>::lowest();
      for (size_t a = 0; a < info_set.second.size(); ++a) {
        const double q_a = info_set.second[a];
        if (q_a > max_value) {
          br_action = a;
          max_value = q_a;
        }
      }
      *info_set.first += info_set.second[br_action];
    }
  }
}

InfoStateUvm<std::vector<double>> BestResponse::BackwardPassMap() const {
  InfoStateUvm<std::vector<double>> policy;

  for (int depth = iss_by_depth_.size() - 1; depth > -1; --depth) {
    for (const auto& iss : iss_by_depth_[depth]) {
      const auto& info_set = info_set_values_.at(iss);

      size_t br_action = 0;
      double max_value = std::numeric_limits<double>::lowest();
      for (size_t a = 0; a < info_set.second.size(); ++a) {
        const double q_a = info_set.second[a];
        if (q_a > max_value) {
          br_action = a;
          max_value = q_a;
        }
      }
      std::vector<double> bp(info_set.second.size(), 0);
      bp[br_action] = 1.0;
      policy[iss] = std::move(bp);

      *info_set.first += info_set.second[br_action];
    }
  }
  return policy;
}

void BestResponse::RecursiveDfs(DecisionPoint& decision_point, size_t depth,
                                double prob, double* parent_value,
                                open_spiel::Player player, size_t action) {
  const auto& outcome_probs = decision_point.OutcomeProbabilities(action);
  for (size_t outcome = 0; outcome < outcome_probs.size(); ++outcome) {
    decision_point.Apply(action, outcome);
    RecursiveDfs(decision_point, depth, prob * outcome_probs[outcome],
                 parent_value, player);
    decision_point.Undo();
  }
}
void BestResponse::Dfs(DecisionPoint& decision_point, double* br_value,
                       int player) {
  iss_by_depth_.clear();
  info_set_values_.clear();
  RecursiveDfs(decision_point, 0, 1.0, br_value, player, 0);
}

double* BestResponse::NewParentValue(const std::string& iss,
                                     double* parent_value, size_t num_actions,
                                     size_t depth) {
  if (iss_by_depth_.size() <= depth) {
    iss_by_depth_.emplace_back();
  }
  iss_by_depth_[depth].push_back(iss);
  auto iter_status_pair = info_set_values_.emplace(
      std::piecewise_construct, std::forward_as_tuple(iss),
      std::forward_as_tuple(std::piecewise_construct,
                            std::forward_as_tuple(parent_value),
                            std::forward_as_tuple(num_actions, 0)));
  return iter_status_pair.first->second.second.data();
}

void BestResponse::RecursiveDfs(DecisionPoint& decision_point, size_t depth,
                                double prob, double* parent_value, int player) {
  if (decision_point.IsTerminal()) {
    *parent_value += CfReturn(decision_point.Returns(), prob, player);
  } else if (UsePolicy(decision_point.PlayerToAct(), player)) {
    const auto action_probs =
        policy_.Response(*decision_point.OpenSpielStatePtr());
    for (size_t a = 0; a < action_probs.size(); ++a) {
      if (action_probs[a] > 0) {
        RecursiveDfs(decision_point, depth, prob * action_probs[a],
                     parent_value, player, a);
      }
    }
  } else {
    const std::string& iss = decision_point.InformationStateStringRef();

    double* next_parent_value =
        info_set_values_.contains(iss)
            ? info_set_values_.at(iss).second.data()
            : NewParentValue(iss, parent_value, decision_point.NumActions(),
                             depth);

    for (size_t a = 0; a < decision_point.NumActions(); ++a) {
      RecursiveDfs(decision_point, depth + 1, prob, next_parent_value, player,
                   a);
      ++next_parent_value;
    }
  }
}

void BestResponse::Dfs(const open_spiel::State& history, double* br_value,
                       int player) {
  iss_by_depth_.clear();
  info_set_values_.clear();
  RecursiveDfs(history, 0, 1.0, br_value, player);
}

void BestResponse::RecursiveDfs(const open_spiel::State& h, size_t depth,
                                double prob, double* parent_value, int player) {
  if (h.IsTerminal()) {
    *parent_value += CfReturn(h.Returns(), prob, player);
  } else if (h.IsChanceNode()) {
    const auto co = h.ChanceOutcomes();
    for (const auto [outcome, prob_o] : co) {
      RecursiveDfs(*h.Child(outcome), depth, prob * prob_o, parent_value,
                   player);
    }
  } else if (UsePolicy(h.CurrentPlayer(), player)) {
    const auto action_probs = policy_.Response(h);
    const auto legal_actions = h.LegalActions();
    for (size_t a = 0; a < action_probs.size(); ++a) {
      const double prob_a = action_probs[a];
      if (prob_a > 0) {
        RecursiveDfs(*h.Child(legal_actions[a]), depth, prob * prob_a,
                     parent_value, player);
      }
    }
  } else {
    const std::string iss = h.InformationStateString();
    const auto legal_actions = h.LegalActions();

    double* next_parent_value;
    if (!info_set_values_.contains(iss)) {
      if (iss_by_depth_.size() <= depth) {
        iss_by_depth_.emplace_back();
      }
      iss_by_depth_[depth].push_back(iss);
      auto iter_status_pair = info_set_values_.emplace(
          std::piecewise_construct, std::forward_as_tuple(iss),
          std::forward_as_tuple(
              std::piecewise_construct, std::forward_as_tuple(parent_value),
              std::forward_as_tuple(legal_actions.size(), 0)));
      next_parent_value = iter_status_pair.first->second.second.data();
    } else {
      next_parent_value = info_set_values_[iss].second.data();
    }
    const size_t next_depth = depth + 1;
    for (const open_spiel::Action a : legal_actions) {
      RecursiveDfs(*h.Child(a), next_depth, prob, next_parent_value, player);
      ++next_parent_value;
    }
  }
}
}  // namespace hr_edl
