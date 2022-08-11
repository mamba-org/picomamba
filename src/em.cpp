#include <emscripten/bind.h>
#include "mpool.hpp"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(my_class_example)
{
    register_vector<std::string>("StringList");
    class_<MPool>("Pool")
        .constructor<>()
        .function("load_repo", &MPool::load_repo)
        .function("solve", &MPool::solve);

    class_<PrefixData>("PrefixData").constructor<>().function("load", &PrefixData::load);

    class_<MRepo>("Repo").constructor<>().function("load", &PrefixData::load);
}
