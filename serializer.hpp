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
#include <stdexcept>
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

constexpr size_t kHeaderSize = 2;

template <class T, class SegmentManager>
auto emplace(std::size_t n, const uint8_t *data,
             const SegmentManager &segment_manager) {
  SharedString res{SharedStringAllocator(segment_manager)};
  res.resize(n + kHeaderSize);

  res[0] = traits::type_char<T>::value[0];
  res[1] = traits::type_char<T>::value[1];
  std::copy_n(data, n, std::next(res.data(), kHeaderSize));
  return res;
}

template<class T>
void raise_for_type(const SharedString &str) {
  for (size_t i = 0; i < kHeaderSize; ++i) {
    if (traits::type_char<T>::value[i] != static_cast<char>(str[i]))
      throw std::domain_error("Type mismatch in deserializing");
  }
}

// bool, char, wchar, integer, real

template <class T, class SegmentManager>
std::enable_if_t<
    std::disjunction<traits::is_bool<T>, traits::is_char<T>,
                     traits::is_wchar<T>, traits::is_signed_integer<T>,
                     traits::is_unsigned_integer<T>, traits::is_real<T>>::value,
    SharedString>
Serialize(T t, const SegmentManager &segment_manager) {
  return emplace<T, SegmentManager>(
      sizeof(T), reinterpret_cast<const uint8_t *>(&t), segment_manager);
}

template <class T, class = void>
std::enable_if_t<
    std::disjunction<traits::is_bool<T>, traits::is_char<T>,
                     traits::is_wchar<T>, traits::is_signed_integer<T>,
                     traits::is_unsigned_integer<T>, traits::is_real<T>>::value,
    T>
Deserialize(const SharedString &str) {
  raise_for_type<T>(str);
  return *reinterpret_cast<const T *>(std::next(str.c_str(), kHeaderSize));
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
  raise_for_type<T>(str);
  return std::string(reinterpret_cast<const char *>(std::next(str.c_str(), kHeaderSize)),
                     (str.size() - kHeaderSize));
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
  raise_for_type<T>(str);
  return std::wstring(
      reinterpret_cast<const wchar_t *>(std::next(str.c_str(), kHeaderSize)),
      (str.size() - kHeaderSize) / sizeof(wchar_t));
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