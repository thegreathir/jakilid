#ifndef JAKILID_SERIALIZER_HPP
#define JAKILID_SERIALIZER_HPP

#include "traits.hpp"

#include <algorithm>
#include <bits/c++config.h>
#include <boost/container/container_fwd.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/mpl/or.hpp>
#include <type_traits>

namespace jakilid {

using ByteArray = std::vector<std::uint8_t>;

template <class T> ByteArray ToByteArray(const T &t);

template <class T> T FromByteArray(const ByteArray &byte_array);

using SharedStringAllocator = boost::interprocess::allocator<
    uint8_t, boost::interprocess::managed_shared_memory::segment_manager>;

using SharedString =
    boost::interprocess::basic_string<uint8_t, std::char_traits<uint8_t>,
                                      SharedStringAllocator>;

template <class T, class SegmentManager>
auto emplace(std::size_t n, const uint8_t *data,
             const SegmentManager &segment_manager) {
  SharedString res{SharedStringAllocator(segment_manager)};
  res.resize(n + 1);

  res[0] = traits::type_char<T>::value[0];
  std::copy_n(data, n, std::next(res.data(), 1));
  return res;
}

// signed int -> long long

template <class T, class SegmentManager>
std::enable_if_t<traits::is_signed_integer<T>::value, SharedString>
Serialize(T t, const SegmentManager &segment_manager) {
  auto value = static_cast<long long>(t);
  return emplace<long long, SegmentManager>(
      sizeof(long long), reinterpret_cast<const uint8_t *>(&value),
      segment_manager);
}

template <class T, class = void>
std::enable_if_t<traits::is_signed_integer<T>::value, T>
Deserialize(const SharedString &str) {
  long long value =
      *reinterpret_cast<const long long *>(std::next(str.c_str(), 1));
  return static_cast<T>(value);
}

// unsigned int -> unsigned long long

template <class T, class SegmentManager>
std::enable_if_t<traits::is_unsigned_integer<T>::value, SharedString>
Serialize(T t, const SegmentManager &segment_manager) {
  auto value = static_cast<unsigned long long>(t);
  return emplace<unsigned long long, SegmentManager>(
      sizeof(unsigned long long), reinterpret_cast<const uint8_t *>(&value),
      segment_manager);
}

template <class T, class = void>
std::enable_if_t<traits::is_unsigned_integer<T>::value, T>
Deserialize(const SharedString &str) {
  unsigned long long value =
      *reinterpret_cast<const unsigned long long *>(std::next(str.c_str(), 1));
  return static_cast<T>(value);
}

// floating point -> long double

template <class T, class SegmentManager>
std::enable_if_t<std::is_floating_point_v<T>, SharedString>
Serialize(T t, const SegmentManager &segment_manager) {
  auto value = static_cast<long double>(t);
  return emplace<long double, SegmentManager>(
      sizeof(long double), reinterpret_cast<const uint8_t *>(&value),
      segment_manager);
}

template <class T, class = void>
std::enable_if_t<std::is_floating_point_v<T>, T>
Deserialize(const SharedString &str) {
  long double value =
      *reinterpret_cast<const long double *>(std::next(str.c_str(), 1));
  return static_cast<T>(value);
}

// bool, char, wchar

template <class T, class SegmentManager>
std::enable_if_t<boost::mpl::or_<traits::is_bool<T>, traits::is_char<T>,
                                 traits::is_wchar<T>>::value,
                 SharedString>
Serialize(T t, const SegmentManager &segment_manager) {
  return emplace<T, SegmentManager>(
      sizeof(T), reinterpret_cast<const uint8_t *>(&t), segment_manager);
}

template <class T, class = void>
std::enable_if_t<boost::mpl::or_<traits::is_bool<T>, traits::is_char<T>,
                                 traits::is_wchar<T>>::value,
                 T>
Deserialize(const SharedString &str) {
  return *reinterpret_cast<const T *>(std::next(str.c_str(), 1));
}

// std string

template <class T, class SegmentManager>
std::enable_if_t<traits::is_std_string<T>::value, SharedString>
Serialize(const T &t, const SegmentManager &segment_manager) {
  return emplace<T, SegmentManager>(
      t.size(), reinterpret_cast<const uint8_t *>(t.c_str()), segment_manager);
}

template <class T, class = void>
std::enable_if_t<traits::is_std_string<T>::value, T>
Deserialize(const SharedString &str) {
  return std::string(reinterpret_cast<const char *>(std::next(str.c_str(), 1)),
                     (str.size() - 1));
}

// std wstring

template <class T, class SegmentManager>
std::enable_if_t<traits::is_std_wstring<T>::value, SharedString>
Serialize(const T &t, const SegmentManager &segment_manager) {
  return emplace<T, SegmentManager>(
      t.size() * sizeof(wchar_t), reinterpret_cast<const uint8_t *>(t.c_str()),
      segment_manager);
}

template <class T, class = void>
std::enable_if_t<traits::is_std_wstring<T>::value, T>
Deserialize(const SharedString &str) {
  return std::wstring(
      reinterpret_cast<const wchar_t *>(std::next(str.c_str(), 1)),
      (str.size() - 1) / sizeof(wchar_t));
}

// c string

template <class T, class SegmentManager>
std::enable_if_t<traits::is_c_string<T>::value, SharedString>
Serialize(T t, const SegmentManager &segment_manager) {
  return emplace<T, SegmentManager>(
      std::strlen(t), reinterpret_cast<const uint8_t *>(t), segment_manager);
}

// c wstring

template <class T, class SegmentManager>
std::enable_if_t<traits::is_c_wstring<T>::value, SharedString>
Serialize(T t, const SegmentManager &segment_manager) {
  return emplace<T, SegmentManager>(std::wcslen(t) * sizeof(wchar_t),
                                    reinterpret_cast<const uint8_t *>(t),
                                    segment_manager);
}

} // namespace jakilid

#endif