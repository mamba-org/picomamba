#include <iostream>

extern "C" {
	#include "solv/solver.h"
	#include "solv/transaction.h"
	#include "solv/repo_conda.h"
	#include "solv/conda.h"
	#include "solv/repo_solv.h"
	#include "solv/pool.h"
}


void transaction_to_txt(Transaction* transaction)
{
	Pool* pool = transaction->pool;
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
            	std::cout << "Removing " << pool_solvable2str(pool, s) << std::endl;
				s2 = transaction->pool->solvables + transaction_obs_pkg(transaction, p);
				s2 = pool_id2solvable(pool, i2);
				std::cout << "Installing " << pool_solvable2str(pool, s2) << std::endl;
                break;
            }
            case SOLVER_TRANSACTION_ERASE:
            {
            	std::cout << "Removing " << pool_solvable2str(pool, s) << std::endl;
                break;
            }
            case SOLVER_TRANSACTION_INSTALL:
            {
				std::cout << "Installing " << pool_solvable2str(pool, s) << std::endl;
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


// wget conda.anaconda.org/conda-forge/osx-arm64/repodata.json -O repodata.json
// wget conda.anaconda.org/conda-forge/noarch/repodata.json -O noarch_repodata.json

int main(int argc, char** argv)
{
	Pool* p = pool_create();
	FILE* o = fopen("repodata.json", "r");

	Repo* r = repo_create(p, "repo/emscripten-32");

	repo_add_conda(r, o, 0);

	FILE* o2 = fopen("noarch_repodata.json", "r");
	Repo* r2 = repo_create(p, "repo/noarch");
	repo_add_conda(r2, o2, 0);


	Solver* s = solver_create(p);

	Queue q;
	queue_init(&q);

	for (int i = 1; i < argc; ++i)
	{
	    Id inst_id = pool_conda_matchspec(p, argv[i]);
		queue_push2(&q, SOLVER_INSTALL | SOLVER_SOLVABLE_PROVIDES, inst_id);
	}

    solver_solve(s, &q);

    bool success = solver_problem_count(s) == 0;
    Transaction* trans = solver_create_transaction(s);

    transaction_order(trans, 0);

    transaction_to_txt(trans);
}