#ifndef JAKILID_JAKILID_HPP
#define JAKILID_JAKILID_HPP

#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <cuckoohash_map.hh>
#include <memory>
#include <type_traits>

namespace jakilid {

constexpr size_t kSegmentSize = 1UL << 32;
constexpr auto kSegmentName = "jakilid";

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

template<class Key, class Value>
class Jakilid {
public:
    static Jakilid *GetInstance(const std::string &name) {
        if (!segment_) {
            segment_ = std::make_unique<boost::interprocess::managed_shared_memory>(
                    boost::interprocess::open_or_create, kSegmentName, kSegmentSize
            );
        }

        return segment_->find_or_construct<Jakilid>(name.c_str())();
    }

    bool Empty() const {
        return hash_map_.empty();
    }

    std::size_t Size() const {
        return hash_map_.size();
    }

    bool Find(const Key &key, Value &val) const {
        SharedString res;
        if (!hash_map_.find(InternalSerialize(key), res))
            return false;
        val = InternalDeserialize(res);
        return true;
    }

    Value Find(const Key &key) const {
        return InternalDeserialize<Value>(hash_map_.find(InternalSerialize(key)));
    }

    bool Contains(const Key &key) const {
        return hash_map_.contains(key);
    }

    bool Insert(const Key &key, const Value &value) {
        return hash_map_.insert(InternalSerialize(key), InternalSerialize(value));
    }

    bool Update(const Key &key, const Value &value) {
        return hash_map_.update(InternalSerialize(key), InternalSerialize(value));
    }

    bool Erase(const Key &key) {
        return hash_map_.erase(InternalSerialize(key));
    }

    Jakilid()
    : hash_map_(LIBCUCKOO_DEFAULT_SIZE, Hasher(), KeyEqual(), Allocator(segment_->get_segment_manager()))
    {}

private:
    using UnderlyingKey = SharedString;
    using UnderlyingValue = SharedString;
    using Combined = std::pair<UnderlyingKey, UnderlyingValue>;
    using Allocator = boost::interprocess::allocator<Combined,
            boost::interprocess::managed_shared_memory::segment_manager>;

    using Hasher = boost::hash<UnderlyingKey>;
    using KeyEqual = std::equal_to<UnderlyingKey>;

    using HashMap = cuckoohash_map<UnderlyingKey, UnderlyingValue, Hasher, KeyEqual, Allocator>;

    static auto manager() {
        return segment_->get_segment_manager();
    }

#define GENERATE_FIND(function_name) \
    static std::false_type Find##function_name##Impl(...); \
    template<class T, class = decltype(function_name<T, decltype(manager())>)> \
    static std::true_type Find##function_name##Impl(T *t); \
    template<class T> \
    using Find##function_name = decltype(Find##function_name##Impl((T *) 0)) \

    GENERATE_FIND(Serialize);
    GENERATE_FIND(Deserialize);
#undef GENERATE_FIND

    template<class T>
    std::enable_if_t<FindSerialize<T>::value, SharedString>
    static InternalSerialize(const T &t) {
        return Serialize(t, manager());
    }

    template<class T>
    std::enable_if_t<!FindSerialize<T>::value, SharedString>
    static InternalSerialize(const T &t) {
        auto result = ToByteArray(t);
        return SharedString(result.data(), result.size(), manager());
    }

    template<class T>
    std::enable_if_t<FindDeserialize<T>::value, T>
    static InternalDeserialize(const SharedString &str) {
        return Deserialize<T>(str);
    }

    template<class T>
    std::enable_if_t<!FindDeserialize<T>::value, T>
    static InternalDeserialize(const SharedString &str) {
        return FromByteArray<T>(ByteArray(str.begin(), str.end()));
    }

    HashMap hash_map_;

    inline static std::unique_ptr<boost::interprocess::managed_shared_memory> segment_;
};

}

#endif //JAKILID_JAKILID_HPP
