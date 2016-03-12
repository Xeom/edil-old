#!/usr/bin/python3

import sys
try:
	exec("import " + sys.argv[1] + " as module")
	exit(hasattr(module, "_".join(sys.argv[2].split("_")[1:])))
except Exception:
	exit(-1)

