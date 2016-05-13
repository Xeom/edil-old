import subprocess
import cProfile

clipcmd = "xclip -selection clipboard -o -"

def do_paste(cursor):
#    editor.uiupdates.hold_buffer(cursor.buffer)

    clip = subprocess.check_output(clipcmd, shell=True)
    clip = clip.decode("ascii")

    split = clip.split("\n")
    cursor.insert(split.pop(0))

    for line in split:
        cursor.enter()
        cursor.insert(line)

 #   editor.uiupdates.release_buffer()
