# uncompyle6 version 3.2.4
# Python bytecode 2.7 (62211)
# Decompiled from: Python 3.6.6 | packaged by conda-forge | (default, Jul 26 2018, 11:48:23) [MSC v.1900 64 bit (AMD64)]
# Embedded file name: ./server_tester.py
# Compiled at: 2018-12-09 20:14:32
from __future__ import print_function
import subprocess as sp, string, random, time, os
DEBUG = False
POINTS = 0.0
TOTAL_POINTS = 10.0
PROG_NAME = './server'
PASSED = 'Test PASSED: {} [{}/{} pts]'
FAILED = 'Test FAILED: {} [{}/{} pts]'
ECHO_ADDRESS = ('localhost', 44445)
HEX_ADDRESS = ('localhost', 44446)
TESTS = [
 {'name': 'connecting to echo server', 'pts': 1, 
    'timeout': 5, 
    'type': 'connect'},
 {'name': 'echo 1', 'pts': 1, 
    'input': 'Hallo Welt! Eeeeeeeeechhhhhooooooooooooo!!!!\n', 
    'timeout': 2, 
    'type': 'text'},
 {'name': 'echo 2', 'pts': 1, 
    'input': 'Testing echo server. This is just any text.\n', 
    'timeout': 2, 
    'type': 'text'},
 {'name': 'random echo 1', 'pts': 1, 
    'timeout': 2, 
    'type': 'rand'},
 {'name': 'multiple parallel connections', 'pts': 1, 
    'timeout': 2, 
    'type': 'mult'},
 {'name': 'close echo connection after 5 seconds', 'pts': 1, 
    'timeout': 8, 
    'type': 'close'},
 {'name': 'connecting to hexdump server', 'pts': 1, 
    'timeout': 5, 
    'type': 'connect'},
 {'name': 'hexdump 1', 'pts': 1, 
    'input': 'Hallo Welt!\n', 
    'hex': '000000 : 48 61 6c 6c 6f 20 57 65 6c 74 21 0a               Hallo Welt!.\n', 
    'timeout': 2, 
    'type': 'text'},
 {'name': 'hexdump 2', 'pts': 1, 
    'input': 'GBS testing hexdump\n', 
    'hex': '000000 : 47 42 53 20 74 65 73 74 69 6e 67 20 68 65 78 64   GBS testing hexd\n' + '000010 : 75 6d 70 0a                                       ump.\n', 
    'timeout': 2, 
    'type': 'text'},
 {'name': 'close hexdump connection after 5 seconds', 'pts': 1, 
    'timeout': 8, 
    'type': 'close'}]

def build_cmd(test):
    cmd = 'timeout ' + str(test['timeout']) + ' ' + PROG_NAME
    if DEBUG:
        print('cmd: ' + cmd)
    return cmd


def build_nc_cmd(test, port):
    cmd = 'timeout ' + str(test['timeout']) + ' nc localhost ' + str(port)
    if DEBUG:
        print('cmd: ' + cmd)
    return cmd


def exec_test():
    with open(os.devnull, 'w') as (devnull):
        test_echo = test_hex = True
        if not wait_for_port(ECHO_ADDRESS[1], devnull):
            print('CRITICAL: Aborting tests for echo server!')
            test_echo = False
        if not wait_for_port(HEX_ADDRESS[1], devnull):
            print('CRITICAL: Aborting tests for hex server!')
            test_hex = False
        print('# starting server')
        server = sp.Popen(build_cmd({'timeout': 45}), stdout=devnull, shell=True)
        if server.poll() is None:
            if server.returncode == 127:
                print(PROG_NAME + ' not found')
                return
        time.sleep(0.5)
        if test_echo:
            for test in TESTS[0:6]:
                if echo_test(test) is False:
                    break

        print('-----')
        if test_hex:
            for test in TESTS[6:]:
                if hex_test(test) is False:
                    break

        server.wait()
    return


def wait_for_port(port, devnull):
    count = 0
    print('# waiting for port ' + str(port) + ' to be available...')
    while count < 300:
        try:
            sp.check_call('fuser ' + str(port) + '/tcp', shell=True, stdout=devnull, stderr=devnull)
        except sp.CalledProcessError as e:
            return True

        time.sleep(1)
        count += 1

    print("[300 sec]port still bound to... Server won't be able to bind to it")
    return False


def echo_test(test):
    nc = sp.Popen(build_nc_cmd(test, 44445), stdin=sp.PIPE, stdout=sp.PIPE, stderr=sp.PIPE, shell=True, cwd=os.path.dirname(os.path.realpath(__file__)))
    time.sleep(0.5)
    if test['type'] == 'connect':
        if nc.poll() is not None:
            print('Could not connect to echo server:')
            print(nc.stderr.readline())
            fail_test(test)
            print('Skipping remaining tests for echo server...')
            return False
        nc.stdin.close()
    else:
        if test['type'] == 'close':
            time.sleep(5)
            if nc.poll() is None:
                print('Connection still up after 5 seconds')
                fail_test(test)
                return
        else:
            if test['type'] == 'mult':
                n2 = sp.Popen(build_nc_cmd(test, 44445), stdin=sp.PIPE, stdout=sp.PIPE, stderr=sp.PIPE, shell=True, cwd=os.path.dirname(os.path.realpath(__file__)))
                if not (nc.poll() is None and n2.poll() is None):
                    print('Connections are not both running at the same time')
                    fail_test(test)
                    return
            else:
                if test['type'] == 'rand':
                    echo_input = ''
                    for _ in range(255):
                        echo_input += random.choice(string.ascii_letters + string.digits + string.punctuation)

                    echo_input += '\n'
                else:
                    if test['type'] == 'text':
                        echo_input = test['input']
                echo_ret, err_output = nc.communicate(echo_input)
                if echo_input != echo_ret:
                    print('server returned wrong echo')
                    print('Expected:')
                    print(echo_input)
                    print('Got:')
                    print(echo_ret)
                    fail_test(test)
                    return
            if DEBUG:
                print('stdout:')
                print(echo_ret)
    pass_test(test)
    return


def hex_test(test):
    nc = sp.Popen(build_nc_cmd(test, 44446), stdin=sp.PIPE, stdout=sp.PIPE, stderr=sp.PIPE, shell=True, cwd=os.path.dirname(os.path.realpath(__file__)))
    time.sleep(0.5)
    if test['type'] == 'connect':
        if nc.poll() is not None:
            print('Could not connect to hexdump server:')
            print(nc.stderr.readline())
            fail_test(test)
            print('Skipping remaining tests for hexdump server...')
            return False
        nc.stdin.close()
    else:
        if test['type'] == 'close':
            time.sleep(5)
            if nc.poll() is None:
                print('Connection still up after 5 seconds')
                fail_test(test)
                return
        else:
            if test['type'] == 'text':
                echo_input = test['input']
                echo_ret, err_output = nc.communicate(echo_input)
                if echo_ret != test['hex']:
                    print('server returned wrong hexdump')
                    print('Expected:')
                    print(test['hex'])
                    print('Got:')
                    print(echo_ret)
                    fail_test(test)
                    return
                if DEBUG:
                    print('stdout:')
                    print(echo_ret)
    pass_test(test)
    return


def fail_test(test):
    print(FAILED.format(test['name'], 0, test['pts']))


def pass_test(test):
    global POINTS
    POINTS += test['pts']
    print(PASSED.format(test['name'], test['pts'], test['pts']))


def main():
    print(os.path.basename(os.path.realpath(__file__)) + ':')
    try:
        exec_test()
    except Exception as e:
        print(e)
    finally:
        print(('Points received: {0}/{1}').format(POINTS, TOTAL_POINTS))


if __name__ == '__main__':
    main()
# okay decompiling server_tester.pyc
