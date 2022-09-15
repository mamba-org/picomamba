#include "pybind11/pybind11.h"
#include "pybind11/numpy.h"
#include "pybind11/stl.h"

#include <iostream>
#include <numeric>


// our headers
#include "picomamba/picomamba_core.hpp"

namespace py = pybind11;



namespace picomamba {


    inline py::dict transaction_to_py(Transaction* transaction)
    {
        Pool* pool = transaction->pool;
        py::dict res;

        py::list remove_list;
        py::list install_list;
        py::list ignore_list;

        res["remove"] = remove_list;
        res["install"] = install_list;
        res["ignore"] = ignore_list;

        auto as_tuple = [&pool](Solvable* s)
        {
            const char* name = pool_id2str(pool, s->name);
            const char* evr = pool_id2str(pool, s->evr);
            const char* build_string;
            build_string = solvable_lookup_str(s, SOLVABLE_BUILDFLAVOR);
            int build_number = std::stoi(solvable_lookup_str(s, SOLVABLE_BUILDVERSION));

            return std::make_tuple(name, evr, build_string, build_number, s->repo->name);
        };

        for (int i = 0; i < transaction->steps.count; i++)
        {
            Id p = transaction->steps.elements[i];
            Id ttype = transaction_type(transaction, p, SOLVER_TRANSACTION_SHOW_ALL);
            Solvable* s = pool_id2solvable(transaction->pool, p);
            Id i2;
            Solvable* s2;
            switch (ttype)
            {
                case SOLVER_TRANSACTION_DOWNGRADED:
                case SOLVER_TRANSACTION_UPGRADED:
                case SOLVER_TRANSACTION_CHANGED:
                case SOLVER_TRANSACTION_REINSTALLED:
                {
                    // std::cout << "Removing " << as_tuple(s) << std::endl;

                    remove_list.append(as_tuple(s));
                    s2 = transaction->pool->solvables + transaction_obs_pkg(transaction, p);
                    s2 = pool_id2solvable(pool, i2);
                    install_list.append(as_tuple(s));
                    break;
                }
                case SOLVER_TRANSACTION_ERASE:
                {
                    // std::cout << "Removing " << as_tuple(s) << std::endl;
                    remove_list.append(as_tuple(s));
                    break;
                }
                case SOLVER_TRANSACTION_INSTALL:
                {
                    // std::cout << "Installing " << as_tuple(s) << std::endl;
                    install_list.append(as_tuple(s));
                    break;
                }
                case SOLVER_TRANSACTION_IGNORE:
                    ignore_list.append(as_tuple(s));
                    break;
                default:
                    std::cout << "Some weird case not handled" << std::endl;
                    break;
            }
        }
        return res;
    }



    void def_pikomamba_core(py::module & m)
    {   

        py::class_<PicoMambaCore::SolveConfig>(m, "PicoMambaCoreSolveConfig")
            .def(py::init<>())
        ;

        py::class_<PicoMambaCore>(m, "_PicoMambaCore")
        .def(py::init<>())
        .def("_load_repodata_from_file", &PicoMambaCore::load_repodata_from_file, py::arg("path"),py::arg("name"))
        .def("_load_installed", &PicoMambaCore::load_installed, py::arg("prefix"))
        // .def("_add_to_installed", &PicoMambaCore::add_to_installed)
        .def("_solve",[](PicoMambaCore & self, const std::vector<std::string>& match_specs, const PicoMambaCore::SolveConfig & config)
        {

            py::dict res;
            self.solve(
                match_specs.begin(), 
                match_specs.end(), 
                config,
                [&](Transaction * transaction){
                    res = transaction_to_py(transaction);
                }
            );
            return res;
        })
        ;
    }

}
