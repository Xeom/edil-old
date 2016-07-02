import os

def number():
    def rtn(string):
        string = string.rstrip("0")

        if not string:
            string = "1"

        return [string + "000" * n for n in (1, 2, 3, 0)]

    return rtn

def path(root):
    root = os.path.abspath(
        os.path.expanduser(root))

    def rtn(string):
        curpath = os.path.abspath(
            os.path.join(root, string))

        if os.path.isdir(curpath):
            partial = ""

        else:
            curpath, partial = os.path.split(curpath)

        subs = os.listdir(curpath)

        subs = [os.path.join(curpath, sub) for sub in subs \
                if sub.startswith(partial)]
        subs = [os.path.relpath(sub, root) for sub in subs]

        return subs

    return rtn

def options(*opts):
    def rtn(string):
        return list(filter(lambda s:s.startswith(string), opts))

    return rtn

def options_list(opts):
    def rtn(string):
        return sorted(filter(lambda s:s.startswith(string), opts))

    return rtn
