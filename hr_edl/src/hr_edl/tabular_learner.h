#ifndef HR_EDL_TABULAR_LEARNER_H_
#define HR_EDL_TABULAR_LEARNER_H_

#include <memory>

#include "hr_edl/action_transformation.h"

namespace hr_edl {

namespace _tl {
class ImmediatePolicy {
 public:
  virtual ~ImmediatePolicy() = default;
  virtual std::vector<double> Response() const = 0;
  virtual void Response(double* out) const = 0;
  virtual size_t NumActions() const = 0;
};

class CachedImmediatePolicy : public ImmediatePolicy {
 public:
  CachedImmediatePolicy(size_t num_actions)
      : policy_(num_actions, 1.0 / num_actions) {}
  CachedImmediatePolicy(std::vector<double>&& policy)
      : policy_(std::move(policy)) {}
  virtual ~CachedImmediatePolicy() = default;

  std::vector<double> Response() const override final { return policy_; }
  void Response(double* out) const override final {
    assert(out);
    std::memcpy(out, policy_.data(), policy_.size() * sizeof(*out));
  }
  size_t NumActions() const override final { return policy_.size(); }

 protected:
  std::vector<double> policy_;
};

using RegretTransformation = std::function<double(
    double prev_regret, double regret, size_t round_number)>;
using LinkFn = std::function<void(const std::vector<double>&,
                                  const EnumerationConsumer<double>&)>;

struct ReachProbLists {
  const std::vector<double> prev_;
  const std::vector<double> next_;
  size_t Size() const { return prev_.size(); }
};

class ImmediateDecisionInfo : public CachedImmediatePolicy {
 public:
  ImmediateDecisionInfo(size_t num_actions, size_t ex_num_pred_reach_probs,
                        size_t in_num_pred_reach_probs)
      : CachedImmediatePolicy(num_actions),
        round_number_(1),
        ex_phi_list_(ex_num_pred_reach_probs > 0
                         ? SwapActionTranformation::External(num_actions)
                         : std::vector<SwapActionTranformation>()),
        in_phi_list_(in_num_pred_reach_probs > 0
                         ? SwapActionTranformation::Internal(num_actions)
                         : std::vector<SwapActionTranformation>()),
        ex_regrets_(ex_phi_list_.size() * ex_num_pred_reach_probs, 0),
        in_regrets_(in_phi_list_.size() * in_num_pred_reach_probs, 0),
        phi_sum_(num_actions) {}
  virtual ~ImmediateDecisionInfo() = default;

  void Update(const CfValues& cfvs, const RegretTransformation& update_target,
              const LinkFn& f, const ReachProbLists& ex_pred_reach_prob_lists,
              const ReachProbLists& in_pred_reach_prob_lists) {
    if (ex_pred_reach_prob_lists.Size() > 0) {
      UpdatePhiSetRegrets(ex_regrets_, ex_phi_list_, cfvs, update_target, f,
                          ex_pred_reach_prob_lists);
    }
    if (in_pred_reach_prob_lists.Size() > 0) {
      UpdatePhiSetRegrets(in_regrets_, in_phi_list_, cfvs, update_target, f,
                          in_pred_reach_prob_lists);
    }
    phi_sum_.FixedPoint(policy_);
    phi_sum_.Reset();
    ++round_number_;
  }

 private:
  void UpdatePhiSetRegrets(std::vector<double>& regrets,
                           const std::vector<SwapActionTranformation>& phi_list,
                           const CfValues& cfvs,
                           const RegretTransformation& update_target,
                           const LinkFn& f,
                           const ReachProbLists& pred_reach_prob_lists) {
    const size_t num_reach_probs = pred_reach_prob_lists.prev_.size();
    assert(num_reach_probs == pred_reach_prob_lists.next_.size());
    size_t idx = 0;
    for (size_t i = 0; i < phi_list.size(); ++i) {
      const double regret = phi_list[i].Regret(cfvs, policy_);
      for (size_t j = 0; j < num_reach_probs; ++j) {
        assert(idx < regrets.size());
        regrets[idx] +=
            update_target(regrets[idx], pred_reach_prob_lists.prev_[j] * regret,
                          round_number_);
        ++idx;
      }
    }
    size_t reach_prob_idx = 0;
    size_t phi_idx = 0;
    double sum = 0;
    f(regrets, [this, num_reach_probs, &reach_prob_idx,
                p_next = pred_reach_prob_lists.next_.data(), &sum, &phi_idx,
                &phi_list](size_t regret_idx, double link_output) {
      sum += p_next[reach_prob_idx] * link_output;
      ++reach_prob_idx;
      if (reach_prob_idx == num_reach_probs) {
        assert(phi_idx < phi_list.size());
        phi_sum_.Add(phi_list[phi_idx], sum);
        ++phi_idx;
        sum = 0;
        reach_prob_idx = 0;
      }
    });
  }

 private:
  size_t round_number_;
  const std::vector<SwapActionTranformation> ex_phi_list_;
  const std::vector<SwapActionTranformation> in_phi_list_;
  std::vector<double> ex_regrets_;
  std::vector<double> in_regrets_;
  WeightedActionTransformation phi_sum_;
};
}  // namespace _tl

struct NoExternal {
  std::vector<double> ExternalPredecessorReachProbs(
      const std::vector<double>& pred_reach_probs) const {
    return {};
  }
};
struct AllExternal {
  std::vector<double> ExternalPredecessorReachProbs(
      const std::vector<double>& pred_reach_probs) const {
    return pred_reach_probs;
  }
};
struct NoInternal {
  std::vector<double> InternalPredecessorReachProbs(
      const std::vector<double>& pred_reach_probs) const {
    return {};
  }
};
struct AllInternal {
  std::vector<double> InternalPredecessorReachProbs(
      const std::vector<double>& pred_reach_probs) const {
    return pred_reach_probs;
  }
};
struct CounterfactualSuccessors {
  std::vector<double> SuccessorReachProbs(
      const std::vector<double>& pred_reach_probs, const double* strat,
      size_t num_actions, size_t action) const {
    return {1.0};
  }
};

struct ImmediateInternalSequencePredecessors : public NoExternal,
                                               public AllInternal,
                                               public CounterfactualSuccessors {
};

struct ImmediateExternalSequencePredecessors : public AllExternal,
                                               public NoInternal,
                                               public CounterfactualSuccessors {
};

struct ImmediateExInSequencePredecessors : public AllExternal,
                                           public AllInternal,
                                           public CounterfactualSuccessors {};

struct IdentitySuccessors {
  std::vector<double> SuccessorReachProbs(
      const std::vector<double>& pred_reach_probs, const double* strat,
      size_t num_actions, size_t action) const {
    return {pred_reach_probs[0] * strat[action]};
  }
};

struct InformedActionSequencePredecessors : public NoExternal,
                                            public AllInternal,
                                            public IdentitySuccessors {};

struct BlindActionSequencePredecessors : public AllExternal,
                                         public NoInternal,
                                         public IdentitySuccessors {};

struct BlindPartialSequenceSuccessors {
  std::vector<double> SuccessorReachProbs(
      const std::vector<double>& pred_reach_probs, const double* strat,
      size_t num_actions, size_t action) const {
    std::vector<double> succ_reach_probs = pred_reach_probs;  // External
    succ_reach_probs.push_back(pred_reach_probs[pred_reach_probs.size() - 1] *
                               strat[action]);  // Identity
    return succ_reach_probs;
  }
};

struct BlindPartialSequencePredecessors
    : public AllExternal,
      public NoInternal,
      public BlindPartialSequenceSuccessors {};

struct CounterfactualPartialSequencePredecessors
    : public NoExternal,
      public AllInternal,
      public BlindPartialSequenceSuccessors {};

struct CounterfactualPartialSequenceExInPredecessors
    : public AllExternal,
      public AllInternal,
      public BlindPartialSequenceSuccessors {};

struct CausalSuccessors {
  std::vector<double> SuccessorReachProbs(
      const std::vector<double>& pred_reach_probs, const double* strat,
      size_t num_actions, size_t action) const {
    std::vector<double> succ_reach_probs = pred_reach_probs;  // External
    succ_reach_probs.reserve(pred_reach_probs.size() + num_actions);
    const double identity_seq = pred_reach_probs[pred_reach_probs.size() - 1];
    for (size_t a = 0; a < num_actions; ++a) {  // Internal
      if (a != action) {
        succ_reach_probs.push_back(identity_seq * strat[a]);
      }
    }
    succ_reach_probs.push_back(identity_seq * strat[action]);  // Identity
    return succ_reach_probs;
  }
};

struct CausalPartialSequencePredecessors : public CausalSuccessors {
  std::vector<double> ExternalPredecessorReachProbs(
      const std::vector<double>& pred_reach_probs) const {
    std::vector<double> p(pred_reach_probs.size() - 1);
    std::copy(pred_reach_probs.begin(), pred_reach_probs.end() - 1, p.begin());
    return p;
  }
  std::vector<double> InternalPredecessorReachProbs(
      const std::vector<double>& pred_reach_probs) const {
    return {pred_reach_probs[pred_reach_probs.size() - 1]};
  }
};

struct TwiceInformedPartialSequencePredecessors : public NoExternal,
                                                  public AllInternal,
                                                  public CausalSuccessors {};

struct TwiceInformedPartialSequenceExInPredecessors : public AllExternal,
                                                      public AllInternal,
                                                      public CausalSuccessors {
};

struct BehavioralPredecessors : public NoExternal, public AllInternal {
  std::vector<double> SuccessorReachProbs(
      const std::vector<double>& pred_reach_probs, const double* strat,
      size_t num_actions, size_t action) const {
    std::vector<double> succ_reach_probs;
    succ_reach_probs.reserve(pred_reach_probs.size() * num_actions);
    for (auto p : pred_reach_probs) {
      for (size_t a = 0; a < num_actions; ++a) {
        succ_reach_probs.push_back(p * strat[a]);
      }
    }
    return succ_reach_probs;
  }
};

class CfValueTreeLearner;
using CfValueTreeLearnerPtr = std::unique_ptr<CfValueTreeLearner>;
class CfValueTreeLearner : public virtual Policy {
 public:
  template <class Subclass, class... Args>
  static std::vector<CfValueTreeLearnerPtr> NewList(
      size_t num_players, Args... args) {
    std::vector<CfValueTreeLearnerPtr> learners;
    learners.reserve(num_players);
    for (int i = 0; i < num_players; ++i) {
      learners.emplace_back(new Subclass(args...));
    }
    return learners;
  }

 public:
  virtual ~CfValueTreeLearner() = default;
  virtual void Update(const std::vector<std::string>& initial_info_states,
                      const InfoStateUvm<CfValueTreeNode>& cf_value_tree) = 0;
  virtual CfValueTreeLearnerPtr Clone() const = 0;
};

template <class T>  // Requires T#Response()
class TabularResponder : public virtual Policy {
 public:
  TabularResponder() : local_info_map_() {}
  virtual ~TabularResponder() = default;

  std::vector<double> Response(
      const open_spiel::State& state) const override final {
    const std::string info_state = state.InformationStateString();
    if (local_info_map_.contains(info_state)) {
      return local_info_map_.at(info_state).Response();
    } else {
      const int n = state.LegalActions().size();
      return std::vector<double>(n, 1.0 / n);
    }
  }

 protected:
  InfoStateUvm<T> local_info_map_;
};

template <class DeviationSequencePredecessors>
// Concept DeviationSequencePredecessors requires
// virtual std::vector<double> ExternalPredecessorReachProbs(
//     const std::vector<double>& pred_reach_probs) const;
// virtual std::vector<double> InternalPredecessorReachProbs(
//     const std::vector<double>& pred_reach_probs) const;
// virtual std::vector<double> SuccessorReachProbs(
//     const std::vector<double>& pred_reach_probs, const double* strat,
//     size_t num_actions, size_t action) const;
class BehavioralDeviationTabularCfvLearner
    : public CfValueTreeLearner,
      public TabularResponder<_tl::ImmediateDecisionInfo> {
 public:
  template <class... Args>
  static std::vector<CfValueTreeLearnerPtr> NewList(
      size_t num_players, Args... args) {
    return CfValueTreeLearner::NewList<
        BehavioralDeviationTabularCfvLearner>(
        num_players, DeviationSequencePredecessors(), args...);
  }

 private:
  struct StackState {
    const std::string* info_state_string_;
    const _tl::ReachProbLists* reach_prob_lists_;

    void Reset(const std::string* info_state_string,
               const _tl::ReachProbLists* reach_prob_lists) {
      info_state_string_ = info_state_string;
      reach_prob_lists_ = reach_prob_lists;
    }
  };

 public:
  BehavioralDeviationTabularCfvLearner(
      DeviationSequencePredecessors dev_seq_predecessors,
      _tl::RegretTransformation&& update_target, _tl::LinkFn&& f)
      : TabularResponder(),
        update_target_(std::move(update_target)),
        f_(std::move(f)),
        dev_seq_predecessors_(std::move(dev_seq_predecessors)) {}
  virtual ~BehavioralDeviationTabularCfvLearner() = default;

  void Update(
      const std::vector<std::string>& initial_info_states,
      const InfoStateUvm<CfValueTreeNode>& cf_value_tree) override final {
    std::vector<StackState> state_stack(cf_value_tree.size());
    const auto initial_reach_prob_lists = new _tl::ReachProbLists{{1.0}, {1.0}};
    int stack_idx = 0;
    for (; stack_idx < initial_info_states.size(); ++stack_idx) {
      state_stack[stack_idx].Reset(&(initial_info_states[stack_idx]),
                                   initial_reach_prob_lists);
    }
    --stack_idx;
    std::vector<_tl::ReachProbLists*> all_reach_prob_lists = {
        initial_reach_prob_lists};
    all_reach_prob_lists.reserve(cf_value_tree.size());

    while (stack_idx >= 0) {
      const auto [info_state_string_ptr, pred_reach_probs] =
          state_stack[stack_idx];
      --stack_idx;

      const auto& [cf_values, child_keys] =
          cf_value_tree.at(*info_state_string_ptr);
      const size_t num_actions = cf_values.Size();

      if (num_actions < 2) {
        for (size_t child_idx = 0; child_idx < child_keys[0].size();
             ++child_idx) {
          ++stack_idx;
          state_stack[stack_idx].Reset(&(child_keys[0][child_idx]),
                                       pred_reach_probs);
        }
        continue;
      }
      const _tl::ReachProbLists ex_reach_probs = {
          dev_seq_predecessors_.ExternalPredecessorReachProbs(
              pred_reach_probs->prev_),
          dev_seq_predecessors_.ExternalPredecessorReachProbs(
              pred_reach_probs->next_)};
      const _tl::ReachProbLists in_reach_probs = {
          dev_seq_predecessors_.InternalPredecessorReachProbs(
              pred_reach_probs->prev_),
          dev_seq_predecessors_.InternalPredecessorReachProbs(
              pred_reach_probs->next_)};

      auto& local_decision_info = GetOrCreateFromArgs(
          local_info_map_, *info_state_string_ptr, num_actions,
          ex_reach_probs.Size(), in_reach_probs.Size());

      double prev_policy[num_actions];
      local_decision_info.Response(prev_policy);

      local_decision_info.Update(cf_values, update_target_, f_, ex_reach_probs,
                                 in_reach_probs);

      double next_policy[num_actions];
      local_decision_info.Response(next_policy);
      for (size_t a = 0; a < num_actions; ++a) {
        if (child_keys[a].size() < 1) {
          continue;
        }
        const auto successor_reach_prob_lists = new _tl::ReachProbLists{
            dev_seq_predecessors_.SuccessorReachProbs(
                pred_reach_probs->prev_, prev_policy, num_actions, a),
            dev_seq_predecessors_.SuccessorReachProbs(
                pred_reach_probs->next_, next_policy, num_actions, a)};
        for (size_t child_idx = 0; child_idx < child_keys[a].size();
             ++child_idx) {
          ++stack_idx;
          state_stack[stack_idx].Reset(&(child_keys[a][child_idx]),
                                       successor_reach_prob_lists);
        }
        all_reach_prob_lists.push_back(successor_reach_prob_lists);
      }
    }
    for (auto ptr : all_reach_prob_lists) {
      delete ptr;
    }
  }
  CfValueTreeLearnerPtr Clone() const override final {
    return CfValueTreeLearnerPtr(
        new BehavioralDeviationTabularCfvLearner(*this));
  }

 private:
  const _tl::RegretTransformation update_target_;
  const _tl::LinkFn f_;
  const DeviationSequencePredecessors dev_seq_predecessors_;
};

}  // namespace hr_edl

#endif  // HR_EDL_TABULAR_LEARNER_H_
