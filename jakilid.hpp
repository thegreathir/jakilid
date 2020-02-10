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

using interprocess_string_allocator = boost::interprocess::allocator<uint8_t ,
        boost::interprocess::managed_shared_memory::segment_manager>;

using interprocess_string = boost::interprocess::basic_string<uint8_t , std::char_traits<uint8_t >,
        interprocess_string_allocator>;

template<class T, class SegmentManager>
std::enable_if_t<std::is_arithmetic<T>::value, interprocess_string>
cast_to_interprocess_string(T t, const SegmentManager &segment_manager) {
    return interprocess_string(reinterpret_cast<uint8_t *>(&t), sizeof(T),
            interprocess_string_allocator(segment_manager));
}

template<class T>
std::enable_if_t<std::is_arithmetic<T>::value, T> from_interprocess_string(interprocess_string str) {
    return *reinterpret_cast<T *>(str.data());
}

template<class T>
std::enable_if_t<std::is_arithmetic<T>::value, byte_array>
to_byte_array(T t) {
    byte_array result;
    std::copy_n(reinterpret_cast<uint8_t *>(&t), std::back_inserter(result), sizeof(T));
    return result;
}

template<class T>
std::enable_if_t<std::is_arithmetic<T>::value, T> from_byte_array(const byte_array& input) {
    return *reinterpret_cast<T *>(input.data());
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

    static std::false_type is_castable_imp(...);

    template<class T, class = decltype(cast_to_interprocess_string<T, decltype(manager())>)>
    static std::true_type is_castable_imp(T* t);

    template<class T>
    using is_castable = decltype(is_castable_imp((T*)0));

    template<class T>
    interprocess_string 

    template<class K = Key, class V = Value>
    std::enable_if_t<is_castable<K>::value && is_castable<V>::value, bool>
    insert(const Key &key, const Value &value) {
        return hash_map.insert(cast_to_interprocess_string(key, manager()), cast_to_interprocess_string(value, manager()));
    }

    Value find(const Key &key) const {
        return from_interprocess_string<Value>(hash_map.find(cast_to_interprocess_string(key, manager())));
    }
};

template<class Key, class Value>
std::unique_ptr<boost::interprocess::managed_shared_memory> Jakilid<Key, Value>::segment;

}

#endif //JAKILID_JAKILID_HPP
