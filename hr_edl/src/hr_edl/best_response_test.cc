#include "hr_edl/best_response.h"

#include "open_spiel/algorithms/tabular_exploitability.h"
#include "open_spiel/spiel.h"
#include "open_spiel/spiel_utils.h"
#include "hr_edl/decision_point.h"
#include "hr_edl/samplers.h"
#include "hr_edl/test_extra.h"
#include "hr_edl/stopwatch.h"

namespace hr_edl {
namespace test {
namespace {

void Kuhn() {
  std::shared_ptr<const open_spiel::Game> game =
      open_spiel::LoadGame("kuhn_poker");

  CachedDecisionPoint decision_point(game->NewInitialState());

  {
    const AlwaysZeroPolicy policy;
    const double always_fold_exploitability =
        open_spiel::algorithms::Exploitability(*game, policy);
    SPIEL_CHECK_FLOAT_NEAR(always_fold_exploitability, 1.0, 0);
    SPIEL_CHECK_FLOAT_NEAR(always_fold_exploitability,
                           Exploitability(decision_point, policy), 0);

    {
      const auto [br_policy0, br_v0] =
          BestResponse(policy).Policy(decision_point, 0);
      const auto [br_policy1, br_v1] =
          BestResponse(policy).Policy(decision_point, 1);
      SPIEL_CHECK_FLOAT_NEAR(always_fold_exploitability, (br_v0 + br_v1) / 2.0,
                             0);
    }
    {
      const auto [br_policy0, br_v0] =
          BestResponse(policy).Policy(decision_point, -2);
      const auto [br_policy1, br_v1] =
          BestResponse(policy).Policy(decision_point, -1);
      SPIEL_CHECK_FLOAT_NEAR(always_fold_exploitability, (br_v0 + br_v1) / 2.0,
                             0);
    }
  }

  {
    const AlwaysMaxActionPolicy policy;
    const double always_raise_exploitability =
        open_spiel::algorithms::Exploitability(*game, policy);
    SPIEL_CHECK_FLOAT_NEAR(always_raise_exploitability, 1 / 3.0, 0);
    SPIEL_CHECK_FLOAT_NEAR(always_raise_exploitability,
                           Exploitability(decision_point, policy), 0);

    {
      const auto [br_policy0, br_v0] =
          BestResponse(policy).Policy(decision_point, 0);
      const auto [br_policy1, br_v1] =
          BestResponse(policy).Policy(decision_point, 1);
      SPIEL_CHECK_FLOAT_NEAR(always_raise_exploitability, (br_v0 + br_v1) / 2.0,
                             0);
    }
    {
      const auto [br_policy0, br_v0] =
          BestResponse(policy).Policy(decision_point, -2);
      const auto [br_policy1, br_v1] =
          BestResponse(policy).Policy(decision_point, -1);
      SPIEL_CHECK_FLOAT_NEAR(always_raise_exploitability, (br_v0 + br_v1) / 2.0,
                             0);
    }
  }

  {
    const MapPolicy policy = UniformRandomPolicy();
    const double uniform_random_exploitability =
        open_spiel::algorithms::Exploitability(*game, policy);
    SPIEL_CHECK_FLOAT_NEAR(uniform_random_exploitability, 0.458333, 1e-5);
    SPIEL_CHECK_FLOAT_NEAR(uniform_random_exploitability,
                           Exploitability(decision_point, policy), 1e-15);
    {
      const auto [br_policy0, br_v0] =
          BestResponse(policy).Policy(decision_point, 0);
      const auto [br_policy1, br_v1] =
          BestResponse(policy).Policy(decision_point, 1);
      SPIEL_CHECK_FLOAT_NEAR(uniform_random_exploitability,
                             (br_v0 + br_v1) / 2.0, 1e-15);
    }
    {
      const auto [br_policy0, br_v0] =
          BestResponse(policy).Policy(decision_point, -2);
      const auto [br_policy1, br_v1] =
          BestResponse(policy).Policy(decision_point, -1);
      SPIEL_CHECK_FLOAT_NEAR(uniform_random_exploitability,
                             (br_v0 + br_v1) / 2.0, 1e-15);
    }
  }
}

void Leduc() {
  std::shared_ptr<const open_spiel::Game> game =
      open_spiel::LoadGame("leduc_poker");

  CachedDecisionPoint decision_point(game->NewInitialState());

  {
    const AlwaysZeroPolicy policy;
    const double always_fold_exploitability =
        open_spiel::algorithms::Exploitability(*game, policy);
    SPIEL_CHECK_FLOAT_NEAR(always_fold_exploitability, 1.0, 0);
    SPIEL_CHECK_FLOAT_NEAR(always_fold_exploitability,
                           Exploitability(decision_point, policy), 0);

    {
      const auto [br_policy0, br_v0] =
          BestResponse(policy).Policy(decision_point, 0);
      const auto [br_policy1, br_v1] =
          BestResponse(policy).Policy(decision_point, 1);
      SPIEL_CHECK_FLOAT_NEAR(always_fold_exploitability, (br_v0 + br_v1) / 2.0,
                             0);
    }
    {
      const auto [br_policy0, br_v0] =
          BestResponse(policy).Policy(decision_point, -2);
      const auto [br_policy1, br_v1] =
          BestResponse(policy).Policy(decision_point, -1);
      SPIEL_CHECK_FLOAT_NEAR(always_fold_exploitability, (br_v0 + br_v1) / 2.0,
                             0);
    }
  }

  {
    const AlwaysMaxActionPolicy policy;
    const double always_raise_exploitability =
        open_spiel::algorithms::Exploitability(*game, policy);
    SPIEL_CHECK_FLOAT_NEAR(always_raise_exploitability, 2.36667, 1e-5);
    SPIEL_CHECK_FLOAT_NEAR(always_raise_exploitability,
                           Exploitability(decision_point, policy), 0);

    {
      const auto [br_policy0, br_v0] =
          BestResponse(policy).Policy(decision_point, 0);
      const auto [br_policy1, br_v1] =
          BestResponse(policy).Policy(decision_point, 1);
      SPIEL_CHECK_FLOAT_NEAR(always_raise_exploitability, (br_v0 + br_v1) / 2.0,
                             0);
    }
    {
      const auto [br_policy0, br_v0] =
          BestResponse(policy).Policy(decision_point, -2);
      const auto [br_policy1, br_v1] =
          BestResponse(policy).Policy(decision_point, -1);
      SPIEL_CHECK_FLOAT_NEAR(always_raise_exploitability, (br_v0 + br_v1) / 2.0,
                             0);
    }
  }

  {
    const MapPolicy policy = UniformRandomPolicy();
    const double uniform_random_exploitability =
        open_spiel::algorithms::Exploitability(*game, policy);
    SPIEL_CHECK_FLOAT_NEAR(uniform_random_exploitability, 2.37361, 1e-5);
    SPIEL_CHECK_FLOAT_NEAR(uniform_random_exploitability,
                           Exploitability(decision_point, policy), 1e-15);
    {
      const auto [br_policy0, br_v0] =
          BestResponse(policy).Policy(decision_point, 0);
      const auto [br_policy1, br_v1] =
          BestResponse(policy).Policy(decision_point, 1);
      SPIEL_CHECK_FLOAT_NEAR(uniform_random_exploitability,
                             (br_v0 + br_v1) / 2.0, 1e-15);
    }
    {
      const auto [br_policy0, br_v0] =
          BestResponse(policy).Policy(decision_point, -2);
      const auto [br_policy1, br_v1] =
          BestResponse(policy).Policy(decision_point, -1);
      SPIEL_CHECK_FLOAT_NEAR(uniform_random_exploitability,
                             (br_v0 + br_v1) / 2.0, 1e-15);
    }
  }
}

void UniformRandomBrValueInLiarsDice() {
  std::shared_ptr<const open_spiel::Game> game =
      open_spiel::LoadGame("liars_dice");

  const MapPolicy policy = UniformRandomPolicy();

  Stopwatch watch;
  CachedDecisionPoint decision_point(game->NewInitialState());
  BestResponse br(policy);
  const double my_exploitability =
      (br.Value(decision_point, 0) + br.Value(decision_point, 1)) / 2.0;
  double ms = watch.milliseconds();
  std::cout << "My exploitability in liar's dice took " << ms << " ms"
            << std::endl;

  watch.reset();
  const double uniform_random_exploitability =
      open_spiel::algorithms::Exploitability(*game, policy);
  ms = watch.milliseconds();
  std::cout << "OpenSpiel exploitability in liar's dice took " << ms << " ms"
            << std::endl;
  SPIEL_CHECK_FLOAT_NEAR(uniform_random_exploitability, 0.780744, 1e-5);

  SPIEL_CHECK_FLOAT_NEAR(uniform_random_exploitability, my_exploitability,
                         1e-15);

  watch.reset();
  const auto [br_policy0, br_v0] = br.Policy(decision_point, 0);
  const auto [br_policy1, br_v1] = br.Policy(decision_point, 1);
  ms = watch.milliseconds();
  std::cout << "Warm BR in liar's dice took " << ms << " ms" << std::endl;
  SPIEL_CHECK_FLOAT_NEAR(uniform_random_exploitability, (br_v0 + br_v1) / 2.0,
                         1e-15);
}

}  // namespace
}  // namespace test
}  // namespace hr_edl

using namespace hr_edl::test;

int main(int argc, char** argv) {
  RUN_TEST(Kuhn);
  RUN_TEST(Leduc);
  RUN_TEST(UniformRandomBrValueInLiarsDice);
}
