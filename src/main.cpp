
// wget conda.anaconda.org/conda-forge/osx-arm64/repodata.json -O repodata.json
// wget conda.anaconda.org/conda-forge/noarch/repodata.json -O noarch_repodata.json

#include "mpool.hpp"

#include "mrepo.hpp"

using namespace mamba_wasm;

int
main(int argc, char** argv)
{
    MPool pool;

    pool.load_repo("repodata.json", "https://conda.anaconda.org/conda-forge/osx-arm64");
    pool.load_repo("noarch_repodata.json", "https://conda.anaconda.org/conda-forge/noarch");

    std::vector<std::string> specs;
    for (int i = 1; i < argc; ++i)
    {
        specs.push_back(argv[i]);
    }
    pool.solve(specs);
}
