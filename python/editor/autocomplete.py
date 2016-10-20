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
            os.path.join(
                root,
                os.path.expanduser(string)
            )
        )

        if os.path.isdir(curpath):
            partial = ""

        else:
            curpath, partial = os.path.split(curpath)

        subs = os.listdir(curpath)
        prefix = os.path.dirname(string)

        subs = [os.path.join(prefix, sub) for sub in subs \
                if sub.startswith(partial)]

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
