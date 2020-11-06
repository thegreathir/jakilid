#ifndef JAKILID_SERIALIZER_HPP
#define JAKILID_SERIALIZER_HPP

#include "helper.hpp"

#include <algorithm>
#include <bits/c++config.h>
#include <boost/container/container_fwd.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/mpl/or.hpp>
#include <type_traits>

namespace jakilid {

template <class T, class = void> struct __type_char__helper {
  static constexpr char value = '*';
};

template <class T>
struct __type_char__helper<
    T, std::enable_if_t<helper::is_numerical_integral<T>::value>> {
  static constexpr char value = 'i';
};

template <class T>
struct __type_char__helper<T, std::enable_if_t<std::is_floating_point_v<T>>> {
  static constexpr char value = 'f';
};

template <class T>
struct __type_char__helper<
    T, std::enable_if_t<boost::mpl::or_<helper::is_c_string<T>,
                                        helper::is_std_string<T>>::value>> {
  static constexpr char value = 's';
};

template <class T>
struct __type_char__helper<T, std::enable_if_t<helper::is_bool<T>::value>> {
  static constexpr char value = 'b';
};

template <class T>
struct __type_char__helper<T, std::enable_if_t<helper::is_char<T>::value>> {
  static constexpr char value = 'c';
};

template <class T>
struct type_char : __type_char__helper<std::remove_cv_t<T>> {};

using ByteArray = std::vector<std::uint8_t>;

template <class T> ByteArray ToByteArray(const T &t);

template <class T> T FromByteArray(const ByteArray &byte_array);

using SharedStringAllocator = boost::interprocess::allocator<
    uint8_t, boost::interprocess::managed_shared_memory::segment_manager>;

using SharedString =
    boost::interprocess::basic_string<uint8_t, std::char_traits<uint8_t>,
                                      SharedStringAllocator>;

template<class T, class SegmentManager>
auto emplace(std::size_t n, const uint8_t* data, const SegmentManager &segment_manager) {
    SharedString res{SharedStringAllocator(segment_manager)};
    res.resize(n + 1);

    res[0] = type_char<T>::value;
    std::copy_n(data, n, std::next(res.data(), 1));
    return res;
}

template <class T, class SegmentManager>
std::enable_if_t<std::is_arithmetic<T>::value, SharedString>
Serialize(const T &t, const SegmentManager &segment_manager) {
  return emplace<T, SegmentManager>(sizeof(T), reinterpret_cast<const uint8_t *>(&t), segment_manager);
}

template <class T, class = void>
std::enable_if_t<std::is_arithmetic<T>::value, T>
Deserialize(const SharedString &str) {
  return *reinterpret_cast<const T *>(str.c_str());
}

template <class T, class SegmentManager>
std::enable_if_t<helper::is_std_string<T>::value, SharedString>
Serialize(const T &t, const SegmentManager &segment_manager) {
  return emplace<T, SegmentManager>(t.size(), reinterpret_cast<const uint8_t *>(t.c_str()), segment_manager);
}

template <class T, class = void>
std::enable_if_t<helper::is_std_string<T>::value, T>
Deserialize(const SharedString &str) {
  return typename std::decay<T>::type(reinterpret_cast<const char *>(str.c_str()), str.size());
}

template <class T, class SegmentManager>
std::enable_if_t<helper::is_c_string<T>::value, SharedString>
Serialize(T t, const SegmentManager &segment_manager) {
  return emplace<T, SegmentManager>(std::strlen(t), reinterpret_cast<const uint8_t *>(t), segment_manager);
}

} // namespace jakilid

#endif