Picomamba
==========





Mamba in the browser can be used in conjunction with python compiled to wasm from `emscripten-forge` 
The usage looks like:
```python
    def callback(name, done, total):
        percent = 100.0 * done / total
        print(f"{name} {percent:.2f}% ({done}/{total})")

    pico_mamba = PicoMamba(
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

Warnings
=========
This is all very experimental and far from usable!!!