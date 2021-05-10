#ifndef HR_EDL_DECISION_POINT_H_
#define HR_EDL_DECISION_POINT_H_

#include <unordered_map>
#include <vector>

#include "open_spiel/spiel.h"
#include "hr_edl/spiel_extra.h"

namespace hr_edl {

// A private namespace.
namespace _decision_point {
class Outcomes {
 public:
  Outcomes() = default;
  ~Outcomes() = default;

  void PushBack(size_t idx, double prob) {
    indices_.push_back(idx);
    probs_.push_back(prob);
  }
  size_t Size() const { return indices_.size(); }
  const std::vector<double>& Probs() const { return probs_; }
  size_t Idx(size_t i) const { return indices_[i]; }

 private:
  std::vector<size_t> indices_;
  std::vector<double> probs_;
};

class HistoryCache {
 public:
  HistoryCache()  // Artificial root
      : os_state_(nullptr),
        parent_idx_(0),
        info_state_key_(),
        player_to_act_(-1),
        outcomes_(),
        returns_() {}
  HistoryCache(std::vector<double>&& returns, size_t parent_idx)  // Terminal
      : os_state_(nullptr),
        parent_idx_(parent_idx),
        info_state_key_(),
        player_to_act_(-1),
        outcomes_(),
        returns_(std::move(returns)) {
    SPIEL_CHECK_TRUE(IsTerminal());
  }
  HistoryCache(open_spiel::StatePtr&& os_state)  // Chance root
      : os_state_(std::move(os_state)),
        parent_idx_(0),
        info_state_key_(),
        player_to_act_(-1),
        outcomes_(),
        returns_(os_state_->IsTerminal() ? os_state_->Returns()
                                         : std::vector<double>()) {
    SPIEL_CHECK_TRUE(os_state_->IsChanceNode());
  }
  HistoryCache(open_spiel::StatePtr&& os_state, size_t parent_idx)
      : os_state_(std::move(os_state)),
        parent_idx_(parent_idx),
        info_state_key_(
            os_state_->IsTerminal() ? "" : os_state_->InformationStateString()),
        player_to_act_(os_state_->IsTerminal() ? -1
                                               : os_state_->CurrentPlayer()),
        outcomes_(),
        returns_(os_state_->IsTerminal() ? os_state_->Returns()
                                         : std::vector<double>()) {
    SPIEL_CHECK_TRUE(os_state_->IsTerminal() ||
                     os_state_->CurrentPlayer() >= 0);
    SPIEL_CHECK_EQ(os_state_->IsTerminal(), IsTerminal());
  }
  ~HistoryCache() = default;

 public:
  bool IsTerminal() const { return !returns_.empty(); }

 public:
  const std::shared_ptr<const open_spiel::State> os_state_;
  const size_t parent_idx_;
  const std::string info_state_key_;
  const open_spiel::Player player_to_act_;
  std::vector<Outcomes> outcomes_;
  const std::vector<double> returns_;
};
}  // namespace _decision_point

// A history-based DecisionPoint interface.
class DecisionPoint {
 public:
  DecisionPoint(size_t num_players, size_t num_distinct_actions)
      : num_players_(num_players),
        num_distinct_actions_(num_distinct_actions) {}
  virtual ~DecisionPoint() = default;

  // Properties
  size_t NumDistinctActions() const { return num_distinct_actions_; }
  size_t NumPlayers() const { return num_players_; }

  virtual std::shared_ptr<const open_spiel::State> OpenSpielState() const = 0;
  virtual const open_spiel::State* OpenSpielStatePtr() const = 0;
  virtual const std::string& InformationStateStringRef() const = 0;
  std::string InformationStateString() const {
    return InformationStateStringRef();
  }
  virtual size_t NumActions() const = 0;
  virtual bool IsRoot() const = 0;
  virtual open_spiel::Player PlayerToAct() const = 0;
  virtual bool IsTerminal() const = 0;
  virtual const std::vector<double>& ReturnsRef() const = 0;
  std::vector<double> Returns() const { return ReturnsRef(); }
  virtual bool TerminalsAreSaved() const = 0;

  // Action-conditional properties
  virtual const std::vector<double>& OutcomeProbabilitiesRef(
      size_t action) const = 0;
  std::vector<double> OutcomeProbabilities(size_t action) const {
    return OutcomeProbabilitiesRef(action);
  }
  virtual size_t NumOutcomes(size_t action) const = 0;

  // Transformation methods
  virtual void Undo() = 0;
  virtual void UndoAll() = 0;
  virtual void Apply(size_t action, size_t outcome) = 0;
  double ApplySampledOutcome(size_t action, double random_number);

 private:
  size_t num_players_;
  size_t num_distinct_actions_;
};

class CachedDecisionPoint : public DecisionPoint {
 public:
  CachedDecisionPoint(open_spiel::StatePtr&& root, bool save_root = false,
                      bool save_terminals = false);
  CachedDecisionPoint(const open_spiel::State& root, bool save_root = false,
                      bool save_terminals = false)
      : CachedDecisionPoint(root.Clone(), save_root, save_terminals) {}

  const std::string& InformationStateStringRef() const override final {
    return histories_[idx_].info_state_key_;
  }
  std::shared_ptr<const open_spiel::State> OpenSpielState()
      const override final {
    return histories_[idx_].os_state_;
  }
  const open_spiel::State* OpenSpielStatePtr() const override final {
    return histories_[idx_].os_state_.get();
  }
  size_t NumActions() const override final {
    return histories_[idx_].outcomes_.size();
  }
  bool IsRoot() const override final { return idx_ == 0; }
  open_spiel::Player PlayerToAct() const override final {
    return histories_[idx_].player_to_act_;
  }
  bool IsTerminal() const override final {
    return histories_[idx_].IsTerminal();
  }
  const std::vector<double>& OutcomeProbabilitiesRef(
      size_t action) const override final {
    return histories_[idx_].outcomes_[action].Probs();
  }
  size_t NumOutcomes(size_t action) const override final {
    return histories_[idx_].outcomes_[action].Size();
  }
  const std::vector<double>& ReturnsRef() const override final {
    return histories_[idx_].returns_;
  }
  bool TerminalsAreSaved() const override final {
    return save_terminals_;
  }
  void Undo() override final { idx_ = histories_[idx_].parent_idx_; }
  void UndoAll() override final { idx_ = 0; }
  void Apply(size_t action, size_t outcome) override final {
    idx_ = histories_[idx_].outcomes_[action].Idx(outcome);
    CacheOutcomes();
  }

 protected:
  void RecursiveCache(const open_spiel::State& child, double prob, size_t aidx);
  void RecursiveCache(open_spiel::StatePtr&& child, double prob, size_t aidx);
  void CacheOutcomes();

 private:
  size_t idx_;
  std::vector<_decision_point::HistoryCache> histories_;
  bool save_terminals_;
};

void _ForEachState(std::unordered_set<std::string>& already_observed,
                   DecisionPoint& decision_point,
                   const std::function<void(const DecisionPoint&)>& f,
                   int player);
void ForEachState(DecisionPoint& root,
                  const std::function<void(const DecisionPoint&)>& f,
                  int player);
int NumStates(DecisionPoint& root, int player = ALL_PLAYERS);
ActionMap<int> NumStatesWithAction(DecisionPoint& root, int player);

}  // namespace hr_edl

#endif  // HR_EDL_DECISION_POINT_H_
