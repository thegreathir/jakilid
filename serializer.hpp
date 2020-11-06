#ifndef JAKILID_SERIALIZER_HPP
#define JAKILID_SERIALIZER_HPP


#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <type_traits>

namespace jakilid {

using ByteArray = std::vector<std::uint8_t>;

template<class T>
ByteArray ToByteArray(const T &t);

template<class T>
T FromByteArray(const ByteArray &byte_array);

using SharedStringAllocator = boost::interprocess::allocator<uint8_t,
        boost::interprocess::managed_shared_memory::segment_manager>;

using SharedString = boost::interprocess::basic_string<uint8_t, std::char_traits<uint8_t>,
        SharedStringAllocator>;

template<class T, class SegmentManager>
std::enable_if_t<std::is_arithmetic<T>::value, SharedString>
Serialize(const T &t, const SegmentManager &segment_manager) {
    return SharedString(reinterpret_cast<const uint8_t *>(&t), sizeof(T), SharedStringAllocator(segment_manager));
}

template<class T, class = void>
std::enable_if_t<std::is_arithmetic<T>::value, T>
Deserialize(const SharedString &str) {
    return *reinterpret_cast<const T *>(str.c_str());
}

template<class T, class SegmentManager>
std::enable_if_t<std::is_same<T, std::string>::value, SharedString>
Serialize(const T &t, const SegmentManager &segment_manager) {
    return SharedString(t.begin(), t.end(), SharedStringAllocator(segment_manager));
}

template<class T, class = void>
std::enable_if_t<std::is_same<T, std::string>::value, T>
Deserialize(const SharedString &str) {
    return std::string(reinterpret_cast<const char *>(str.c_str()), str.size());
}

template<class T, class SegmentManager>
std::enable_if_t<helper::is_c_string<T>::value, SharedString>
Serialize(T t, const SegmentManager &segment_manager) {
    return SharedString(reinterpret_cast<const uint8_t *>(t), SharedStringAllocator(segment_manager));
}

}

#endif