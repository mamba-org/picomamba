import tempfile
import tarfile
import contextlib
from pathlib import Path


@contextlib.contextmanager
def untar_to_temporary(tar):
    with tempfile.TemporaryDirectory() as temp_dir:
        temp_dir_path = Path(str(temp_dir))
        tar.extractall(path=str(temp_dir))
        yield temp_dir_path


def untar_repodata(repodata_tar, path):
    path = Path(path)
    path.mkdir(parents=True, exist_ok=True)
    members = repodata_tar.getmembers()
    if len(members) != 1:
        raise RuntimeError(
            f"there must be precisely 1 file in the repodata tar.bz2 file, not {len(members)}"
        )
    if not members[0].name.endswith(".json"):
        raise RuntimeError(
            f"there must be a json file in the the repodata tar.bz2 file not: {members[0]}"
        )

    repodata_tar.extractall(path=path)

    return path / members[0].name
