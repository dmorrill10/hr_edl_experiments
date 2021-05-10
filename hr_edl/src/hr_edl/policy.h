#ifndef HR_EDL_POLICY_H_
#define HR_EDL_POLICY_H_

#include "open_spiel/policy.h"
#include "open_spiel/spiel.h"
#include "hr_edl/containers.h"
#include "hr_edl/decision_point.h"
#include "hr_edl/enumeration.h"
#include "hr_edl/math.h"
#include "hr_edl/types.h"

namespace hr_edl {

class Policy : public open_spiel::Policy {
 public:
  virtual ~Policy() = default;
  virtual std::vector<double> Response(
      const open_spiel::State& state) const = 0;
  open_spiel::ActionsAndProbs GetStatePolicy(
      const open_spiel::State& state) const override final {
    const auto legal_actions = state.LegalActions();
    open_spiel::ActionsAndProbs actions_and_probs;
    actions_and_probs.reserve(legal_actions.size());

    const auto policy = Response(state);
    for (int a = 0; a < legal_actions.size(); ++a) {
      actions_and_probs.emplace_back(legal_actions[a], policy[a]);
    }
    return actions_and_probs;
  }
};
using PolicyPtr = std::unique_ptr<Policy>;

inline std::unordered_map<std::string, open_spiel::ActionsAndProbs>
ActionsAndProbsTable(const open_spiel::Game& game, const Policy& policy) {
  std::unordered_map<std::string, open_spiel::ActionsAndProbs> map;
  ForEachState(*(game.NewInitialState()),
               [&map, &policy](const open_spiel::State& state) {
                 const auto iss = state.InformationStateString();
                 if (!Contains(map, iss)) {
                   map[iss] = policy.GetStatePolicy(state);
                 }
               });
  return map;
}
inline std::unordered_map<std::string, open_spiel::ActionsAndProbs>
ActionsAndProbsTable(DecisionPoint& root, const Policy& policy) {
  std::unordered_map<std::string, open_spiel::ActionsAndProbs> map;
  ForEachState(
      root,
      [&map, &policy](const DecisionPoint& dp) {
        const auto iss = dp.InformationStateString();
        if (!Contains(map, iss)) {
          map[iss] = policy.GetStatePolicy(*dp.OpenSpielStatePtr());
        }
      },
      ALL_PLAYERS);
  return map;
}

class ResponsePolicyAdapter : public Policy {
 public:
  ResponsePolicyAdapter(std::unique_ptr<open_spiel::Policy>&& policy)
      : policy_(std::move(policy)) {}
  std::vector<double> Response(
      const open_spiel::State& state) const override final {
    const auto action_prob_pairs = policy_->GetStatePolicy(state);
    std::vector<double> response(action_prob_pairs.size());
    for (int i = 0; i < action_prob_pairs.size(); ++i) {
      response[i] = action_prob_pairs[i].second;
    }
    return response;
  }

  open_spiel::ActionsAndProbs GetStatePolicy(
      const std::string& info_state) const override final {
    return policy_->GetStatePolicy(info_state);
  }

 private:
  std::unique_ptr<open_spiel::Policy> policy_;
};

class MapPolicy;
using MapPolicyPtr = std::unique_ptr<MapPolicy>;

class MapPolicy : public Policy {
 public:
  MapPolicy() : map_({}) {}
  MapPolicy(InfoStateUvm<std::vector<double>>&& map) : map_(std::move(map)) {}
  MapPolicy(const InfoStateUm<std::vector<double>>& map) {
    map_.reserve(map.size());
    for (const auto& [k, v] : map) {
      map_.emplace(k, v);
    }
  }
  MapPolicy(const InfoStateUvm<std::vector<double>>& map) : map_(map) {}
  MapPolicy(const Policy& policy, DecisionPoint& root) : map_() {
    Avg(policy, root);
  }

  std::vector<double> Response(
      const open_spiel::State& state) const override final {
    const std::string iss = state.InformationStateString();
    if (map_.contains(iss)) {
      auto policy = map_.at(iss);
      double z = 0;
      for (const auto prob : policy) {
        z += prob;
      }
      if (z > 1.0 || z < 1.0) {
        SafeDivide(policy, z, true);
      }
      return policy;
    } else {
      const size_t num_actions = state.LegalActions().size();
      return std::vector<double>(num_actions, 1.0 / num_actions);
    }
  }

  MapPolicyPtr Clone() const { return std::make_unique<MapPolicy>(map_); }

  void Avg(const Policy& other, DecisionPoint& root, double weight = 1.0,
           int player = ALL_PLAYERS) {
    // Compute their sequence weights and add them to the current sequence
    // weights in map_
    std::vector<double> their_reach_probs(root.NumPlayers(), 1.0);
    InfoStateUvm<std::vector<double>> their_seq_probs;
    for (size_t outcome_idx = 0; outcome_idx < root.NumOutcomes(0);
         ++outcome_idx) {
      root.Apply(0, outcome_idx);
      Avg_r(their_seq_probs, their_reach_probs, other, root, player);
      root.Undo();
    }
    for (const auto& [iss, seq_probs] : their_seq_probs) {
      const size_t num_actions = seq_probs.size();
      auto& policy_ref = GetOrCreate<std::string, std::vector<double>>(
          map_, iss,
          [num_actions]() { return std::vector<double>(num_actions, 0); });
      for (size_t action_idx = 0; action_idx < num_actions; ++action_idx) {
        policy_ref[action_idx] += weight * seq_probs[action_idx];
      }
    }
  }

 private:
  void Avg_r(InfoStateUvm<std::vector<double>>& their_seq_probs,
             std::vector<double>& their_reach_probs, const Policy& other,
             DecisionPoint& decision_point, int player) const {
    if (decision_point.IsTerminal()) {
      return;
    }
    const auto player_to_act = decision_point.PlayerToAct();
    const double their_reach_prob = their_reach_probs[player_to_act];
    const auto their_policy =
        other.Response(*decision_point.OpenSpielStatePtr());

    std::string iss = decision_point.InformationStateString();
    if (PlayerInSet(player_to_act, player) && !their_seq_probs.contains(iss)) {
      std::vector<double> seq_probs;
      seq_probs.reserve(their_policy.size());
      for (size_t action_idx = 0; action_idx < their_policy.size();
           ++action_idx) {
        seq_probs.push_back(their_reach_prob * their_policy[action_idx]);
      }
      their_seq_probs.emplace(std::move(iss), std::move(seq_probs));
    }
    for (size_t action_idx = 0; action_idx < their_policy.size();
         ++action_idx) {
      for (size_t outcome_idx = 0;
           outcome_idx < decision_point.NumOutcomes(action_idx);
           ++outcome_idx) {
        their_reach_probs[player_to_act] =
            their_reach_prob * their_policy[action_idx];
        decision_point.Apply(action_idx, outcome_idx);
        Avg_r(their_seq_probs, their_reach_probs, other, decision_point,
              player);
        decision_point.Undo();
      }
    }
    their_reach_probs[player_to_act] = their_reach_prob;
  }

 private:
  mutable InfoStateUvm<std::vector<double>> map_;
};

inline MapPolicy UniformRandomPolicy() { return MapPolicy(); }

class PolicyRefProfile;

class PlayerMapProfile : public Policy {
 public:
  PlayerMapProfile(size_t num_players) : player_map_(Range(num_players)) {}
  PlayerMapProfile(std::vector<size_t>&& player_map)
      : player_map_(std::move(player_map)) {}
  PlayerMapProfile(const std::vector<size_t>& player_map)
      : player_map_(player_map) {}

  std::vector<double> Response(
      const open_spiel::State& state) const override final {
    return (*this)[state.CurrentPlayer()]->Response(state);
  }
  virtual const Policy* operator[](size_t player) const = 0;

 protected:
  size_t NumPlayers() const { return player_map_.size(); }
  virtual std::vector<const Policy*> Policies() const = 0;

 protected:
  std::vector<size_t> player_map_;
};

class PolicyRefProfile : public PlayerMapProfile {
 public:
  PolicyRefProfile(std::vector<const Policy*>&& policies)
      : PlayerMapProfile(policies.size()), policies_(std::move(policies)) {}
  PolicyRefProfile(std::vector<const Policy*>&& policies,
                   std::vector<size_t>&& player_map)
      : PlayerMapProfile(std::move(player_map)),
        policies_(std::move(policies)) {}
  PolicyRefProfile(const std::vector<const Policy*>& policies)
      : PlayerMapProfile(policies.size()), policies_(policies) {}
  PolicyRefProfile(const std::vector<const Policy*>& policies,
                   const std::vector<size_t>& player_map)
      : PlayerMapProfile(player_map), policies_(policies) {}
  template <class PolicyLike>
  PolicyRefProfile(const std::vector<std::unique_ptr<PolicyLike>>& policies)
      : PlayerMapProfile(policies.size()) {
    policies_.reserve(policies.size());
    for (size_t i = 0; i < policies.size(); ++i) {
      policies_.push_back(static_cast<const Policy*>(policies[i].get()));
    }
  }
  template <class PolicyLike>
  PolicyRefProfile(const Policy* learner,
                   const std::vector<std::unique_ptr<PolicyLike>>& compatriots,
                   size_t learner_idx)
      : PlayerMapProfile(compatriots.size()) {
    policies_.reserve(compatriots.size());
    for (size_t player = 0; player < compatriots.size(); ++player) {
      policies_.push_back(
          (player == learner_idx)
              ? learner
              : static_cast<const Policy*>(compatriots[player].get()));
    }
  }

  PolicyRefProfile WithSubstitute(const Policy* learner,
                                  size_t learner_idx) const {
    std::vector<const Policy*> policies = Policies();
    auto player_map = player_map_;
    player_map[learner_idx] = policies.size();
    policies.push_back(learner);
    return PolicyRefProfile(std::move(policies), std::move(player_map));
  }

  const Policy* operator[](size_t player) const override {
    return policies_[player_map_[player]];
  }

 protected:
  std::vector<const Policy*> Policies() const override { return policies_; }

 private:
  std::vector<const Policy*> policies_;
};

class PolicyProfile : public PlayerMapProfile {
 public:
  PolicyProfile(std::vector<PolicyPtr>&& policies)
      : PlayerMapProfile(policies.size()), policies_(std::move(policies)) {}
  PolicyProfile(PolicyProfile&& profile)
      : PolicyProfile(std::move(profile.policies_)) {}
  template <class PolicyLike>
  PolicyProfile(std::vector<std::unique_ptr<PolicyLike>>&& policies)
      : PlayerMapProfile(policies.size()) {
    policies_.reserve(policies.size());
    for (size_t i = 0; i < policies.size(); ++i) {
      policies_.push_back(PolicyPtr(std::move(policies[i])));
    }
  }

  PolicyRefProfile WithSubstitute(const Policy* learner,
                                  size_t learner_idx) const {
    std::vector<const Policy*> policies = Policies();
    auto player_map = player_map_;
    player_map[learner_idx] = policies.size();
    policies.push_back(learner);
    return PolicyRefProfile(std::move(policies), std::move(player_map));
  }

  const Policy* operator[](size_t player) const override {
    return policies_[player_map_[player]].get();
  }

 protected:
  std::vector<const Policy*> Policies() const override {
    std::vector<const Policy*> policies;
    policies.reserve(policies_.size());
    for (size_t i = 0; i < policies_.size(); ++i) {
      policies.push_back(policies_[i].get());
    }
    return policies;
  }

 private:
  std::vector<PolicyPtr> policies_;
};

}  // namespace hr_edl

#endif  // HR_EDL_POLICY_H_
