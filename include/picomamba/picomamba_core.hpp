#pragma once
#ifndef PICOMAMBA_PICOMAMBA_HPP
#define PICOMAMBA_PICOMAMBA_HPP

#include <cstdint>
#include <iostream>


#include <nlohmann/json.hpp>

#include "picomamba/conda_meta.hpp"


extern "C"
{
#include "solv/solver.h"
#include "solv/transaction.h"
#include "solv/solverdebug.h"
#include "solv/repo_conda.h"
#include "solv/conda.h"
#include "solv/repo_solv.h"
#include "solv/pool.h"
#include "solv/repo_write.h"
}

namespace picomamba {


    class PicoMambaCore
    {
    public:

        struct SolveConfig{
            bool silent_errors;
            bool throw_on_errors;
        };

        PicoMambaCore();
        ~PicoMambaCore();
        Pool* get();
        void load_repodata_from_file(const std::string& fname, const std::string& url);

        // make pool aware of already installed packages
        int load_installed(const std::string & pkg_prefix);

        void add_to_installed(const std::string name, const std::string version, const std::string build, const int build_number);

        template<class SPECS_ITER, class FUNCTOR>
        bool solve(SPECS_ITER specs_begin, SPECS_ITER specs_end, const PicoMambaCore::SolveConfig & config,FUNCTOR && );


        Pool* m_pool;
        Repo* m_repo;
        Repodata * m_data_installed;
    };

    PicoMambaCore::PicoMambaCore()
        : m_pool(pool_create()),
          m_repo(nullptr)
    {
    }

    PicoMambaCore::~PicoMambaCore()
    {
        pool_free(m_pool);
    }


    inline void PicoMambaCore::load_repodata_from_file(const std::string& fname, const std::string& name)
    {
        FILE* f = fopen(fname.c_str(), "r");
        Repo* r = repo_create(m_pool, name.c_str());
        repo_add_conda(r, f, 0);
        fclose(f);
    }

    template<class SPECS_ITER, class FUNCTOR>
    bool PicoMambaCore::solve(
        SPECS_ITER specs_begin, 
        SPECS_ITER specs_end, 
        const PicoMambaCore::SolveConfig & config,
        FUNCTOR && functor
    )
    {
        Solver* s = solver_create(m_pool);

        Queue q;
        queue_init(&q);

        for (auto iter=specs_begin; iter!=specs_end; ++iter)
        {
            Id inst_id = pool_conda_matchspec(m_pool, iter->c_str());
            queue_push2(&q, SOLVER_INSTALL | SOLVER_SOLVABLE_PROVIDES, inst_id);
        }

        solver_solve(s, &q);

        const int problem_count = solver_problem_count(s);
        const bool success = problem_count == 0;
        if (!success)
        {
            if(!config.silent_errors)
            {
                Id problem = 0;
                int pcnt;
                while ((problem = solver_next_problem(s, problem)) != 0)
                {
                    solver_printcompleteprobleminfo(s, problem);
                }
            }
            if(!config.throw_on_errors)
            {
                throw std::runtime_error("cannot solve problem");
            }
        }
        else
        {
            Transaction* transaction = solver_create_transaction(s);
            transaction_order(transaction, 0);
            //py::dict res = transaction_to_py(transaction);

            functor(transaction);

            transaction_free(transaction);
        }
        solver_free(s);
        queue_free(&q);
        return success;
    }

    // THIS IS NOT WORKING :/
    // void PicoMambaCore::add_to_installed(const std::string name, const std::string version, const std::string build, const int build_number)
    // {
    //     std::cout<<"add_to_installed name "<<name<<" version "<<version<<" build "<<build<<" build_number "<<build_number<<"\n";
    //     int flags = REPO_REUSE_REPODATA | REPO_NO_INTERNALIZE;
    //     Repodata * data = repo_add_repodata(m_repo, flags);

    //     Id handle = repo_add_solvable(m_repo);
    //     Solvable* s = pool_id2solvable(m_pool, handle);

    //     repodata_set_str(data,           handle, SOLVABLE_BUILDVERSION, std::to_string(build_number).c_str());
    //     repodata_add_poolstr_array(data, handle, SOLVABLE_BUILDFLAVOR,  build.c_str());

    //     s->name = pool_str2id(m_pool, name.c_str(), 1);
    //     s->evr = pool_str2id(m_pool, version.c_str(), 1);
        
    //     s->provides = repo_addid_dep(m_repo, s->provides, pool_rel2id(m_pool, s->name, s->evr, REL_EQ, 1), 0);
    //     repodata_internalize(data);
    //     // pool_set_installed(m_repo->pool, m_repo);
    // }

    int PicoMambaCore::load_installed(const std::string & pkg_prefix)
    {   
        if(m_repo != nullptr)
        {
            repo_free(m_repo, true);
        }
        // create new repo for the installed pkgs
        m_repo = repo_create(m_pool, "installed");
        m_repo->appdata = this;
        int flags = REPO_REUSE_REPODATA;
        Repodata* data = repo_add_repodata(m_repo, flags);
        
        // static Id real_repo_key = pool_str2id(m_pool, "solvable:real_repo_url", 1);
        // static Id noarch_repo_key = pool_str2id(m_pool, "solvable:noarch_type", 1);

        auto n_packages = 0;
        for_each_pkg_meta(pkg_prefix,[&](auto && pkg_meta){
            n_packages += 1;


            // basic info of the pkg
            const auto name = pkg_meta["name"]. template get<std::string>();
            const auto version = pkg_meta["version"]. template get<std::string>();
            const auto build = pkg_meta["build"]. template get<std::string>();
            const auto build_number = pkg_meta["build_number"]. template get<int>();

            // libsolv handle for that pkg instance
            Id handle = repo_add_solvable(m_repo);
            Solvable* s = pool_id2solvable(m_pool, handle);

            repodata_set_str(data,           handle, SOLVABLE_BUILDVERSION, std::to_string(build_number).c_str());
            repodata_add_poolstr_array(data, handle, SOLVABLE_BUILDFLAVOR,  build.c_str());

            s->name = pool_str2id(m_pool, name.c_str(), 1);
            s->evr = pool_str2id(m_pool, version.c_str(), 1);
            
            s->provides = repo_addid_dep(m_repo, s->provides, pool_rel2id(m_pool, s->name, s->evr, REL_EQ, 1), 0);

        });

        repodata_internalize(data);
        pool_set_installed(m_repo->pool, m_repo);
        return n_packages;
    }


} // end namespace picomamba


#endif // PICOMAMBA_PICOMAMBA_HPP