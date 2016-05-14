import subprocess
import cProfile
import core.ui

clipcmd = "xclip -selection clipboard -o -"

def do_paste(cursor):
    core.ui.updates_hold(cursor.buffer)

    clip = subprocess.check_output(clipcmd, shell=True)
    clip = clip.decode("ascii")

    split = clip.split("\n")
    cursor.insert(split.pop(0))

    for line in split:
        cursor.enter()
        cursor.insert(line)

    core.ui.updates_release()
