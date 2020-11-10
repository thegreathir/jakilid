#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <stdexcept>

#include "jakilid.hpp"
#include "serializer.hpp"
#include "mp.hpp"
#include "traits.hpp"

namespace py = pybind11;

using python_types = std::tuple<int, float, std::string>;

using int_key = jakilid::mp::construct_pair_tuple<int, python_types>::type;
using str_key = jakilid::mp::construct_pair_tuple<std::string, python_types>::type;

using all_types = jakilid::mp::concat_tuple<int_key, str_key>::type;

template <class, class ...>
struct add_insert{};

template<class Class_, class Pair, class ...Pairs>
struct add_insert<Class_, std::tuple<Pair, Pairs...>> {
    void operator()(Class_& class_) {
        class_.def("insert",
            &jakilid::SharedDict::Insert<typename Pair::first_type,
                                        typename Pair::second_type>);

        add_insert<Class_, std::tuple<Pairs...>>()(class_);
    }
};

template<class Class_, class Pair>
struct add_insert<Class_, std::tuple<Pair>> {
    void operator()(Class_& class_) {
        class_.def("insert",
            &jakilid::SharedDict::Insert<typename Pair::first_type,
                                        typename Pair::second_type>);
    }
};

template<class T, class PyT>
std::pair<std::string, std::function<py::object()>>
get_implemention(const jakilid::SharedString& raw_value) {
    return {
        {jakilid::traits::type_char<T>::value, jakilid::kHeaderSize}, [&raw_value]() {
            return PyT(
                jakilid::Deserialize<T>(raw_value)
            );
        }
    };
}

template<class Key>
py::object find(const jakilid::SharedDict& dict, const Key& key) {
    auto raw_value = dict.FindRaw(key);

    if (!raw_value)
        return py::none();

    auto implementions = std::map<std::string, std::function<py::object()>> {
        get_implemention<int, py::int_>(raw_value.value()),
        get_implemention<float, py::float_>(raw_value.value()),
        get_implemention<std::string, py::str>(raw_value.value()),
    };
    std::string header = "";
    for (size_t i = 0; i < jakilid::kHeaderSize; ++i)
        header += raw_value.value()[i];

    auto it = implementions.find(header);
    if (it == implementions.end())
        throw std::domain_error("Value type not supported in python");

    return (it->second)();
}

PYBIND11_MODULE(jakilid, m) {
    m.doc() = "Interprocess concurrent dictionary";

    auto shared_dict_class = py::class_<jakilid::SharedDict>(m, "SharedDict")
        .def(py::init<const std::string&>())
        .def("find", find<std::string>)
        .def("find", find<int>);

    add_insert<decltype(shared_dict_class), all_types>()(shared_dict_class);
}
