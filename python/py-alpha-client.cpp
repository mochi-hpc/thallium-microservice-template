
#include <alpha/Client.hpp>
#include <alpha/ResourceHandle.hpp>
#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace pybind11::literals;


/**
 * @brief This function can be called to create Python classes for
 * alpha::Future<T> C++ classes. It must be called for any Future<T>
 * the ResourceHandle may return.
 *
 * @tparam ResultType Type of result.
 * @param name Name of the type.
 * @param m Module.
 */
template<typename ResultType>
static void exportFutureType(const char* name, py::module_ m) {
    auto classname = std::string{"Future"} + name;
    py::class_<alpha::Future<ResultType>>(m, classname.c_str())
        .def("completed", &alpha::Future<ResultType>::completed,
             "Returns whether the future has completed.")
        .def("wait", &alpha::Future<ResultType>::wait,
             "Blocks until the future has completed, then returns the result.");
}


PYBIND11_MODULE(_pyalpha_client, m) {
    m.doc() = "Python binding for the Alpha client library";

    py::class_<alpha::Client>(m, "Client")
        .def(py::init([](const py::object& pyMargoEngine) {
            py::capsule mid = pyMargoEngine.attr("get_internal_mid")();
            return alpha::Client{mid};
        }), py::keep_alive<1, 2>(),
            R"(
            Client constructor.

            Parameters
            ----------

            engine (pymargo.Engine): PyMargo Engine to use.

            Returns
            -------

            A alpha.Client instance.
            )",
            "engine"_a)
        .def("make_resource_handle",
            py::overload_cast<const std::string&, uint16_t, bool>(
                 &alpha::Client::makeResourceHandle, py::const_),
            R"(
            Create a ResourceHandle.

            Parameters
            ----------

            address (str): Address of the process owning the resource.
            provider_id (int): Provider ID of the resource.
            check (Optional[bool]): Check that the provider exists.

            Returns
            -------

            A alpha.ResourceHandle instance.
            )",
            "address"_a, "provider_id"_a, "check"_a=false)
        ;

    py::class_<alpha::ResourceHandle>(m, "ResourceHandle")
        .def("compute_sum", &alpha::ResourceHandle::computeSum,
            R"(
            "Compute the sum of two numbers.

            Parameters
            ----------

            x (int): First number.
            y (int): Second number.

            Returns
            -------

            A Future object that the caller must wait on to get the result.
            )", "x"_a, "y"_a);

    exportFutureType<int32_t>("Int32", m);
}
