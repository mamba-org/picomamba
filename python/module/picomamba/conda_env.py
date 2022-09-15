from pathlib import Path
import json


def ensure_is_conda_env(env_prefix):
    if not (Path(env_prefix) / "conda-meta").is_dir():
        raise RuntimeError(f"{env_prefix} is not a conda environment")


def create_conda_meta(package, env_prefix):
    pkg_conda_meta = dict(
        name=package.name,
        version=package.version,
        build=package.build,
        build_number=package.build_number,
    )

    with open(env_prefix / "conda-meta" / package.filename(extension="json"), "w") as f:
        json.dump(pkg_conda_meta, f)
