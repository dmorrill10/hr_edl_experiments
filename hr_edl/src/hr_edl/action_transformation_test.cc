#include "hr_edl/action_transformation.h"

#include "open_spiel/spiel_utils.h"
#include "hr_edl/test_extra.h"

namespace hr_edl {
namespace phi_regret_matching {
namespace test {
namespace {

void SimpleInternalActionTranformation() {
  // Internal transformation from action 1 to action 2.
  SwapActionTranformation phi({0, 2, 2});

  SPIEL_CHECK_EQ(phi(0).size(), 3);
  SPIEL_CHECK_EQ(phi(1).size(), 3);
  SPIEL_CHECK_EQ(phi(2).size(), 3);

  SPIEL_CHECK_FLOAT_NEAR(phi(0)[0], 1.0, 0);
  SPIEL_CHECK_FLOAT_NEAR(phi(0)[1], 0.0, 0);
  SPIEL_CHECK_FLOAT_NEAR(phi(0)[2], 0.0, 0);

  SPIEL_CHECK_FLOAT_NEAR(phi(1)[0], 0.0, 0);
  SPIEL_CHECK_FLOAT_NEAR(phi(1)[1], 0.0, 0);
  SPIEL_CHECK_FLOAT_NEAR(phi(1)[2], 1.0, 0);

  SPIEL_CHECK_FLOAT_NEAR(phi(2)[0], 0.0, 0);
  SPIEL_CHECK_FLOAT_NEAR(phi(2)[1], 0.0, 0);
  SPIEL_CHECK_FLOAT_NEAR(phi(2)[2], 1.0, 0);

  SPIEL_CHECK_FLOAT_NEAR(phi({1.0, 0, 0})[0], 1.0, 0);
  SPIEL_CHECK_FLOAT_NEAR(phi({1.0, 0, 0})[1], 0.0, 0);
  SPIEL_CHECK_FLOAT_NEAR(phi({1.0, 0, 0})[2], 0.0, 0);

  SPIEL_CHECK_FLOAT_NEAR(phi({0.5, 0.5, 0})[0], 0.5, 0);
  SPIEL_CHECK_FLOAT_NEAR(phi({0.5, 0.5, 0})[1], 0.0, 0);
  SPIEL_CHECK_FLOAT_NEAR(phi({0.5, 0.5, 0})[2], 0.5, 0);

  SPIEL_CHECK_FLOAT_NEAR(phi({0.2, 0.3, 0.5})[0], 0.2, 0);
  SPIEL_CHECK_FLOAT_NEAR(phi({0.2, 0.3, 0.5})[1], 0.0, 0);
  SPIEL_CHECK_FLOAT_NEAR(phi({0.2, 0.3, 0.5})[2], 0.8, 0);
}

void WeightedActionTransformationSum() {
  // Internal transformation from action 1 to action 2.
  SwapActionTranformation phi1({0, 2, 2});

  // Swap transformation from action 0 to action 1, 1 to 2, and 2 to 0.
  SwapActionTranformation phi2({1, 2, 0});

  WeightedActionTransformation sum(3);
  sum.Add(phi1, 2.0);
  sum.Add(phi2, 3.0);
  {
    const auto matrix_sum = sum.ToMatrix();
    SPIEL_CHECK_FLOAT_NEAR(sum.WeightSum(), 5.0, 0);
    SPIEL_CHECK_FLOAT_NEAR(matrix_sum.coeff(0, 0), 2.0, 0);
    SPIEL_CHECK_FLOAT_NEAR(matrix_sum.coeff(1, 0), 3.0, 0);
    SPIEL_CHECK_FLOAT_NEAR(matrix_sum.coeff(2, 0), 0.0, 0);
    SPIEL_CHECK_FLOAT_NEAR(matrix_sum.coeff(0, 1), 0.0, 0);
    SPIEL_CHECK_FLOAT_NEAR(matrix_sum.coeff(1, 1), 0.0, 0);
    SPIEL_CHECK_FLOAT_NEAR(matrix_sum.coeff(2, 1), 5.0, 0);
    SPIEL_CHECK_FLOAT_NEAR(matrix_sum.coeff(0, 2), 3.0, 0);
    SPIEL_CHECK_FLOAT_NEAR(matrix_sum.coeff(1, 2), 0.0, 0);
    SPIEL_CHECK_FLOAT_NEAR(matrix_sum.coeff(2, 2), 2.0, 0);
  }
  sum.Reset();
  {
    const auto matrix_sum = sum.ToMatrix();
    SPIEL_CHECK_FLOAT_NEAR(sum.WeightSum(), 0.0, 0);
    SPIEL_CHECK_FLOAT_NEAR(matrix_sum.coeff(0, 0), 0.0, 0);
    SPIEL_CHECK_FLOAT_NEAR(matrix_sum.coeff(1, 0), 0.0, 0);
    SPIEL_CHECK_FLOAT_NEAR(matrix_sum.coeff(2, 0), 0.0, 0);
    SPIEL_CHECK_FLOAT_NEAR(matrix_sum.coeff(0, 1), 0.0, 0);
    SPIEL_CHECK_FLOAT_NEAR(matrix_sum.coeff(1, 1), 0.0, 0);
    SPIEL_CHECK_FLOAT_NEAR(matrix_sum.coeff(2, 1), 0.0, 0);
    SPIEL_CHECK_FLOAT_NEAR(matrix_sum.coeff(0, 2), 0.0, 0);
    SPIEL_CHECK_FLOAT_NEAR(matrix_sum.coeff(1, 2), 0.0, 0);
    SPIEL_CHECK_FLOAT_NEAR(matrix_sum.coeff(2, 2), 0.0, 0);
  }
}

void WeightedActionTransformationFixedPoint() {
  // Internal transformation from action 1 to action 2.
  SwapActionTranformation phi1({0, 2, 2});

  // Swap transformation from action 0 to action 1, 1 to 2, and 2 to 0.
  SwapActionTranformation phi2({1, 2, 0});

  WeightedActionTransformation sum(3);
  sum.Add(phi1, 2.0);
  sum.Add(phi2, 3.0);
  const std::vector<double> prob_vec = sum.FixedPoint();

  SPIEL_CHECK_EQ(prob_vec.size(), 3);
  SPIEL_CHECK_FLOAT_NEAR(prob_vec[0], 0.384615, 1e-5);
  SPIEL_CHECK_FLOAT_NEAR(prob_vec[1], 0.230769, 1e-5);
  SPIEL_CHECK_FLOAT_NEAR(prob_vec[2], 0.384615, 1e-5);

  const Eigen::Map<const Eigen::VectorXd> pi(prob_vec.data(), prob_vec.size());

  SPIEL_CHECK_FLOAT_NEAR(pi.sum(), 1.0, 0);

  const Eigen::VectorXd pi2 = (sum.ToMatrix() / sum.WeightSum()) * pi;
  SPIEL_CHECK_EQ(pi.rows(), pi2.rows());
  for (size_t a = 0; a < pi.rows(); ++a) {
    SPIEL_CHECK_FLOAT_NEAR(pi2.coeff(a), pi.coeff(a), 0);
  }
}

void WeightedActionTransformationFixedPointInternalExternalAndSwap() {
  // Internal transformation from action 1 to action 2.
  SwapActionTranformation phi1({0, 2, 2});

  // Swap transformation from action 0 to action 1, 1 to 2, and 2 to 0.
  SwapActionTranformation phi2({1, 2, 0});

  // External transformation to action 0
  SwapActionTranformation phi3({0, 0, 0});

  WeightedActionTransformation sum(3);
  sum.Add(phi1, 2.0);
  sum.Add(phi2, 3.0);
  sum.Add(phi3, 4.0);
  const std::vector<double> prob_vec = sum.FixedPoint();

  SPIEL_CHECK_EQ(prob_vec.size(), 3);
  SPIEL_CHECK_FLOAT_NEAR(prob_vec[0], 0.636364, 1e-5);
  SPIEL_CHECK_FLOAT_NEAR(prob_vec[1], 0.212121, 1e-5);
  SPIEL_CHECK_FLOAT_NEAR(prob_vec[2], 0.151515, 1e-5);

  const Eigen::Map<const Eigen::VectorXd> pi(prob_vec.data(), prob_vec.size());

  SPIEL_CHECK_FLOAT_NEAR(pi.sum(), 1.0, 0);

  const Eigen::VectorXd pi2 = (sum.ToMatrix() / sum.WeightSum()) * pi;
  SPIEL_CHECK_EQ(pi.rows(), pi2.rows());
  for (size_t a = 0; a < pi.rows(); ++a) {
    SPIEL_CHECK_FLOAT_NEAR(pi2.coeff(a), pi.coeff(a), 0);
  }
}

void AllExternalTransformations() {
  const size_t num_actions = 3;
  const auto external_transformations =
      SwapActionTranformation::External(num_actions);
  SPIEL_CHECK_EQ(external_transformations.size(), num_actions);

  for (size_t a1 = 0; a1 < num_actions; ++a1) {
    const auto& phi = external_transformations[a1];
    for (size_t a2 = 0; a2 < num_actions; ++a2) {
      for (size_t a3 = 0; a3 < num_actions; ++a3) {
        SPIEL_CHECK_FLOAT_NEAR(phi(a2)[a3], double(a3 == a1), 0);
      }
    }
  }
  {
    WeightedActionTransformation sum(num_actions);
    for (size_t a1 = 0; a1 < num_actions; ++a1) {
      const auto& phi = external_transformations[a1];
      sum.Add(phi, a1 + 1.0);
    }
    const std::vector<double> prob_vec = sum.FixedPoint();
    SPIEL_CHECK_EQ(prob_vec.size(), 3);
    SPIEL_CHECK_FLOAT_NEAR(prob_vec[0], 1 / 6.0, 1e-5);
    SPIEL_CHECK_FLOAT_NEAR(prob_vec[1], 2 / 6.0, 1e-5);
    SPIEL_CHECK_FLOAT_NEAR(prob_vec[2], 0.5, 1e-5);
  }
  {
    WeightedActionTransformation sum(num_actions);
    for (size_t a1 = 0; a1 < num_actions; ++a1) {
      const auto& phi = external_transformations[a1];
      sum.Add(phi, 0);
    }
    const std::vector<double> prob_vec = sum.FixedPoint();
    SPIEL_CHECK_EQ(prob_vec.size(), 3);
    SPIEL_CHECK_FLOAT_NEAR(prob_vec[0], 1 / 3.0, 1e-5);
    SPIEL_CHECK_FLOAT_NEAR(prob_vec[1], 1 / 3.0, 1e-5);
    SPIEL_CHECK_FLOAT_NEAR(prob_vec[2], 1 / 3.0, 1e-5);
  }
}

void AllInternalTransformations() {
  const size_t num_actions = 3;
  const auto internal_transformations =
      SwapActionTranformation::Internal(num_actions);
  SPIEL_CHECK_EQ(internal_transformations.size(),
                 num_actions * num_actions - num_actions);

  size_t i = 0;
  for (size_t a1 = 0; a1 < num_actions; ++a1) {
    for (size_t a2 = 0; a2 < num_actions; ++a2) {
      if (a1 == a2) {
        continue;
      }
      const auto& phi = internal_transformations[i];
      ++i;
      for (size_t a3 = 0; a3 < num_actions; ++a3) {
        SPIEL_CHECK_FLOAT_NEAR(phi(a3)[a3], double(a3 != a1), 0);
      }
    }
  }
}
}  // namespace
}  // namespace test
}  // namespace phi_regret_matching
}  // namespace hr_edl

using namespace hr_edl::phi_regret_matching::test;

int main(int argc, char** argv) {
  RUN_TEST(SimpleInternalActionTranformation);
  RUN_TEST(WeightedActionTransformationSum);
  RUN_TEST(WeightedActionTransformationFixedPoint);
  RUN_TEST(WeightedActionTransformationFixedPointInternalExternalAndSwap);
  RUN_TEST(AllInternalTransformations);
  RUN_TEST(AllExternalTransformations);
}
