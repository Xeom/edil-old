import core.buffer
import core.windows

buffers = []

@core.buffer.hooks.create(900)
def handle_buffer_create(b):
    buffers.append(b)

@core.buffer.hooks.delete(900)
def handle_buffer_delete(b):
    buffers.remove(b)

def next(b):
    index = buffers.index(b)
    index = (index + 1) % len(buffers)

    return buffers[index]

def prev(b):
    index = buffers.index(b)
    index = (index - 1) % len(buffers)

    return buffers[index]

def nextify_window(w):
    w.buffer = next(w.buffer)

def prevify_window(w):
    w.buffer = prev(w.buffer)

for window in core.windows.get_root():
    buffers.append(window.buffer)
