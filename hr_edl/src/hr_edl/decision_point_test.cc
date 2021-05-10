#include "hr_edl/decision_point.h"

#include "open_spiel/spiel.h"
#include "open_spiel/spiel_utils.h"
#include "hr_edl/test_extra.h"

namespace hr_edl {
namespace {
void InitCachedDecisionPoint() {
  CachedDecisionPoint decision_point(
      open_spiel::LoadGame("kuhn_poker")->NewInitialState());
  SPIEL_CHECK_TRUE(decision_point.IsRoot());
  SPIEL_CHECK_FALSE(decision_point.IsTerminal());
  SPIEL_CHECK_EQ(decision_point.NumActions(), 1);
  SPIEL_CHECK_EQ(decision_point.NumOutcomes(0), 6);
  SPIEL_CHECK_EQ(decision_point.PlayerToAct(), -1);
}

void CachedDecisionPointApplyAndUndo() {
  std::shared_ptr<const open_spiel::Game> game =
      open_spiel::LoadGame("kuhn_poker");

  CachedDecisionPoint decision_point(game->NewInitialState());
  decision_point.Apply(0, 0);
  SPIEL_CHECK_FALSE(decision_point.IsRoot());
  SPIEL_CHECK_FALSE(decision_point.IsTerminal());
  SPIEL_CHECK_EQ(decision_point.PlayerToAct(), 0);

  decision_point.Undo();
  SPIEL_CHECK_TRUE(decision_point.IsRoot());
  SPIEL_CHECK_FALSE(decision_point.IsTerminal());
  SPIEL_CHECK_EQ(decision_point.NumActions(), 1);
  SPIEL_CHECK_EQ(decision_point.NumOutcomes(0), 6);
  SPIEL_CHECK_EQ(decision_point.PlayerToAct(), -1);

  decision_point.Apply(0, 1);
  SPIEL_CHECK_FALSE(decision_point.IsRoot());
  SPIEL_CHECK_FALSE(decision_point.IsTerminal());
  SPIEL_CHECK_EQ(decision_point.PlayerToAct(), 0);

  decision_point.Undo();
  SPIEL_CHECK_TRUE(decision_point.IsRoot());
  SPIEL_CHECK_FALSE(decision_point.IsTerminal());
  SPIEL_CHECK_EQ(decision_point.NumActions(), 1);
  SPIEL_CHECK_EQ(decision_point.NumOutcomes(0), 6);
  SPIEL_CHECK_EQ(decision_point.PlayerToAct(), -1);

  decision_point.ApplySampledOutcome(0, 0.16);
  SPIEL_CHECK_FALSE(decision_point.IsRoot());
  SPIEL_CHECK_FALSE(decision_point.IsTerminal());
  SPIEL_CHECK_EQ(decision_point.PlayerToAct(), 0);

  decision_point.Undo();
  decision_point.ApplySampledOutcome(0, 0.32);
  SPIEL_CHECK_FALSE(decision_point.IsRoot());
  SPIEL_CHECK_FALSE(decision_point.IsTerminal());
  SPIEL_CHECK_EQ(decision_point.PlayerToAct(), 0);

  decision_point.Undo();
  decision_point.ApplySampledOutcome(0, 0.48);
  SPIEL_CHECK_FALSE(decision_point.IsRoot());
  SPIEL_CHECK_FALSE(decision_point.IsTerminal());
  SPIEL_CHECK_EQ(decision_point.PlayerToAct(), 0);
}

void CachedDecisionPointLiarsDiceTraversal() {
  CachedDecisionPoint decision_point(
      open_spiel::LoadGame("liars_dice")->NewInitialState());

  std::function<void()> f;
  f = [&decision_point, &f]() {
    if (decision_point.IsTerminal()) {
      return;
    } else {
      for (size_t a = 0; a < decision_point.NumActions(); ++a) {
        for (size_t outcome = 0; outcome < decision_point.NumOutcomes(a);
             ++outcome) {
          decision_point.Apply(a, outcome);
          f();
          decision_point.Undo();
        }
      }
    }
  };
  f();
  SPIEL_CHECK_TRUE(decision_point.IsRoot());
  SPIEL_CHECK_FALSE(decision_point.IsTerminal());
}

void CachedDecisionPointWithTerminals() {
  CachedDecisionPoint decision_point(
      open_spiel::LoadGame("liars_dice")->NewInitialState(), false, true);

  std::function<void()> f;
  f = [&decision_point, &f]() {
    if (decision_point.IsTerminal()) {
      return;
    } else {
      decision_point.Apply(0, 0);
      f();
    }
  };
  f();
  SPIEL_CHECK_TRUE(decision_point.IsTerminal());
  SPIEL_CHECK_TRUE(decision_point.OpenSpielStatePtr());
  SPIEL_CHECK_EQ(decision_point.OpenSpielStatePtr()->HistoryString(),
                 std::string("0 0 0 1 2 3 4 5 6 7 8 9 10 11 12"));
}

}  // namespace
}  // namespace hr_edl

using namespace hr_edl;

int main(int argc, char** argv) {
  RUN_TEST(InitCachedDecisionPoint);
  RUN_TEST(CachedDecisionPointApplyAndUndo);
  RUN_TEST(CachedDecisionPointLiarsDiceTraversal);
  RUN_TEST(CachedDecisionPointWithTerminals);
}
