from .pkg_spec import PkgSpec


class Transaction(object):
    def __init__(self, raw_transaction):
        raw_install = raw_transaction["install"]
        self.install = {k: [] for k in ["arch", "noarch", "installed", "mocked"]}
        for name, version, build, build_number, repo_name in raw_install:
            self.install[repo_name].append(
                PkgSpec(
                    name=name, version=version, build=build, build_number=build_number
                )
            )
