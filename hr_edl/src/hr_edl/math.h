#ifndef HR_EDL_MATH_H_
#define HR_EDL_MATH_H_

#include <vector>

namespace hr_edl {

void SafeDivide(std::vector<double>& v, double s, bool set_uniform = false);
double CounterfactualReachProb(const std::vector<double>& reach_probabilities,
                               int player);
std::vector<double> CounterfactualReturns(
    std::vector<double> returns, double chance_reach_importance_weight,
    const std::vector<double>& reach_probabilities,
    double player_sampling_prob);

double MaxPositiveValue(const std::vector<double>& v);
double MaxAbs(const std::vector<double>& v);
double ComputeAdaNormalHedgeWeights(std::vector<double>& weights,
                                    const std::vector<double>& regrets,
                                    const std::vector<double>& c,
                                    double max_utility);
inline double Relu(double x) { return std::max(0.0, x); }
double InnerProduct(const std::vector<double>& u, const std::vector<double>& v);
void Add(std::vector<double>& sum, const std::vector<double>& v);
inline double Square(double x) { return x * x; }
}  // namespace hr_edl

#endif  // HR_EDL_MATH_H_
