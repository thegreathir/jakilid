#ifndef JAKILID_JAKILID_HPP
#define JAKILID_JAKILID_HPP

#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <cuckoohash_map.hh>
#include <memory>
#include <type_traits>

namespace jakilid {

constexpr size_t segment_size = 1UL << 32;
constexpr char segment_name[] = "jakilid";

using byte_array = std::vector<std::uint8_t>;

template<class T>
byte_array to_byte_array(const T& t);

template<class T>
T from_byte_array(const byte_array &);

using interprocess_string_allocator = boost::interprocess::allocator<uint8_t,
        boost::interprocess::managed_shared_memory::segment_manager>;

using interprocess_string = boost::interprocess::basic_string<uint8_t, std::char_traits<uint8_t>,
        interprocess_string_allocator>;

template<class T, class SegmentManager>
std::enable_if_t<std::is_arithmetic<T>::value, interprocess_string>
to_interprocess_string(const T &t, const SegmentManager &segment_manager) {
    return interprocess_string(reinterpret_cast<const uint8_t *>(&t), sizeof(T),
            interprocess_string_allocator(segment_manager));
}

template<class T, class = void>
std::enable_if_t<std::is_arithmetic<T>::value, T> from_interprocess_string(const interprocess_string &str) {
    return *reinterpret_cast<const T *>(str.c_str());
}

template<class T, class SegmentManager>
std::enable_if_t<std::is_same<T, std::string>::value, interprocess_string>
to_interprocess_string(const T &t, const SegmentManager &segment_manager) {
    return interprocess_string(t.begin(), t.end(), interprocess_string_allocator(segment_manager));
}

template<class T, class = void>
std::enable_if_t<std::is_same<T, std::string>::value, T>
from_interprocess_string(const interprocess_string &str) {
    return std::string(reinterpret_cast<const char *>(str.c_str()), str.size());
}

template<class Key, class Value>
class Jakilid {
private:

    using underlying_key_type = interprocess_string;
    using underlying_mapped_type = interprocess_string;
    using value_type = std::pair<underlying_key_type, underlying_mapped_type>;
    using allocator_type = boost::interprocess::allocator<value_type,
            boost::interprocess::managed_shared_memory::segment_manager>;

    using hasher = boost::hash<underlying_key_type>;
    using key_equal = std::equal_to<underlying_key_type>;

    using hash_map_type = cuckoohash_map<underlying_key_type, underlying_mapped_type, hasher, key_equal,
            allocator_type>;

    static std::unique_ptr<boost::interprocess::managed_shared_memory> segment;

    static auto manager() {
        return segment->get_segment_manager();
    }

#define GENERATE_FIND(function_name) \
    static std::false_type find_##function_name##_impl(...); \
    template<class T, class = decltype(function_name<T, decltype(manager())>)> \
    static std::true_type find_##function_name##_impl(T* t); \
    template<class T> \
    using find_##function_name = decltype(find_##function_name##_impl((T *) 0)) \


    GENERATE_FIND(to_interprocess_string);

    GENERATE_FIND(from_interprocess_string);
#undef GENERATE_FIND

    template<class T>
    std::enable_if_t<find_to_interprocess_string<T>::value, interprocess_string>
    internal_to_string(const T &t) const {
        return to_interprocess_string(t, manager());
    }

    template<class T>
    std::enable_if_t<!find_to_interprocess_string<T>::value, interprocess_string>
    internal_to_string(const T &t) const {
        auto result = to_byte_array(t);
        return interprocess_string(result.data(), result.size(), manager());
    }

    template<class T>
    std::enable_if_t<find_from_interprocess_string<T>::value, T>
    internal_from_string(const interprocess_string& str) const {
        return from_interprocess_string<T>(str);
    }

    template<class T>
    std::enable_if_t<!find_from_interprocess_string<T>::value, T>
    internal_from_string(const interprocess_string& str) const {
        return from_byte_array<T>(byte_array(str.begin(), str.end()));
    }

    hash_map_type hash_map;

public:

    Jakilid()
    : hash_map(LIBCUCKOO_DEFAULT_SIZE, hasher(), key_equal(), allocator_type(segment->get_segment_manager()))
    {}

    static Jakilid *get_instance(const std::string &name) {
        if (!segment) {
            segment = std::make_unique<boost::interprocess::managed_shared_memory>(
                    boost::interprocess::open_or_create, segment_name, segment_size
            );
        }

        return segment->find_or_construct<Jakilid>(name.c_str())();
    }

    bool insert(const Key &key, const Value &value) {
        return hash_map.insert(internal_to_string(key), internal_to_string(value));
    }

    Value find(const Key &key) const {
        return internal_from_string<Value>(hash_map.find(internal_to_string(key)));
    }
};

template<class Key, class Value>
std::unique_ptr<boost::interprocess::managed_shared_memory> Jakilid<Key, Value>::segment;

}

#endif //JAKILID_JAKILID_HPP
