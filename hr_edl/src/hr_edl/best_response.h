#ifndef HR_EDL_BEST_RESPONSE_H_
#define HR_EDL_BEST_RESPONSE_H_

#include "hr_edl/decision_point.h"
#include "hr_edl/policy.h"

namespace hr_edl {

class BestResponse {
 public:
  BestResponse(const Policy& policy)
      : policy_(policy), iss_by_depth_(), info_set_values_() {}

  double Value(DecisionPoint& decision_point, int player);
  double Value(const open_spiel::State& history, int player);
  std::pair<MapPolicy, double> Policy(DecisionPoint& decision_point,
                                      int player);
  std::pair<MapPolicy, double> Policy(const open_spiel::State& history,
                                      int player);

 private:
  using InfoSetValues = std::pair<double*, std::vector<double>>;

  // A policy_player of -i means that all positions except i - 1 should use
  // the policy while i - 1 uses a BR.
  static bool UsePolicy(open_spiel::Player acting_player, int policy_player) {
    return PlayerInSet(acting_player, policy_player);
  }
  static double CfReturn(const std::vector<double>& utils, double prob,
                         int policy_player);

 private:
  double* NewParentValue(const std::string& iss, double* parent_value,
                         size_t num_actions, size_t depth);
  void BackwardPassValue() const;
  InfoStateUvm<std::vector<double>> BackwardPassMap() const;
  MapPolicy BackwardPassPolicy() const { return BackwardPassMap(); }
  void RecursiveDfs(DecisionPoint& decision_point, size_t depth, double prob,
                    double* parent_value, int player, size_t action);
  void Dfs(DecisionPoint& decision_point, double* br_value, int player);
  void RecursiveDfs(DecisionPoint& decision_point, size_t depth, double prob,
                    double* parent_value, int player);
  void Dfs(const open_spiel::State& history, double* br_value, int player);
  void RecursiveDfs(const open_spiel::State& h, size_t depth, double prob,
                    double* parent_value, int player);

 private:
  const class Policy& policy_;
  std::vector<std::vector<std::string>> iss_by_depth_;
  InfoStateUvm<InfoSetValues> info_set_values_;
};

// TODO: This is not sufficiently general
inline double NashConv(DecisionPoint& decision_point,
                       const Policy& policy_profile) {
  double nc = 0.0;
  for (open_spiel::Player player = 0; player < decision_point.NumPlayers();
       ++player) {
    nc += BestResponse(policy_profile).Value(decision_point, player);
  }
  return nc;
}

inline double Exploitability(DecisionPoint& decision_point,
                             const Policy& policy_profile) {
  return NashConv(decision_point, policy_profile) / decision_point.NumPlayers();
}

}  // namespace hr_edl

#endif  // HR_EDL_BEST_RESPONSE_H_
