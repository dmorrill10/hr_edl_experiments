#ifndef HR_EDL_CONTAINERS_H_
#define HR_EDL_CONTAINERS_H_

#include <vector>

#include "open_spiel/spiel.h"

namespace hr_edl {

template <class K, class V, template <class... Types> class Container,
          class... Args>
V& GetOrCreateFromArgs(Container<K, V>& map, const K& key, Args... args) {
  auto iter = map.find(key);
  if (iter == map.end()) {
    auto pair =
        map.emplace(std::piecewise_construct, std::forward_as_tuple(key),
                    std::forward_as_tuple(args...));
    return pair.first->second;
  }
  return iter->second;
}

template <class K, class V, template <class... Types> class Container>
V& GetOrCreate(Container<K, V>& map, const K& key,
               const std::function<V()>& init) {
  auto iter = map.find(key);
  if (iter == map.end()) {
    auto pair = map.emplace(key, init());
    return pair.first->second;
  }
  return iter->second;
}

template <class K, class V, template <class... Types> class Container,
          class... Args>
V& GetOrCreate(Container<K, V>& map, const K& key,
               const std::function<V(Args...)>& init, Args... init_args) {
  auto iter = map.find(key);
  if (iter == map.end()) {
    auto pair = map.emplace(key, init(init_args...));
    return pair.first->second;
  }
  return iter->second;
}

template <class K, class V, template <class... Types> class Container>
V& GetOrCreate(Container<K, V>& map, const K& key,
               const std::function<V(const K& key)>& init) {
  auto iter = map.find(key);
  if (iter == map.end()) {
    auto pair = map.emplace(key, init(key));
    return pair.first->second;
  }
  return iter->second;
}

template <class Value, template <class... Types> class Container>
[[deprecated]] inline bool Contains(const Container<Value>& s, const Value& k) {
  return s.find(k) != s.end();
}

template <class Key, class Value, template <class... Types> class Container>
[[deprecated]] inline bool Contains(const Container<Key, Value>& s, const Key& k) {
  return s.find(k) != s.end();
}

template <template <class... Types> class Container, class T>
void Reserve(Container<T>& v, int n) {
  if (n > 0) v.reserve(n);
}

template <class T>
std::vector<T> Concat(std::vector<T>&& a, const std::vector<T>& b) {
  size_t prev_size = a.size();
  a.resize(a.size() + b.size());
  std::copy(b.begin(), b.end(), a.begin() + prev_size);
  return a;
}

template <class T>
void Concat(std::vector<T>& a, const std::vector<T>& b) {
  size_t prev_size = a.size();
  a.resize(a.size() + b.size());
  std::copy(b.begin(), b.end(), a.begin() + prev_size);
}

template <class In, class Out>
std::vector<Out> Map(const std::vector<In>& a, const std::function<Out(const In&)>& f) {
  std::vector<Out> b;
  b.reserve(a.size());
  for (const auto& a_i : a) {
    b.push_back(f(a_i));
  }
  return b;
}
template <class In, class Out>
std::vector<Out> Map(const std::vector<In>& a,
                     const std::function<Out(In)>& f) {
  std::vector<Out> b;
  b.reserve(a.size());
  for (const auto& a_i : a) {
    b.push_back(f(a_i));
  }
  return b;
}
}  // namespace hr_edl

#endif  // HR_EDL_CONTAINERS_H_
