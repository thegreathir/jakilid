#ifndef JAKILID_JAKILID_HPP
#define JAKILID_JAKILID_HPP

#include "helper.hpp"

#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <cuckoohash_map.hh>
#include <memory>
#include <type_traits>

namespace jakilid {

constexpr size_t kSegmentSize = (1UL << 20) * 500;
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

template<class T, class SegmentManager>
std::enable_if_t<helper::is_c_string<T>::value, SharedString>
Serialize(T t, const SegmentManager &segment_manager) {
    return SharedString(reinterpret_cast<const uint8_t *>(t), SharedStringAllocator(segment_manager));
}

class SharedDict {
public:

    SharedDict(const std::string& name)
    : name(name)
    , hash_map_(nullptr)
    {
        open();
    }

    bool Drop() {
        hash_map_ = nullptr;
        return segment_->destroy<HashMap>(name.c_str());
    }

    bool Empty() const {
        open();
        return hash_map_->empty();
    }

    std::size_t Size() const {
        open();
        return hash_map_->size();
    }


    template<class Key, class Value>
    bool Find(const Key &key, Value &val) const {
        open();
        SharedString res(manager());
        if (!hash_map_->find(InternalSerialize(key), res))
            return false;
        val = InternalDeserialize<Value>(res);
        return true;
    }


    template<class Key>
    bool Contains(const Key &key) const {
        open();
        return hash_map_->contains(key);
    }


    template<class Key, class Value>
    bool Insert(const Key &key, const Value &value) {
        open();
        return hash_map_->upsert(InternalSerialize(key), [value](HashMap::mapped_type& v) {
            v = InternalSerialize(value);
        }
        , InternalSerialize(value));
    }


    template<class Key, class Value>
    bool Update(const Key &key, const Value &value) {
        open();
        return hash_map_->update(InternalSerialize(key), InternalSerialize(value));
    }


    template<class Key>
    bool Erase(const Key &key) {
        open();
        return hash_map_->erase(InternalSerialize(key));
    }

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

    void open_manager() const {
        if (!segment_) {
            segment_ = std::make_unique<boost::interprocess::managed_shared_memory>(
                    boost::interprocess::open_or_create, kSegmentName, kSegmentSize
            );
        }
    }

    void open() const {
        if (hash_map_ == nullptr) {
            open_manager();
            hash_map_ = segment_->find_or_construct<HashMap>(name.c_str())(
                LIBCUCKOO_DEFAULT_SIZE, Hasher(), KeyEqual(), Allocator(manager()));
        }
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

    const std::string name;
    mutable HashMap *hash_map_;

    inline static thread_local std::unique_ptr<boost::interprocess::managed_shared_memory> segment_;
};

}

#endif //JAKILID_JAKILID_HPP
