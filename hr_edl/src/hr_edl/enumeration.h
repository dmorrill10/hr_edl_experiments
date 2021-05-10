#ifndef HR_EDL_ENUMERATION_H_
#define HR_EDL_ENUMERATION_H_

#include <algorithm>
#include <vector>
#include <numeric>

namespace hr_edl {
template <class T>
using EnumerationConsumer = std::function<void(int, T)>;

template <class T>
class Enumeration {
 public:
  Enumeration(std::function<void(const EnumerationConsumer<T>&)>&& f,
              int size = -1)
      : f_(std::move(f)), size_(size) {}
  Enumeration(std::function<void(const EnumerationConsumer<T>&)>&& f,
              size_t size)
      : f_(std::move(f)), size_(size) {}
  ~Enumeration() = default;
  inline int Size() const { return size_; }
  inline bool SizeKnown() const { return size_ > -1; }
  inline void operator()(const EnumerationConsumer<T>& yield) const {
    return f_(yield);
  }

 private:
  std::function<void(const EnumerationConsumer<T>&)> f_;
  int size_;
};

template <class T>
Enumeration<T> Enumerate(const std::vector<T>& v) {
  return {[&v](const EnumerationConsumer<T>& yield) {
            for (int i = 0; i < v.size(); ++i) {
              yield(i, v[i]);
            }
          },
          v.size()};
}

template <class T>
std::vector<T> Range(T l, T u) {
  std::vector<T> range(u - l);
  std::iota(range.begin(), range.end(), l);
  return range;
}

inline std::vector<size_t> Range(size_t size) { return Range<size_t>(0, size); }

template <class T, class UniformRandomBitGenerator>
std::vector<T> ShuffledRange(T l, T u, UniformRandomBitGenerator&& g) {
  std::vector<T> range = Range(l, u);
  std::shuffle(range.begin(), range.end(), g);
  return range;
}
}  // namespace hr_edl

#endif  // HR_EDL_ENUMERATION_H_
