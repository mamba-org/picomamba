#pragma once
#ifndef PICOMAMBA_CONDA_META_HPP
#define PICOMAMBA_CONDA_META_HPP

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>



namespace picomamba {

    template<class FUNCTOR>
    void for_each_pkg_meta(const std::string & pkg_prefix, FUNCTOR && functor)
    {

        auto conda_meta_path = std::filesystem::path(pkg_prefix) / std::filesystem::path("conda-meta");

        for (auto &p : std::filesystem::recursive_directory_iterator(conda_meta_path))
        {
            if (p.path().extension() == ".json")
            {
                std::ifstream fs(p.path().string());
                nlohmann::json pkg_meta = nlohmann::json::parse(fs);
                functor(pkg_meta);
            }
        }
    }


}

#endif // PICOMAMBA_CONDA_META_HPP