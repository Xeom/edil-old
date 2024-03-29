import os
import core.err
import editor.buffers.userlog
import editor.query
import time

def revert(b, override=False):
    filename = b.properties["filename"]
    modified = b.properties["modified"]

    if filename == None:
        core.err.new(
            title="No associated file",
            details=("Associate a file with this buffer with the file-associate"
                     " command before trying to dump its contents.")
        )
        return


    filename = filename.decode("ascii")

    if not os.path.exists(filename):
        core.err.new(
            title="File does not exist",
            details=("You cannot revert a buffer to the contents of a file that"
                     " does not exist.")
        )
        return

    for index in reversed(range(len(b))):
        del b[index]

    def rlydoit():
        b.read(filename)

        b.properties["modified"]    = "False"
        b.properties["revert-time"] = str(int(time.time()))

        editor.buffers.userlog.log("Read file " + filename + " to buffer")

    if override:
        rlydoit()

    else:
        if modified and modified.lower() == "true":
            editor.query.confirm(rlydoit, ("Buffer has been modified. "
                                           "Really revert buffer?"))
        else:
            editor.query.confirm(rlydoit, "Really revert buffer?")

def associate(b, filename):
    oldfilename = b.properties["filename"]

    def rlydoit():
        b.properties["filename"] = filename
        editor.buffers.userlog.log("Associated buffer with file " + filename)
        revert(b)

    editor.query.confirm(rlydoit, "Really associate with " + filename + "?")

def dumpnew(b):
    filename = b.properties["filename"]

    if filename == None:
        core.err.new(
            title="No associated file",
            details=("Associate a file with this buffer with the file-associate"
                     " command before trying to dump its contents.")
        )
        return

    filename = filename.decode("ascii")

    def rlydoit():
        b.dump(filename)

        b.properties["dump-time"] = str(int(time.time()))
        b.properties["modified"]  = "False"

        editor.buffers.userlog.log("Dumped buffer to new file " + filename)

    editor.query.confirm(rlydoit, "Really dump to new file " + filename + "?")

def dump(b):
    filename = b.properties["filename"]

    if filename == None:
        core.err.new(
            title="No associated file",
            details=("Associate a file with this buffer with the file-associate"
                     " command before trying to dump its contents.")
        )
        return

    filename = filename.decode("ascii")

    if not os.path.exists(filename):
        dumpnew(b)
        return

    revert_time = b.properties["revert-time"]
    revert_time = float(revert_time) if revert_time else time.time()
    dump_time   = b.properties["dump-time"]
    dump_time = float(dump_time) if dump_time else None

    stats       = os.stat(filename)

    def rlydoit():
        b.dump(filename)

        b.properties["dump-time"] = str(int(time.time()))
        b.properties["modified"]  = "False"

        editor.buffers.userlog.log("Dumped buffer to file " + filename)

    if revert_time < stats.st_mtime and (dump_time == None or dump_time < stats.st_mtime):
        editor.query.confirm(rlydoit, filename +
                             (" has been changed since you "
                              "began editing it. Really dump to it?"))

    else:
        editor.query.confirm(rlydoit, "Really dump to " + filename + "?")

