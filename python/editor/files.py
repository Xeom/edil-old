import os
import editor.userlog

def revert(b):
    #TODO: "Are you sure?"

    filename = b.properties["filename"]

    if filename == None:
        return

    if not os.path.exists(filename):
        return

    for index in reversed(range(len(b))):
        del b[index]

    b.read(filename)
    editor.userlog.log(b"Read file " + filename + b" to buffer")

def associate(b, filename):
    curr = b.properties["filename"]

    b.properties["filename"] = filename

    if curr == None:
        revert(b)

def dump(b):
    filename = b.properties["filename"]

    if filename == None:
        return

    b.dump(filename)
    editor.userlog.log(b"Dumped buffer to file " + filename)
