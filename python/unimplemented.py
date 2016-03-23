#!/usr/bin/python
import os
import importlib

sourcedir = "../src/"

sourcedir = os.path.abspath(sourcedir) + "/"

symbolsraw = list(os.popen("nm -l lib.so").readlines())

symbolsbyfile = {}

for s in symbolsraw:
    if len(s.split()) != 4:
        continue

    addr, type, fname, line = s.split()
    file, lineno = line.split(":")

    if type.lower() == type:
        continue

    if file.startswith(sourcedir):
        key = file[len(sourcedir):]
        symbolsbyfile[key] = symbolsbyfile.get(key, []) + [fname]

for file, fnames in symbolsbyfile.items():
    if file.endswith(".c"):
        file = file[:-2]

    else:
        continue

    try:
        module = "c." + file.split("/")[0]
        obj    = importlib.import_module(module)

    except ImportError:
        print("No module " + module)
        continue

    for fname in fnames:
        cls     = []
        pyfname = []

        fileparts = list(file.split("/"))[1:]

        for fnamepart in fname.split("_")[1:]:
            if (len(fileparts) and fileparts[0] == fnamepart):
                cls.append(fileparts.pop(0))
            else:
                pyfname.append(fnamepart)

        pyfname = "_".join(pyfname)
        subobj = obj
        for sub in cls:
            if hasattr(subobj, sub):
                subobj = getattr(subobj, sub)
            else:
                print(module + " missing class " + ".".join(cls))
                break
        else:
            if not hasattr(subobj, pyfname):
                print(module + " missing function " + fname)
            continue
        break
