from picomamba import PicoMamba
import logging
import json
from pathlib import Path
import pyjs
import shutil
import sysconfig
from itertools import chain


_async_done_ = [False]

import asyncio


async def main_runner():
    try:
        await main()

    except Exception as e:
        print(e)
    finally:
        global _async_done_
        _async_done_[0] = True


async def main():

    import picomamba
    import logging

    logging.basicConfig(
        level=logging.INFO, format="%(asctime)s %(levelname)s -- %(message)s"
    )

    version = "@0.3.0"
    version = ""  # empty string means master
    cdn_base = f"https://cdn.jsdelivr.net/gh/DerThorsten/distribution{version}"
    cdn_base = "http://127.0.0.1:8000/distribution"

    arch_url = f"{cdn_base}/repodata/arch_repodata.tar.bz2"
    noarch_url = (
        f"http://127.0.0.1:8000/distribution/repodata/slim_noarch_repodata.tar.bz2"
    )

    cors_proxy = "https://cors-anywhere.herokuapp.com/"
    cors_proxy = "https://proxy.cors.sh/"
    # cors_proxy = ""

    noarch_template = cdn_base + "/noarch/{name}-{version}-{build}.tar.bz2"

    env_prefix = "/home/runner/env"
    repodata_dir = os.path.join(env_prefix, "repodata")

    def callback(name, done, total):
        percent = 100.0 * done / total
        print(f"{name} {percent:.2f}% ({done}/{total})")

    pico_mamba = PicoMamba(
        env_prefix=env_prefix,
        repodata_dir=repodata_dir,
        arch_root_url=f"https://cdn.jsdelivr.net/gh/DerThorsten/distribution{version}",
        noarch_template=noarch_template,
        progress_callback=callback,
    )
    await pico_mamba.fetch_repodata(arch_url=arch_url, noarch_url=noarch_url)
    transaction = pico_mamba.solve(
        ["regex", "imageio", "numpy", "python", "scipy", "networkx"]
    )
    await pico_mamba.install_transaction(transaction)


asyncio.ensure_future(main_runner())
