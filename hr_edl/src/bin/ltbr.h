#ifndef LTBR_H_
#define LTBR_H_

#include <functional>
#include <vector>

#include "absl/strings/str_format.h"
#include "hr_edl/adaptive_profile.h"
#include "hr_edl/tabular_learner.h"
#include "open_spiel/matrix_game.h"

inline double RmUpdate(double prev_regret, double next_regret, size_t _) {
  return next_regret;
}
inline void RmLink(const std::vector<double>& regrets,
                   const hr_edl::EnumerationConsumer<double>& yield) {
  for (size_t i = 0; i < regrets.size(); ++i) {
    yield(i, hr_edl::Relu(regrets[i]));
  }
}

struct LabeledAdaptiveProfile {
  std::string label_;
  const std::function<hr_edl::AdaptiveProfilePtr(size_t num_players,
                                                 double utility_diameter)>
      New;
};

inline std::vector<std::string> LearnerProfileLabels(
    const std::vector<LabeledAdaptiveProfile>& algs) {
  std::vector<std::string> labels;
  labels.reserve(algs.size());
  for (const auto& a : algs) {
    labels.push_back(a.label_);
  }
  return labels;
}

inline std::vector<hr_edl::AdaptiveProfilePtr> NewLearnerProfiles(
    const std::vector<LabeledAdaptiveProfile>& algs, size_t num_players,
    double utility_diameter) {
  std::vector<hr_edl::AdaptiveProfilePtr> learners;
  learners.reserve(algs.size());
  for (const auto& a : algs) {
    learners.push_back(a.New(num_players, utility_diameter));
  }
  return learners;
}

const std::vector<LabeledAdaptiveProfile> ltbr_cfr_algs{
    LabeledAdaptiveProfile{
        "CFR",
        [](size_t num_players, double utility_diameter) {
          return hr_edl::AdaptiveProfilePtr(new hr_edl::CfTreeLearnerProfile(
              hr_edl::BehavioralDeviationTabularCfvLearner<
                  hr_edl::ImmediateExternalSequencePredecessors>::
                  NewList(num_players, RmUpdate, RmLink)));
        }},
    LabeledAdaptiveProfile{
        "CFR_EX+IN",
        [](size_t num_players, double utility_diameter) {
          return hr_edl::AdaptiveProfilePtr(new hr_edl::CfTreeLearnerProfile(
              hr_edl::BehavioralDeviationTabularCfvLearner<
                  hr_edl::ImmediateExInSequencePredecessors>::
                  NewList(num_players, RmUpdate, RmLink)));
        }},
    LabeledAdaptiveProfile{
        "CFR_IN",
        [](size_t num_players, double utility_diameter) {
          return hr_edl::AdaptiveProfilePtr(new hr_edl::CfTreeLearnerProfile(
              hr_edl::BehavioralDeviationTabularCfvLearner<
                  hr_edl::ImmediateInternalSequencePredecessors>::
                  NewList(num_players, RmUpdate, RmLink)));
        }},
    LabeledAdaptiveProfile{
        "A-EFR_IN",
        [](size_t num_players, double utility_diameter) {
          return hr_edl::AdaptiveProfilePtr(new hr_edl::CfTreeLearnerProfile(
              hr_edl::BehavioralDeviationTabularCfvLearner<
                  hr_edl::InformedActionSequencePredecessors>::
                  NewList(num_players, RmUpdate, RmLink)));
        }},
    LabeledAdaptiveProfile{
        "BPS-EFR",
        [](size_t num_players, double utility_diameter) {
          return hr_edl::AdaptiveProfilePtr(new hr_edl::CfTreeLearnerProfile(
              hr_edl::BehavioralDeviationTabularCfvLearner<
                  hr_edl::BlindPartialSequencePredecessors>::
                  NewList(num_players, RmUpdate, RmLink)));
        }},
    LabeledAdaptiveProfile{
        "TIPS-EFR",
        [](size_t num_players, double utility_diameter) {
          return hr_edl::AdaptiveProfilePtr(new hr_edl::CfTreeLearnerProfile(
              hr_edl::BehavioralDeviationTabularCfvLearner<
                  hr_edl::TwiceInformedPartialSequencePredecessors>::
                  NewList(num_players, RmUpdate, RmLink)));
        }},
    LabeledAdaptiveProfile{
        "CSPS-EFR",
        [](size_t num_players, double utility_diameter) {
          return hr_edl::AdaptiveProfilePtr(new hr_edl::CfTreeLearnerProfile(
              hr_edl::BehavioralDeviationTabularCfvLearner<
                  hr_edl::CausalPartialSequencePredecessors>::
                  NewList(num_players, RmUpdate, RmLink)));
        }},
    LabeledAdaptiveProfile{
        "CFPS-EFR",
        [](size_t num_players, double utility_diameter) {
          return hr_edl::AdaptiveProfilePtr(new hr_edl::CfTreeLearnerProfile(
              hr_edl::BehavioralDeviationTabularCfvLearner<
                  hr_edl::CounterfactualPartialSequencePredecessors>::
                  NewList(num_players, RmUpdate, RmLink)));
        }},
    LabeledAdaptiveProfile{
        "CFPS-EFR_EX+IN",
        [](size_t num_players, double utility_diameter) {
          return hr_edl::AdaptiveProfilePtr(new hr_edl::CfTreeLearnerProfile(
              hr_edl::BehavioralDeviationTabularCfvLearner<
                  hr_edl::CounterfactualPartialSequenceExInPredecessors>::
                  NewList(num_players, RmUpdate, RmLink)));
        }},
    LabeledAdaptiveProfile{
        "TIPS-EFR_EX+IN", [](size_t num_players, double utility_diameter) {
          return hr_edl::AdaptiveProfilePtr(new hr_edl::CfTreeLearnerProfile(
              hr_edl::BehavioralDeviationTabularCfvLearner<
                  hr_edl::TwiceInformedPartialSequenceExInPredecessors>::
                  NewList(num_players, RmUpdate, RmLink)));
        }}};

const std::vector<LabeledAdaptiveProfile> expensive_cfr_algs{
    LabeledAdaptiveProfile{
        "BEHAV-EFR", [](size_t num_players, double utility_diameter) {
          return hr_edl::AdaptiveProfilePtr(new hr_edl::CfTreeLearnerProfile(
              hr_edl::BehavioralDeviationTabularCfvLearner<
                  hr_edl::BehavioralPredecessors>::NewList(num_players,
                                                           RmUpdate, RmLink)));
        }}};

inline std::vector<LabeledAdaptiveProfile> AlgsInGroup(size_t alg_group) {
  std::vector<LabeledAdaptiveProfile> algs;
  for (const auto& alg : ltbr_cfr_algs) {
    algs.push_back(alg);
  }
  if (alg_group > 0) {
    for (const auto& alg : expensive_cfr_algs) {
      algs.push_back(alg);
    }
  }
  return algs;
}
#endif  // LTBR_H_
