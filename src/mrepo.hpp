#ifndef MAMBA_REPO_HPP
#define MAMBA_REPO_HPP

#include "package_info.hpp"
#include "prefix_data.hpp"

#include <filesystem>
namespace fs = std::filesystem;

namespace mamba_wasm
{
    /**
     * Represents a channel subdirectory
     * index.
     */
    struct RepoMetadata
    {
        std::string url;
        bool pip_added;
        std::string etag;
        std::string mod;
    };

    /**
     * A wrapper class of libsolv Repo.
     * Represents a channel subdirectory and
     * is built using a ready-to-use index/metadata
     * file (see ``MSubdirData``).
     */
    class MRepo
    {
    public:
        ~MRepo();

        MRepo(const MRepo&) = delete;
        MRepo& operator=(const MRepo&) = delete;

        MRepo(MRepo&&);
        MRepo& operator=(MRepo&&);

        void set_installed();
        void set_priority(int priority, int subpriority);
        void add_package_info(Repodata*, const PackageInfo& pkg_info);
        void add_pip_as_python_dependency();

        const fs::path& index_file();

        Id id() const;
        std::string name() const;
        bool write() const;
        const std::string& url() const;
        Repo* repo() const;
        std::tuple<int, int> priority() const;
        std::size_t size() const;

        bool clear(bool reuse_ids);

        /**
         * Static constructor.
         * @param pool ``libsolv`` pool wrapper
         * @param name Name of the subdirectory (<channel>/<subdir>)
         * @param filename Name of the index file
         * @param url Subdirectory URL
         */
        static MRepo& create(MPool& pool,
                             const std::string& name,
                             const std::string& filename,
                             const std::string& url);

        /**
         * Static constructor.
         * @param pool ``libsolv`` pool wrapper
         * @param name Name of the subdirectory (<channel>/<subdir>)
         * @param index Path to the index file
         * @param meta Metadata of the repo
         * @param channel Channel of the repo
         */
        static MRepo& create(MPool& pool,
                             const std::string& name,
                             const fs::path& filename,
                             const RepoMetadata& meta);

        /**
         * Static constructor.
         * @param pool ``libsolv`` pool wrapper
         * @param prefix_data prefix data
         */
        static MRepo& create(MPool& pool, const PrefixData& prefix_data);

        /**
         * Static constructor.
         * @param pool ``libsolv`` pool wrapper
         * @param name Name
         * @param uris Matchspecs pointing to unique resources (URL or files)
         */
        static MRepo& create(MPool& pool,
                             const std::string& name,
                             const std::vector<PackageInfo>& uris);

    private:
        MRepo(MPool& pool,
              const std::string& name,
              const std::string& filename,
              const std::string& url);

        MRepo(MPool& pool,
              const std::string& name,
              const fs::path& filename,
              const RepoMetadata& meta);

        MRepo(MPool& pool, const PrefixData& prefix_data);

        MRepo(MPool& pool, const std::string& name, const std::vector<PackageInfo>& uris);

        bool read_file(const fs::path& filename);

        fs::path m_json_file, m_solv_file;
        std::string m_url;

        RepoMetadata m_metadata;

        Repo* m_repo;
    };
}

#endif  // MAMBA_REPO_HPP
