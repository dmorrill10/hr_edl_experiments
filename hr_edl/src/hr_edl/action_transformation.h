#ifndef HR_EDL_ACTION_TRANSFORMATION_H_
#define HR_EDL_ACTION_TRANSFORMATION_H_

#include <cassert>

#include "eigen/Eigen/Dense"
#include "hr_edl/math.h"
#include "hr_edl/types.h"

namespace hr_edl {

inline std::vector<size_t> SquareIdentity(size_t d) {
  std::vector<size_t> eye;
  eye.reserve(d);
  for (size_t i = 0; i < d; ++i) {
    eye.push_back(i);
  }
  return eye;
}

inline bool RepresentsExternalTransformation(
    const std::vector<size_t>& phi_block) {
  const size_t ex_a = phi_block[0];
  for (size_t a = 1; a < phi_block.size(); ++a) {
    if (phi_block[a] != ex_a) {
      return false;
    }
  }
  return true;
}

class SwapActionTranformation {
 public:
  static std::vector<SwapActionTranformation> External(size_t num_actions) {
    std::vector<SwapActionTranformation> list;
    list.reserve(num_actions);
    for (size_t a1 = 0; a1 < num_actions; ++a1) {
      list.emplace_back(std::vector<size_t>(num_actions, a1));
    }
    return list;
  }
  static std::vector<SwapActionTranformation> Internal(size_t num_actions) {
    std::vector<SwapActionTranformation> list;
    list.reserve(num_actions * num_actions - num_actions);
    std::vector<size_t> phi_block = SquareIdentity(num_actions);
    for (size_t a1 = 0; a1 < num_actions; ++a1) {
      for (size_t a2 = 0; a2 < num_actions; ++a2) {
        if (a1 == a2) {
          continue;
        }
        phi_block[a1] = a2;
        list.emplace_back(phi_block);
        phi_block[a1] = a1;
      }
    }
    return list;
  }

 public:
  SwapActionTranformation(std::vector<size_t>&& phi_block)
      : phi_block_(std::move(phi_block)),
        is_external_(RepresentsExternalTransformation(phi_block_)) {}
  SwapActionTranformation(const std::vector<size_t>& phi_block)
      : phi_block_(phi_block),
        is_external_(RepresentsExternalTransformation(phi_block_)) {}
  virtual ~SwapActionTranformation() = default;

  std::vector<double> operator()(size_t action) const {
    std::vector<double> phi_policy(NumActions(), 0);
    phi_policy[SwapTarget(action)] = 1.0;
    return phi_policy;
  }
  std::vector<double> operator()(const std::vector<double>& policy) const {
    std::vector<double> phi_policy(policy.size(), 0);
    for (size_t action = 0; action < NumActions(); ++action) {
      phi_policy[SwapTarget(action)] += policy[action];
    }
    return phi_policy;
  }
  size_t SwapTarget(size_t action) const { return phi_block_[action]; }
  size_t NumActions() const { return phi_block_.size(); }

  double Regret(const CfValues& cfvs, const std::vector<double>& policy) const {
    double r = -cfvs();
    assert(policy.size() == cfvs.Size());
    assert(policy.size() == NumActions());
    for (size_t action = 0; action < policy.size(); ++action) {
      r += cfvs[phi_block_[action]] * policy[action];
    }
    return r;
  }

 private:
  const std::vector<size_t> phi_block_;

 public:
  const bool is_external_;
};
using Sat = SwapActionTranformation;

class WeightedActionTransformation {
 private:
  inline static Eigen::MatrixXd ProjectiveEye(size_t num_actions) {
    Eigen::MatrixXd a =
        -Eigen::MatrixXd::Identity(num_actions + 1, num_actions);
    a.row(num_actions) = Eigen::VectorXd::Constant(num_actions, 1.0);
    return a;
  }
  inline static Eigen::VectorXd BVector(size_t num_actions) {
    Eigen::VectorXd b = Eigen::VectorXd::Zero(num_actions + 1);
    b(num_actions) = 1.0;
    return b;
  }

 public:
  WeightedActionTransformation(size_t num_actions)
      : weight_sum_(0),
        weighted_phi_blocks_(Eigen::MatrixXd::Zero(num_actions, num_actions)),
        weighted_external_blocks_(Eigen::VectorXd::Zero(num_actions)),
        all_external_(true),
        projective_eye_(ProjectiveEye(num_actions)),
        b_(BVector(num_actions)) {}
  virtual ~WeightedActionTransformation() = default;

  void Add(const Sat& phi, double weight = 1.0) {
    weight_sum_ += weight;
    if (phi.is_external_) {
      weighted_external_blocks_(phi.SwapTarget(0)) += weight;
    } else {
      all_external_ = false;
      for (size_t action = 0; action < NumActions(); ++action) {
        weighted_phi_blocks_(phi.SwapTarget(action), action) += weight;
      }
    }
  }

  Eigen::MatrixXd ToMatrix() const {
    return weighted_phi_blocks_.colwise() + weighted_external_blocks_;
  }

  std::vector<double> FixedPoint() const {
    std::vector<double> pi_mem(NumActions(), 1.0 / NumActions());
    FixedPoint(pi_mem);
    return pi_mem;
  }

  void FixedPoint(std::vector<double>& pi_mem) const {
    if (weight_sum_ > 0) {
      Eigen::Map<Eigen::VectorXd> pi(pi_mem.data(), NumActions());
      if (all_external_) {
        pi = weighted_external_blocks_ / weight_sum_;
      } else {
        Eigen::MatrixXd A = projective_eye_;
        A.topRows(NumActions()) += ToMatrix() / weight_sum_;
        pi = (A.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b_))
                 .cwiseMax(0)
                 .cwiseMin(1.0);
      }
    } else {
      pi_mem.assign(NumActions(), 1.0 / NumActions());
    }
  }

  double WeightSum() const { return weight_sum_; }

  void Reset() {
    weight_sum_ = 0;
    weighted_phi_blocks_.setZero();
    weighted_external_blocks_.setZero();
  }

  size_t NumActions() const { return weighted_phi_blocks_.rows(); }

 private:
  double weight_sum_;
  Eigen::MatrixXd weighted_phi_blocks_;
  Eigen::VectorXd weighted_external_blocks_;
  bool all_external_;
  const Eigen::MatrixXd projective_eye_;
  const Eigen::VectorXd b_;
};

}  // namespace hr_edl
#endif  // HR_EDL_ACTION_TRANSFORMATION_H_
