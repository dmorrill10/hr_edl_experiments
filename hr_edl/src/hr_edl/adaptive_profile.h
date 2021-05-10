#ifndef HR_EDL_ADAPTIVE_PROFILE_H_
#define HR_EDL_ADAPTIVE_PROFILE_H_

#include "hr_edl/best_response.h"
#include "hr_edl/decision_point.h"
#include "hr_edl/tabular_learner.h"
#include "hr_edl/policy_evaluation.h"
#include "hr_edl/samplers.h"
#include "hr_edl/types.h"

namespace hr_edl {
class AdaptiveProfile;
using AdaptiveProfilePtr = std::unique_ptr<AdaptiveProfile>;

class AdaptiveProfile {
 public:
  virtual ~AdaptiveProfile() = default;

  virtual Policy* Strategy(size_t player) const = 0;
  Cfv Ev(DecisionPoint& root, MccfrSampler& sampler,
         const PolicyProfile& compatriots) const {
    double avg = 0.0;
    for (int player = 0; player < root.NumPlayers(); ++player) {
      const auto [v, _] = PolicyValue(
          root, player, compatriots.WithSubstitute(Strategy(player), player),
          sampler);
      avg += (v - avg) / (player + 1.0);
    }
    return avg;
  }
  virtual Cfv UpdateAndReturnEv(DecisionPoint& root, MccfrSampler& sampler,
                                const PolicyProfile& compatriots) = 0;
  virtual Cfv UpdateAlternateAndReturnEv(DecisionPoint& root,
                                         MccfrSampler& sampler) = 0;

  virtual PolicyProfile Frozen() const = 0;
  MapPolicy ToMapPolicy(DecisionPoint& root) const {
    return MapPolicy(Frozen(), root);
  }
};

class CfTreeLearnerProfile : public AdaptiveProfile {
 public:
  CfTreeLearnerProfile(std::vector<CfValueTreeLearnerPtr>&& learners)
      : learners_(std::move(learners)), evaluators_(NewEvaluators()) {}

  Policy* Strategy(size_t player) const override final {
    return learners_[player].get();
  }
  Cfv UpdateAndReturnEv(DecisionPoint& root, MccfrSampler& sampler,
                        const PolicyProfile& compatriots) override final {
    double avg = 0.0;
    for (int player = 0; player < root.NumPlayers(); ++player) {
      const auto [v, initial_info_states, cf_value_tree, _] =
          evaluators_[player].ComputeCfValueTreeEvaluation(
              root, compatriots.WithSubstitute(Strategy(player), player),
              sampler);
      learners_[player]->Update(initial_info_states, *cf_value_tree);
      avg += (v - avg) / (player + 1.0);
    }
    return avg;
  }

  Cfv UpdateAlternateAndReturnEv(DecisionPoint& root,
                                 MccfrSampler& sampler) override final {
    double avg = 0.0;
    for (int player = 0; player < root.NumPlayers(); ++player) {
      const auto [v, initial_info_states, cf_value_tree, _] =
          evaluators_[player].ComputeCfValueTreeEvaluation(
              root, PolicyRefProfile(learners_), sampler);
      learners_[player]->Update(initial_info_states, *cf_value_tree);
      avg += (v - avg) / (player + 1.0);
    }
    return avg;
  }
  PolicyProfile Frozen() const override final { return Clone(learners_); }

 private:
  std::vector<PolicyCfValueTreeEvaluator> NewEvaluators() {
    std::vector<PolicyCfValueTreeEvaluator> l;
    for (size_t i = 0; i < learners_.size(); ++i) {
      l.emplace_back(i);
    }
    return l;
  }

 private:
  std::vector<CfValueTreeLearnerPtr> learners_;
  std::vector<PolicyCfValueTreeEvaluator> evaluators_;
};

class MapPolicyAdaptiveProfile : public AdaptiveProfile {
 public:
  MapPolicyAdaptiveProfile(size_t num_players) : AdaptiveProfile(), profile_() {
    profile_.reserve(num_players);
    for (size_t i = 0; i < num_players; ++i) {
      profile_.emplace_back(new MapPolicy());
    }
  }

  Policy* Strategy(size_t player) const override final {
    return profile_[player].get();
  }
  PolicyProfile Frozen() const override final { return Clone(profile_); }

 protected:
  std::vector<MapPolicyPtr> profile_;
};

class PolicyIterationProfile : public MapPolicyAdaptiveProfile {
 public:
  PolicyIterationProfile(size_t num_players)
      : MapPolicyAdaptiveProfile(num_players) {}

  Cfv UpdateAndReturnEv(DecisionPoint& root, MccfrSampler& sampler,
                        const PolicyProfile& compatriots) override final {
    double avg = 0.0;
    for (int player = 0; player < root.NumPlayers(); ++player) {
      const auto [v, _] = PolicyValue(
          root, player, compatriots.WithSubstitute(Strategy(player), player),
          sampler);
      profile_[player].reset(new MapPolicy(
          BestResponse(compatriots).Policy(root, -player - 1).first));
      avg += (v - avg) / (player + 1.0);
    }
    return avg;
  }

  Cfv UpdateAlternateAndReturnEv(DecisionPoint& root,
                                 MccfrSampler& sampler) override final {
    double avg = 0.0;
    for (int player = 0; player < root.NumPlayers(); ++player) {
      const auto [v, _] =
          PolicyValue(root, player, PolicyRefProfile(profile_), sampler);
      profile_[player].reset(
          new MapPolicy(BestResponse(PolicyRefProfile(profile_))
                            .Policy(root, -player - 1)
                            .first));
      avg += (v - avg) / (player + 1.0);
    }
    return avg;
  }
};

class AntiPolicyIterationProfile : public MapPolicyAdaptiveProfile {
 public:
  AntiPolicyIterationProfile(size_t num_players)
      : MapPolicyAdaptiveProfile(num_players), policy_iteration_(num_players) {}

  Cfv UpdateAndReturnEv(DecisionPoint& root, MccfrSampler& sampler,
                        const PolicyProfile& compatriots) override final {
    double avg = 0.0;
    for (int player = 0; player < root.NumPlayers(); ++player) {
      const auto [v, _] = PolicyValue(
          root, player, compatriots.WithSubstitute(Strategy(player), player),
          sampler);
      avg += (v - avg) / (player + 1.0);
    }
    policy_iteration_.UpdateAndReturnEv(root, sampler, compatriots);
    const auto pi_profile = policy_iteration_.Frozen();

    for (int player = 0; player < root.NumPlayers(); ++player) {
      profile_[player].reset(new MapPolicy(
          BestResponse(pi_profile).Policy(root, -player - 1).first));
    }
    return avg;
  }

  Cfv UpdateAlternateAndReturnEv(DecisionPoint& root,
                                 MccfrSampler& sampler) override final {
    double avg = 0.0;
    for (int player = 0; player < root.NumPlayers(); ++player) {
      const auto [v, _] =
          PolicyValue(root, player, PolicyRefProfile(profile_), sampler);
      avg += (v - avg) / (player + 1.0);

      policy_iteration_.UpdateAndReturnEv(root, sampler, Frozen());
      profile_[player].reset(
          new MapPolicy(BestResponse(policy_iteration_.Frozen())
                            .Policy(root, -player - 1)
                            .first));
    }
    return avg;
  }

 private:
  PolicyIterationProfile policy_iteration_;
};

class BestResponseProfile : public MapPolicyAdaptiveProfile {
 public:
  BestResponseProfile(size_t num_players)
      : MapPolicyAdaptiveProfile(num_players) {}

  Cfv UpdateAndReturnEv(DecisionPoint& root, MccfrSampler& sampler,
                        const PolicyProfile& compatriots) override final {
    double avg = 0.0;
    for (int player = 0; player < root.NumPlayers(); ++player) {
      auto [policy, v] = BestResponse(compatriots).Policy(root, -player - 1);
      profile_[player].reset(new MapPolicy(std::move(policy)));
      avg += (v - avg) / (player + 1.0);
    }
    return avg;
  }

  Cfv UpdateAlternateAndReturnEv(DecisionPoint& root,
                                 MccfrSampler& sampler) override final {
    double avg = 0.0;
    for (int player = 0; player < root.NumPlayers(); ++player) {
      auto [policy, v] =
          BestResponse(PolicyRefProfile(profile_)).Policy(root, -player - 1);
      profile_[player].reset(new MapPolicy(std::move(policy)));
      avg += (v - avg) / (player + 1.0);
    }
    return avg;
  }
};

class FictitiousPlayProfile : public MapPolicyAdaptiveProfile {
 public:
  FictitiousPlayProfile(size_t num_players)
      : MapPolicyAdaptiveProfile(num_players), compatriot_empirical_play_() {}

  Cfv UpdateAndReturnEv(DecisionPoint& root, MccfrSampler& sampler,
                        const PolicyProfile& compatriots) override final {
    double avg = 0.0;
    for (int player = 0; player < root.NumPlayers(); ++player) {
      const auto [v, _] = PolicyValue(
          root, player, compatriots.WithSubstitute(Strategy(player), player),
          sampler);
      avg += (v - avg) / (player + 1.0);
    }
    compatriot_empirical_play_.Avg(compatriots, root);

    for (int player = 0; player < root.NumPlayers(); ++player) {
      profile_[player].reset(
          new MapPolicy(BestResponse(compatriot_empirical_play_)
                            .Policy(root, -player - 1)
                            .first));
    }
    return avg;
  }

  Cfv UpdateAlternateAndReturnEv(DecisionPoint& root,
                                 MccfrSampler& sampler) override final {
    double avg = 0.0;
    for (int player = 0; player < root.NumPlayers(); ++player) {
      const auto profile_ref = PolicyRefProfile(profile_);
      const auto [v, _] = PolicyValue(root, player, profile_ref, sampler);
      avg += (v - avg) / (player + 1.0);

      const int not_player = -player - 1;
      compatriot_empirical_play_.Avg(profile_ref, root, not_player);
      profile_[player].reset(
          new MapPolicy(BestResponse(compatriot_empirical_play_)
                            .Policy(root, not_player)
                            .first));
    }
    return avg;
  }

 private:
  MapPolicy compatriot_empirical_play_;
};
}  // namespace hr_edl

#endif  // HR_EDL_ADAPTIVE_PROFILE_H_
