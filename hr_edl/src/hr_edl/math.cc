#include <cmath>

#include "hr_edl/math.h"

namespace hr_edl {

double CounterfactualReachProb(const std::vector<double>& reach_probabilities,
                               int player) {
  double cf_reach_prob = 1.0;
  for (int i = 0; i < reach_probabilities.size(); ++i) {
    if (i != player) {
      cf_reach_prob *= reach_probabilities[i];
    }
  }
  return cf_reach_prob;
}

std::vector<double> CounterfactualReturns(
    std::vector<double> returns, double chance_reach_importance_weight,
    const std::vector<double>& reach_probabilities,
    double player_sampling_prob) {
  for (size_t player = 0; player < returns.size(); ++player) {
    returns[player] *= chance_reach_importance_weight *
                       CounterfactualReachProb(reach_probabilities, player) /
                       player_sampling_prob;
  }
  return returns;
}

double MaxPositiveValue(const std::vector<double>& v) {
  double max_positive_value = 0.0;
  for (const auto next_value : v) {
    if (next_value > max_positive_value) {
      max_positive_value = next_value;
    }
  }
  return max_positive_value;
}

double MaxAbs(const std::vector<double>& v) {
  double max_value = std::abs(v[0]);
  for (int i = 1; i < v.size(); ++i) {
    const double next_value = std::abs(v[i]);
    if (next_value > max_value) {
      max_value = next_value;
    }
  }
  return max_value;
}

double ComputeAdaNormalHedgeWeights(std::vector<double>& weights,
                                    const std::vector<double>& regrets,
                                    const std::vector<double>& c,
                                    double max_utility) {
  if (!(max_utility > 0)) {
    return 0;
  }
  double regrets_p1_squared[regrets.size()];
  double regrets_m1_squared[regrets.size()];

  double denominator = 3 * (1 + std::abs(c[0]) / max_utility);
  double r = regrets[0] / max_utility;
  double rp1_squared = Square(Relu(r + 1)) / denominator;
  double max_rp1_squared = rp1_squared;

  regrets_p1_squared[0] = rp1_squared;
  regrets_m1_squared[0] = Square(Relu(r - 1)) / denominator;
  for (int i = 1; i < regrets.size(); ++i) {
    denominator = 3 * (1 + std::abs(c[i]) / max_utility);
    r = regrets[i] / max_utility;
    rp1_squared = Square(Relu(r + 1)) / denominator;
    regrets_p1_squared[i] = rp1_squared;
    if (rp1_squared > max_rp1_squared) {
      max_rp1_squared = rp1_squared;
    }
    regrets_m1_squared[i] = Square(Relu(r - 1)) / denominator;
  }

  double normalizer = 0.0;
  for (int i = 0; i < regrets.size(); ++i) {
    const double y = std::exp(regrets_p1_squared[i] - max_rp1_squared) -
                     std::exp(regrets_m1_squared[i] - max_rp1_squared);
    weights[i] = y;
    normalizer += y;
  }
  return normalizer;
}

double InnerProduct(const std::vector<double>& u,
                    const std::vector<double>& v) {
  double s = 0.0;
  for (int i = 0; i < u.size(); ++i) {
    s += u[i] * v[i];
  }
  return s;
}

void Add(std::vector<double>& sum, const std::vector<double>& v) {
  for (size_t i = 0; i < v.size(); ++i) {
    sum[i] += v[i];
  }
}

void SafeDivide(std::vector<double>& v, double s, bool set_uniform) {
  if (s > 0) {
    for (double& v_i : v) {
      v_i = v_i / s;
    }
  } else if (set_uniform) {
    for (double& v_i : v) {
      v_i = 1.0 / v.size();
    }
  }
}

}  // namespace hr_edl
