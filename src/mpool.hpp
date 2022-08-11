#include <iostream>
#include <vector>
#include <string>

extern "C"
{
#include "solv/solver.h"
#include "solv/transaction.h"
#include "solv/repo_conda.h"
#include "solv/conda.h"
#include "solv/repo_solv.h"
#include "solv/pool.h"
}


namespace mamba_wasm
{
    class MRepo;

    void transaction_to_txt(Transaction* transaction);

    class MPool
    {
    public:
        MPool();
        ~MPool();
        Pool* get();
        void load_repo(const std::string& fname, const std::string& url);
        std::vector<std::string> solve(const std::vector<std::string>& match_specs);

        MRepo& add_repo(MRepo&& repo);
        operator Pool*()
        {
            return m_pool;
        }

        std::vector<MRepo> m_repo_list;
        Pool* m_pool;
    };
}
