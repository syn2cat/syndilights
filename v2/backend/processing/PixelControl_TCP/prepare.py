from network import receive_config, send_config, TCPConfigure
from data_generator import prepare_data

def check_config(max_height, max_width, max_framerate, height, width, framerate):
    if height <= 0 or height > max_height:
        return False, "height cannot be higher than {}. Current: {}.".format(max_height, height)
    if width <= 0 or width > max_width:
        return False, "width cannot be higher than {}. Current: {}.".format(max_width, width)
    if framerate <= 0 or framerate > max_framerate:
        return False, "framerate cannot be higher than {}. Current: {}.".format(max_framerate, framerate)
    return True, None

def prepare(cl_class, server_ip, server_port, height, width, framerate, brightness):
    # Just to make sure pixels[] is initialized.
    if server_ip == 'dummy':
        size(width, height)
        loadPixels()
        return None, None
    ledTCP = TCPConfigure(cl_class, server_ip, server_port)
    max_height, max_width, max_framerate = receive_config(ledTCP)
    good, reason = check_config(max_height, max_width, max_framerate, height, width, framerate)
    if not good:
        raise Exception(reason)
    send_config(ledTCP, height, width, framerate)
    size(width, height)
    loadPixels()
    dimension = width * height
    data = prepare_data(dimension, brightness)
    return ledTCP, data
