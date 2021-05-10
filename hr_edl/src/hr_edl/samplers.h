#ifndef HR_EDL_SAMPLERS_H_
#define HR_EDL_SAMPLERS_H_

#include <functional>
#include <memory>
#include <random>
#include <vector>

#include "absl/strings/match.h"
#include "open_spiel/spiel.h"
#include "hr_edl/types.h"

namespace hr_edl {

int SampleActionIndex(const std::vector<double>& policy, double random_number,
                      double epsilon = 0);
int SampleActionIndex(const open_spiel::ActionsAndProbs& actions_and_probs,
                      double random_number);
void SampleAllChanceOutcomes(
    const open_spiel::State& state,
    const std::function<void(const ActionAndProb&, double)>& f);
void SampleOneChanceOutcome(
    double random_number, const open_spiel::State& state,
    const std::function<void(const ActionAndProb&, double)>& f);
void SampleAllTargetPlayerActions(
    const std::vector<double>& policy,
    const std::function<void(int action_idx, double policy_prob,
                             double sampling_prob)>& f);
void SampleOneTargetPlayerAction(
    double random_number, const std::vector<double>& policy,
    const std::function<void(int action_idx, double policy_prob,
                             double sampling_prob)>& f,
    double epsilon = 0);
void SampleAllExternalPlayerActions(
    const std::vector<double>& policy,
    const std::function<void(int action_idx, double policy_prob,
                             double sampling_prob)>& f);
void SampleOneExternalPlayerAction(
    double random_number, const std::vector<double>& policy,
    const std::function<void(int action_idx, double policy_prob,
                             double sampling_prob)>& f);

class /* interface */ MccfrSampler {
 public:
  virtual ~MccfrSampler() = default;

  virtual void SampleChanceOutcomes(
      const open_spiel::State& state,
      const std::function<void(const ActionAndProb&, double)>& f) = 0;
  virtual void SampleChanceOutcomes(
      const std::vector<double>& outcome_probs,
      const std::function<void(int action_idx, double policy_prob,
                               double sampling_prob)>& f) = 0;
  virtual void SampleTargetPlayerActions(
      const std::vector<double>& policy,
      const std::function<void(int action_idx, double policy_prob,
                               double sampling_prob)>& f) = 0;
  virtual void SampleExternalPlayerActions(
      const std::vector<double>& policy,
      const std::function<void(int action_idx, double policy_prob,
                               double sampling_prob)>& f) = 0;
};
using MccfrSamplerPtr = std::unique_ptr<MccfrSampler>;

class NullSampler : public MccfrSampler {
 public:
  NullSampler() {}
  void SampleChanceOutcomes(
      const open_spiel::State& state,
      const std::function<void(const ActionAndProb&, double)>& f)
      override final {
    SampleAllChanceOutcomes(state, f);
  }
  void SampleChanceOutcomes(
      const std::vector<double>& outcome_probs,
      const std::function<void(int action_idx, double policy_prob,
                               double sampling_prob)>& f) override final {
    SampleAllExternalPlayerActions(outcome_probs, f);
  }
  void SampleTargetPlayerActions(
      const std::vector<double>& policy,
      const std::function<void(int action_idx, double policy_prob,
                               double sampling_prob)>& f) override final {
    SampleAllTargetPlayerActions(policy, f);
  }

  void SampleExternalPlayerActions(
      const std::vector<double>& policy,
      const std::function<void(int action_idx, double policy_prob,
                               double sampling_prob)>& f) override final {
    SampleAllExternalPlayerActions(policy, f);
  }
};

class ChanceSampler : public MccfrSampler {
 public:
  ChanceSampler(std::shared_ptr<std::mt19937_64> random_engine)
      : random_engine_(std::move(random_engine)), uniform_dist_(0, 1) {}

  void SampleChanceOutcomes(
      const open_spiel::State& state,
      const std::function<void(const ActionAndProb&, double)>& f)
      override final {
    SampleOneChanceOutcome(uniform_dist_(*random_engine_), state, f);
  }
  void SampleChanceOutcomes(
      const std::vector<double>& outcome_probs,
      const std::function<void(int action_idx, double policy_prob,
                               double sampling_prob)>& f) override final {
    SampleOneExternalPlayerAction(uniform_dist_(*random_engine_), outcome_probs,
                                  f);
  }

  void SampleTargetPlayerActions(
      const std::vector<double>& policy,
      const std::function<void(int action_idx, double policy_prob,
                               double sampling_prob)>& f) override final {
    SampleAllTargetPlayerActions(policy, f);
  }

  void SampleExternalPlayerActions(
      const std::vector<double>& policy,
      const std::function<void(int action_idx, double policy_prob,
                               double sampling_prob)>& f) override final {
    SampleAllExternalPlayerActions(policy, f);
  }

 private:
  std::shared_ptr<std::mt19937_64> random_engine_;
  std::uniform_real_distribution<double> uniform_dist_;
};

class ExternalSampler : public MccfrSampler {
 public:
  ExternalSampler(std::shared_ptr<std::mt19937_64> random_engine)
      : random_engine_(std::move(random_engine)), uniform_dist_(0, 1) {}

  void SampleChanceOutcomes(
      const open_spiel::State& state,
      const std::function<void(const ActionAndProb&, double)>& f)
      override final {
    SampleOneChanceOutcome(uniform_dist_(*random_engine_), state, f);
  }
  void SampleChanceOutcomes(
      const std::vector<double>& outcome_probs,
      const std::function<void(int action_idx, double policy_prob,
                               double sampling_prob)>& f) override final {
    SampleOneExternalPlayerAction(uniform_dist_(*random_engine_), outcome_probs,
                                  f);
  }

  void SampleTargetPlayerActions(
      const std::vector<double>& policy,
      const std::function<void(int action_idx, double policy_prob,
                               double sampling_prob)>& f) override final {
    SampleAllTargetPlayerActions(policy, f);
  }

  void SampleExternalPlayerActions(
      const std::vector<double>& policy,
      const std::function<void(int action_idx, double policy_prob,
                               double sampling_prob)>& f) override final {
    SampleOneExternalPlayerAction(uniform_dist_(*random_engine_), policy, f);
  }

 private:
  std::shared_ptr<std::mt19937_64> random_engine_;
  std::uniform_real_distribution<double> uniform_dist_;
};

class OutcomeSampler : public MccfrSampler {
 public:
  OutcomeSampler(std::shared_ptr<std::mt19937_64> random_engine,
                 double epsilon = 0)
      : random_engine_(std::move(random_engine)),
        epsilon_(epsilon),
        uniform_dist_(0, 1) {}

  void SampleChanceOutcomes(
      const open_spiel::State& state,
      const std::function<void(const ActionAndProb&, double)>& f)
      override final {
    SampleOneChanceOutcome(uniform_dist_(*random_engine_), state, f);
  }
  void SampleChanceOutcomes(
      const std::vector<double>& outcome_probs,
      const std::function<void(int action_idx, double policy_prob,
                               double sampling_prob)>& f) override final {
    SampleOneExternalPlayerAction(uniform_dist_(*random_engine_), outcome_probs,
                                  f);
  }

  void SampleTargetPlayerActions(
      const std::vector<double>& policy,
      const std::function<void(int action_idx, double policy_prob,
                               double sampling_prob)>& f) override final {
    SampleOneTargetPlayerAction(uniform_dist_(*random_engine_), policy, f,
                                epsilon_);
  }

  void SampleExternalPlayerActions(
      const std::vector<double>& policy,
      const std::function<void(int action_idx, double policy_prob,
                               double sampling_prob)>& f) override final {
    SampleOneExternalPlayerAction(uniform_dist_(*random_engine_), policy, f);
  }

 private:
  std::shared_ptr<std::mt19937_64> random_engine_;
  double epsilon_;
  std::uniform_real_distribution<double> uniform_dist_;
};

inline MccfrSamplerPtr NewSampler(const std::string& sampler, int random_seed) {
  if (absl::StrContains("null", sampler)) {
    return MccfrSamplerPtr(new NullSampler());
  }
  auto rng = std::make_shared<std::mt19937_64>(random_seed);
  if (absl::StrContains("chance", sampler)) {
    return MccfrSamplerPtr(new ChanceSampler(rng));
  } else if (absl::StrContains("external", sampler)) {
    return MccfrSamplerPtr(new ExternalSampler(rng));
  } else if (absl::StrContains("outcome", sampler)) {
    return MccfrSamplerPtr(new OutcomeSampler(rng, 0.6));
  }
  return nullptr;
}

}  // namespace hr_edl

#endif  // HR_EDL_SAMPLERS_H_
