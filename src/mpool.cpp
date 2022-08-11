#include "mpool.hpp"
#include "mrepo.hpp"

namespace mamba_wasm
{
    void transaction_to_txt(Transaction* transaction)
    {
        Pool* pool = transaction->pool;

        auto to_stream = [&pool](Solvable* s)
        {
            const char* name = pool_id2str(pool, s->name);
            const char* evr = pool_id2str(pool, s->evr);
            const char* build_string;
            build_string = solvable_lookup_str(s, SOLVABLE_BUILDFLAVOR);
            std::string res(name);
            res.append("-");
            res.append(evr);
            res.append("-");
            res.append(build_string);
            return res;
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
                    std::cout << "Removing " << to_stream(s) << std::endl;
                    s2 = transaction->pool->solvables + transaction_obs_pkg(transaction, p);
                    s2 = pool_id2solvable(pool, i2);
                    std::cout << "Installing " << to_stream(s2) << std::endl;
                    break;
                }
                case SOLVER_TRANSACTION_ERASE:
                {
                    std::cout << "Removing " << to_stream(s) << std::endl;
                    break;
                }
                case SOLVER_TRANSACTION_INSTALL:
                {
                    std::cout << "Installing " << to_stream(s) << std::endl;
                    break;
                }
                case SOLVER_TRANSACTION_IGNORE:
                    break;
                default:
                    std::cout << "Some weird case not handled" << std::endl;
                    break;
            }
        }
    }

    MPool::MPool()
        : m_pool(pool_create()){};

    MPool::~MPool()
    {
        pool_free(m_pool);
    }

    Pool* MPool::get()
    {
        return m_pool;
    }

    void MPool::load_repo(const std::string& fname, const std::string& url)
    {
        FILE* f = fopen(fname.c_str(), "r");
        Repo* r = repo_create(m_pool, url.c_str());
        repo_add_conda(r, f, 0);
        fclose(f);

        // return r;
    }

    std::vector<std::string> MPool::solve(const std::vector<std::string>& match_specs)
    {
        std::cout << "Solving for ";
        for (auto& el : match_specs)
            std::cout << el << " ";
        std::cout << std::endl;

        Solver* s = solver_create(m_pool);

        Queue q;
        queue_init(&q);

        for (auto& spec : match_specs)
        {
            Id inst_id = pool_conda_matchspec(m_pool, spec.c_str());
            queue_push2(&q, SOLVER_INSTALL | SOLVER_SOLVABLE_PROVIDES, inst_id);
        }

        solver_solve(s, &q);

        bool success = solver_problem_count(s) == 0;
        Transaction* trans = solver_create_transaction(s);

        transaction_order(trans, 0);

        transaction_to_txt(trans);

        transaction_free(trans);
        solver_free(s);
        queue_free(&q);

        return { "yippie" };
    }

    MRepo& MPool::add_repo(MRepo&& repo)
    {
        m_repo_list.push_back(std::move(repo));
        return m_repo_list.back();
    }

}
