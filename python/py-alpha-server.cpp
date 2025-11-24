
#include <alpha/Provider.hpp>
#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace pybind11::literals;

static std::string dict_to_json(const py::dict& d) {
    py::module json = py::module::import("json");
    py::object dumps = json.attr("dumps");
    py::object res = dumps(d);
    return res.cast<std::string>();
}


PYBIND11_MODULE(_pyalpha_server, m) {
    m.doc() = "Python binding for the Alpha server library";

    py::class_<alpha::Provider>(m, "Provider")
        .def(py::init([](py::object pyMargoEngine,
                         uint16_t provider_id,
                         const py::dict& config) {
            py::capsule mid = pyMargoEngine.attr("get_internal_mid")();
            auto config_str = dict_to_json(config);
            return alpha::Provider{thallium::engine{mid}, provider_id, config_str};
        }),
            R"(
            Provider constructor.

            Parameters
            ----------

            engine (pymargo.Engine): PyMargo Engine to use.
            provider_id (int): Provider ID.
            config (dict): Configuration of the provider.

            Returns
            -------

            A alpha.Provider instance.
            )",
            "engine"_a,
            "provider_id"_a,
            "config"_a,
            py::keep_alive<1, 2>())
        ;
}
