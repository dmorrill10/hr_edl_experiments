#ifndef HR_EDL_STOPWATCH_H_
#define HR_EDL_STOPWATCH_H_

#include <atomic>
#include <chrono>

namespace hr_edl {

// Thanks to Incomputable
// https://codereview.stackexchange.com/questions/196245/extremely-simple-timer-class-in-c
// for this.
template <typename Clock = std::chrono::steady_clock>
class Stopwatch {
 public:
  Stopwatch() : start_time_(Clock::now()) {}

  void reset() { start_time_ = Clock::now(); }

  template <typename Rep = typename Clock::duration::rep,
            typename Units = typename Clock::duration>
  Rep duration() const {
    std::atomic_thread_fence(std::memory_order_relaxed);
    const auto duration =
        std::chrono::duration_cast<Units>(Clock::now() - start_time_).count();
    std::atomic_thread_fence(std::memory_order_relaxed);
    return static_cast<Rep>(duration);
  }

  double seconds() const { return duration<double, std::chrono::seconds>(); }
  double milliseconds() const {
    return duration<double, std::chrono::milliseconds>();
  }

 private:
  typename Clock::time_point start_time_;
};
}  // namespace hr_edl

#endif  // HR_EDL_STOPWATCH_H_
