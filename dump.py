from subprocess import check_output


def get_pid(name='vendscreen.com'):
    """
    Get the PID of a given process name via adb shell and ps

    name - Name of process

    Returns: PID of process

    """

    ps_list = check_output(['adb', 'shell', 'ps']).splitlines()

    for process in ps_list:
        if name in process:
            return process.split()[0]


def get_map(pid):
    """
    Get the memory map for a given PID as a list

    Returns: List of tuples: (start_address, end_address, pathname)
    """

    map_list = check_output(['adb', 'shell', 'cat', '/proc/' + pid + '/maps']).splitlines()

    splitted_list = [map.split() for map in map_list]
    
    final_list = []

    for map in splitted_list:
        start_addr = map[0].split('-')[0]
        end_addr = map[0].split('-')[1]

        if len(map) == 6:
            path = map[5]
        else:
            path = "(anonymous)"

        final_list.append((start_addr, end_addr, path))
        
    return final_list


def dump_to_file(pid, start, end, file="memdump"):
    """
    Dump a memory range of a PID to a file, using the dump_range executable on the device

    Pulls the dumpfile to the local machine

    Deletes the dumpfile on the device

    """

    print check_output(['adb', 'shell', '/data/dump_range', pid, start, end, '/data/' + file])

    print check_output(['adb', 'pull', '/data/' + file])

    print check_output(['adb', 'shell', 'rm', '/data/' + file])


if __name__ == "__main__":
    
    pid = get_pid()
    print "Found process: " + pid
    
    for map in get_map(pid):
        print "Dumping: " + str(map)
        dump_to_file(pid, map[0], map[1])


