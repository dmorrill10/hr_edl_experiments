#include "hr_edl/samplers.h"

#include "open_spiel/spiel_utils.h"

namespace hr_edl {

int SampleActionIndex(const std::vector<double>& policy, double random_number,
                      double epsilon) {
  double cumulative_prob = 0;
  int aidx = 0;
  for (; aidx < policy.size(); ++aidx) {
    const double prob =
        epsilon / policy.size() + (1.0 - epsilon) * policy[aidx];
    cumulative_prob += prob;
    if (cumulative_prob > random_number) {
      return aidx;
    }
  }
  return aidx - 1;
}

int SampleActionIndex(const open_spiel::ActionsAndProbs& actions_and_probs,
                      double random_number) {
  double cumulative_prob = 0;
  int a = 0;
  for (; a < actions_and_probs.size(); ++a) {
    cumulative_prob += actions_and_probs[a].second;
    if (cumulative_prob > random_number) {
      return a;
    }
  }
  return a - 1;
}

void SampleAllChanceOutcomes(
    const open_spiel::State& state,
    const std::function<void(const ActionAndProb&, double)>& f) {
  for (const auto outcome : state.ChanceOutcomes()) {
    f(outcome, outcome.second);
  }
}

void SampleOneChanceOutcome(
    double random_number, const open_spiel::State& state,
    const std::function<void(const ActionAndProb&, double)>& f) {
  const auto outcomes = state.ChanceOutcomes();
  f(outcomes[SampleActionIndex(outcomes, random_number)], 1.0);
}

void SampleAllTargetPlayerActions(
    const std::vector<double>& policy,
    const std::function<void(int action_idx, double policy_prob,
                             double sampling_prob)>& f) {
  for (int action_idx = 0; action_idx < policy.size(); ++action_idx) {
    f(action_idx, policy[action_idx], 1.0);
  }
}

void SampleOneTargetPlayerAction(
    double random_number, const std::vector<double>& policy,
    const std::function<void(int action_idx, double policy_prob,
                             double sampling_prob)>& f,
    double epsilon) {
  const int action_idx = SampleActionIndex(policy, random_number, epsilon);
  const double p = policy[action_idx];
  f(action_idx, p, (1 - epsilon) * p + epsilon / policy.size());
}

void SampleAllExternalPlayerActions(
    const std::vector<double>& policy,
    const std::function<void(int action_idx, double policy_prob,
                             double sampling_prob)>& f) {
  for (int action_idx = 0; action_idx < policy.size(); ++action_idx) {
    f(action_idx, policy[action_idx], 1.0);
  }
}

void SampleOneExternalPlayerAction(
    double random_number, const std::vector<double>& policy,
    const std::function<void(int action_idx, double policy_prob,
                             double sampling_prob)>& f) {
  const int action_idx = SampleActionIndex(policy, random_number);
  const double p = policy[action_idx];
  f(action_idx, p, p);
}

}  // namespace hr_edl
