import contextlib
import logging
import time


logger = logging.getLogger("picomamba")


@contextlib.contextmanager
def logged(msg):
    t0 = time.time()
    logger.info(msg)
    yield
    logger.info(f"{msg} DONE! took {time.time()-t0:0.2f}s")
