#ifndef JAKILID_TEST_MP_HPP
#define JAKILID_TEST_MP_HPP

#include <type_traits>
#include <utility>
#include <tuple>

namespace jakilid {

namespace mp {

template<class T, class U ,class ...Ts> 
struct another {
    using type = std::conditional_t<std::is_same_v<T, U>,
            typename another<T, Ts...>::type, U>;
};

template<class T, class U>
struct another<T, U> {
    using type = std::conditional_t<std::is_same_v<T, U>, void, U>;
};

template<class T, class U>
struct tuple_another;

template<class T, class ...Ts>
struct tuple_another<T, std::tuple<Ts...>> : another<T, Ts...> {};

static_assert(std::is_same_v<another<bool, int, int, bool>::type, int>);
static_assert(std::is_same_v<another<float, float, float, float>::type, void>);
static_assert(std::is_same_v<tuple_another<int, std::tuple<int, short, double>>::type, short>);

template<class First, class T>
struct construct_pair_tuple;

template<class First, class ...Ts>
struct construct_pair_tuple<First, std::tuple<Ts...>> {
    using type = std::tuple<std::pair<First, Ts>...>;
};

template <class ...Args>
struct concat_tuple;

template <class... First, class... Second, class ...Args>
struct concat_tuple<std::tuple<First...>, std::tuple<Second...>, Args...> {
    using type = typename concat_tuple<std::tuple<First..., Second...>, Args...>::type;
};

template <class... First, class... Second>
struct concat_tuple<std::tuple<First...>, std::tuple<Second...>> {
    using type = std::tuple<First..., Second...>;
};


static_assert(std::is_same_v<
    std::tuple<int, float, char, double, void, void>
    , concat_tuple<
        std::tuple<int, float>,
        std::tuple<char, double, void>,
        std::tuple<void>
    >::type
>);

template<class Tuple, template<class...> class GTestType>
struct tuple_to_gtest_types;

template <template<class...> class GTestType, class ...Ts>
struct tuple_to_gtest_types<std::tuple<Ts...>, GTestType>
{
    using types = GTestType<Ts...>;
};

}

}

#endif