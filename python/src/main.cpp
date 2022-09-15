#include "pybind11/pybind11.h"



#include <iostream>
#include <numeric>
#include <string>
#include <sstream>


// our headers
#include "picomamba/picomamba.hpp"
#include "picomamba/picomamba_config.hpp"

namespace py = pybind11;



namespace picomamba {


    // implementation in def_myclass.cpp
    void def_pikomamba_core(py::module & m);

    // implementation in def.cpp
    void def_build_config(py::module & m);

}


// Python Module and Docstrings
PYBIND11_MODULE(_picomamba , module)
{
    module.doc() = R"pbdoc(
        _picomamba  python bindings

        .. currentmodule:: _picomamba 

        .. autosummary::
           :toctree: _generate

           BuildConfiguration
           PikoMambaCore
    )pbdoc";

    picomamba::def_build_config(module);
    picomamba::def_pikomamba_core(module);

    // make version string
    std::stringstream ss;
    ss<<PICOMAMBA_VERSION_MAJOR<<"."
      <<PICOMAMBA_VERSION_MINOR<<"."
      <<PICOMAMBA_VERSION_PATCH;
    module.attr("__version__") = ss.str().c_str();
}