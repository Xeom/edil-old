#!/usr/bin/python3

import sys
exec("import " + sys.argv[1] + " as module")
exit(hasattr(module, "_".join(sys.argv[2].split("_")[1:])))
