#ifndef HR_EDL_TEST_EXTRA_H_
#define HR_EDL_TEST_EXTRA_H_

#include "hr_edl/policy.h"

#define RUN_TEST(_TEST, ...)                                     \
  {                                                              \
    _TEST(__VA_ARGS__);                                          \
    std::cout << #_TEST "(" #__VA_ARGS__ "): Pass" << std::endl; \
  }

namespace hr_edl {
namespace test {
class AlwaysZeroPolicy : public Policy {
 public:
  std::vector<double> Response(
      const open_spiel::State& state) const override final {
    std::vector<double> response(state.LegalActions().size(), 0);
    response[0] = 1.0;
    return response;
  }
};

class AlwaysMaxActionPolicy : public Policy {
 public:
  std::vector<double> Response(
      const open_spiel::State& state) const override final {
    std::vector<double> response(state.LegalActions().size(), 0);
    response[response.size() - 1] = 1.0;
    return response;
  }
};
}  // namespace test
}  // namespace hr_edl

#endif  // HR_EDL_TEST_EXTRA_H_
