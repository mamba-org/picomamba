#include <iostream>
#include <cstdint>
#include <string>

#include <nlohmann/json.hpp>
#include "picomamba/picomamba_core.hpp"

int main(const int argc, const char ** argv)
{   
    if (argc < 4)
    {
        std::cerr << "Usage: picomamba <noarch_conda_forge_repodata_path> <noarch_emscripten_forge>  <arch_emscripten_forge>" << std::endl;
        return 1;
    }

    std::string noarch_conda_forge_repodata_path = argv[1];
    std::string noarch_emscripten_forge = argv[2];
    std::string arch_emscripten_forge = argv[3];

    picomamba::PicoMambaCore picomamba_core;
    picomamba_core.load_repodata_from_file(noarch_conda_forge_repodata_path, "noarch_conda_forge");
    picomamba_core.load_repodata_from_file(noarch_emscripten_forge, "noarch_emscripten_forge");
    picomamba_core.load_repodata_from_file(arch_emscripten_forge, "arch_emscripten_forge");

    std::vector<std::string> specs = {"python", "numpy"};

    picomamba_core.solve(specs.begin(), specs.end(), picomamba::PicoMambaCore::SolveConfig{false, true}, 
    
    [&](Transaction * transaction){
        std::cout << "Solving done: " << std::endl;

        Pool* pool = transaction->pool;

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
                
                    s2 = transaction->pool->solvables + transaction_obs_pkg(transaction, p);
                    s2 = pool_id2solvable(pool, i2);
                    auto t = as_tuple(s);
                    std::cout << "Removing " << std::get<0>(t) << std::get<1>(t) << std::get<2>(t) << std::get<3>(t) << std::get<4>(t) << std::endl;
                    break;
                }
                case SOLVER_TRANSACTION_ERASE:
                {
                    auto t = as_tuple(s);
                    std::cout << "Removing " << std::get<0>(t) << std::get<1>(t) << std::get<2>(t) << std::get<3>(t) << std::get<4>(t) << std::endl;
                    break;
                }
                case SOLVER_TRANSACTION_INSTALL:
                {
                    auto t = as_tuple(s);
                    std::cout << "Installing " << std::get<0>(t) << std::get<1>(t) << std::get<2>(t) << std::get<3>(t) << std::get<4>(t) << std::endl;
                    break;
                }
                case SOLVER_TRANSACTION_IGNORE:
                {
                    auto t = as_tuple(s);
                    std::cout << "Ignoring " << std::get<0>(t) << std::get<1>(t) << std::get<2>(t) << std::get<3>(t) << std::get<4>(t) << std::endl;
                    break;
                }
                default:
                    std::cout << "Some weird case not handled" << std::endl;
                    break;
            }
        }






    });

}