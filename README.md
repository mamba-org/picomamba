Picomamba
==========





Mamba in the browser can be used in conjunction with python compiled to wasm from `emscripten-forge` 
The usage looks like:
```python
import picomamba

def callback(name, done, total):
    percent = 100.0 * done / total
    print(f"{name} {percent:.2f}% ({done}/{total})")

pico_mamba = picomamba.PicoMamba(
    env_prefix=env_prefix,           # the name of the env
    repodata_dir=repodata_dir,       # where to store repodata
    arch_root_url=arch_url,          # root url for arch pkgs
    noarch_template=noarch_template, # templated url for norach pkgs
    progress_callback=callback,      # report download progress 
)
await pico_mamba.fetch_repodata(
    arch_url=arch_url,       # url for arch repodata tar.bz2 file
    noarch_url=noarch_url    # url for noarch repodata tar.bz2 file
)
transaction = pico_mamba.solve(
    ["regex", "imageio", "numpy", "networkx"]
)
await pico_mamba.install_transaction(transaction)


import numpy
import regex
import networkx
```
the output would be
```
2022-09-15 11:08:11,235 INFO -- download_repodata
repodata 0.00% (0/13688)
repodata 0.00% (0/3511132)
repodata 0.39% (13688/3511132)
repodata 0.39% (13688/3511132)
repodata 30.25% (1062264/3511132)
repodata 60.12% (2110840/3511132)
repodata 100.00% (3511132/3511132)
repodata 100.00% (3511132/3511132)
2022-09-15 11:08:12,095 INFO -- download_repodata DONE! took 0.86s
2022-09-15 11:08:12,096 INFO -- untar repodata
2022-09-15 11:08:13,648 INFO -- untar repodata DONE! took 1.55s
2022-09-15 11:08:13,649 INFO -- load installed packages
2022-09-15 11:08:13,654 INFO -- load installed packages DONE! took 0.01s
2022-09-15 11:08:13,655 INFO -- solve
2022-09-15 11:08:13,665 INFO -- solve DONE! took 0.01s
2022-09-15 11:08:13,667 INFO -- install transaction
2022-09-15 11:08:13,668 INFO -- install-noarch [networkx=2.8.6=pyhd8ed1ab_0, imageio=2.21.2=pyhfa7a67d_0]
packages 0.00% (0/1629208)
packages 0.00% (0/4973031)
packages 32.76% (1629208/4973031)
packages 32.76% (1629208/4973031)
packages 74.93% (3726360/4973031)
packages 84.16% (4185112/4973031)
packages 100.00% (4973031/4973031)
packages 100.00% (4973031/4973031)
2022-09-15 11:08:13,799 INFO -- install noarch networkx=2.8.6=pyhd8ed1ab_0
2022-09-15 11:08:16,246 INFO -- install noarch networkx=2.8.6=pyhd8ed1ab_0 DONE! took 2.45s
2022-09-15 11:08:16,247 INFO -- install noarch imageio=2.21.2=pyhfa7a67d_0
2022-09-15 11:08:16,854 INFO -- install noarch imageio=2.21.2=pyhfa7a67d_0 DONE! took 0.61s
2022-09-15 11:08:16,855 INFO -- wait for deps
2022-09-15 11:08:16,857 INFO -- install-arch [regex=2022.1.18=py310h672cd09_0, pillow=9.1.0=h8b4d581_0, clapack=3.2.1=h1a65802_0, scipy=1.8.1=py310h7c23efa_0]
```



Warnings
=========
This is all very experimental and far from usable!!!
