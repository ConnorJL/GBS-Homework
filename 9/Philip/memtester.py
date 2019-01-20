# uncompyle6 version 3.2.5
# Python bytecode 2.7 (62211)
# Decompiled from: Python 3.6.0 |Continuum Analytics, Inc.| (default, Dec 23 2016, 11:57:41) [MSC v.1900 64 bit (AMD64)]
# Embedded file name: ./mem_tester.py
# Compiled at: 2018-12-16 15:18:25
from __future__ import print_function
import subprocess as sp, os
DEBUG = False
POINTS = 0.0
TOTAL_POINTS = 10.0
PROG_NAME = './mem'
PASSED = 'Test PASSED: {} [{}/{} pts]'
FAILED = 'Test FAILED: {} [{}/{} pts]'
TESTS = [
 {'name': 'First Fit: case 1', 'input': '-1 -a 100 -f 1 -a 200', 
    'output': [
             [
              ('F', 0, 1048576)],
             [
              ('P', 100, 4096), ('F', 0, 1044480)],
             [
              ('F', 0, 1048576)],
             [
              ('P', 200, 4096), ('F', 0, 1044480)]], 
    'timeout': 3, 
    'pts': 1},
 {'name': 'First Fit: case 2', 'input': '-1 -a 520192 -a 4096 -f 1 -a 100 ', 
    'output': [
             [
              ('F', 0, 1048576)],
             [
              ('P', 520192, 520192), ('F', 0, 528384)],
             [
              ('P', 520192, 520192), ('P', 4096, 4096),
              ('F', 0, 524288)],
             [
              ('F', 0, 520192), ('P', 4096, 4096), ('F', 0, 524288)],
             [
              ('P', 100, 4096), ('F', 0, 516096), ('P', 4096, 4096),
              ('F', 0, 524288)]], 
    'timeout': 3, 
    'pts': 1},
 {'name': 'First Fit: case 3', 'input': '-1 -a 4096 -a 4096 -a 8192 -a 4096 -f 1 -f 3 -a 8192', 
    'output': [
             [
              ('F', 0, 1048576)],
             [
              ('P', 4096, 4096), ('F', 0, 1044480)],
             [
              ('P', 4096, 4096), ('P', 4096, 4096), ('F', 0, 1040384)],
             [
              ('P', 4096, 4096), ('P', 4096, 4096), ('P', 8192, 8192),
              ('F', 0, 1032192)],
             [
              ('P', 4096, 4096), ('P', 4096, 4096), ('P', 8192, 8192),
              ('P', 4096, 4096), ('F', 0, 1028096)],
             [
              ('F', 0, 4096), ('P', 4096, 4096), ('P', 8192, 8192),
              ('P', 4096, 4096), ('F', 0, 1028096)],
             [
              ('F', 0, 4096), ('P', 4096, 4096), ('F', 0, 8192),
              ('P', 4096, 4096), ('F', 0, 1028096)],
             [
              ('F', 0, 4096), ('P', 4096, 4096), ('P', 8192, 8192),
              ('P', 4096, 4096), ('F', 0, 1028096)]], 
    'timeout': 3, 
    'pts': 1},
 {'name': 'Worst Fit: case 1', 'input': '-w -a 100 -a 200 -f 1 -a 300', 
    'output': [
             [
              ('F', 0, 1048576)],
             [
              ('P', 100, 4096), ('F', 0, 1044480)],
             [
              ('P', 100, 4096), ('P', 200, 4096), ('F', 0, 1040384)],
             [
              ('F', 0, 4096), ('P', 200, 4096), ('F', 0, 1040384)],
             [
              ('F', 0, 4096), ('P', 200, 4096), ('P', 300, 4096),
              ('F', 0, 1036288)]], 
    'timeout': 3, 
    'pts': 1},
 {'name': 'Worst Fit: case 2', 'input': '-w -a 520192 -a 4096 -f 1 -a 100 ', 
    'output': [
             [
              ('F', 0, 1048576)],
             [
              ('P', 520192, 520192), ('F', 0, 528384)],
             [
              ('P', 520192, 520192), ('P', 4096, 4096),
              ('F', 0, 524288)],
             [
              ('F', 0, 520192), ('P', 4096, 4096), ('F', 0, 524288)],
             [
              ('F', 0, 520192), ('P', 4096, 4096), ('P', 100, 4096),
              ('F', 0, 520192)]], 
    'timeout': 3, 
    'pts': 1},
 {'name': 'Worst Fit: case 3', 'input': '-w -a 4096 -a 4096 -a 8192 -a 4096 -f 1 -f 3 -a 100', 
    'output': [
             [
              ('F', 0, 1048576)],
             [
              ('P', 4096, 4096), ('F', 0, 1044480)],
             [
              ('P', 4096, 4096), ('P', 4096, 4096), ('F', 0, 1040384)],
             [
              ('P', 4096, 4096), ('P', 4096, 4096), ('P', 8192, 8192),
              ('F', 0, 1032192)],
             [
              ('P', 4096, 4096), ('P', 4096, 4096), ('P', 8192, 8192),
              ('P', 4096, 4096), ('F', 0, 1028096)],
             [
              ('F', 0, 4096), ('P', 4096, 4096), ('P', 8192, 8192),
              ('P', 4096, 4096), ('F', 0, 1028096)],
             [
              ('F', 0, 4096), ('P', 4096, 4096), ('F', 0, 8192),
              ('P', 4096, 4096), ('F', 0, 1028096)],
             [
              ('F', 0, 4096), ('P', 4096, 4096), ('F', 0, 8192),
              ('P', 4096, 4096), ('P', 100, 4096), ('F', 0, 1024000)]], 
    'timeout': 3, 
    'pts': 1},
 {'name': 'Merge free memory blocks', 'input': '-1 -a 100 -a 200 -a 300 -f 3 -f 1 -f 2', 
    'output': [
             [
              ('F', 0, 1048576)],
             [
              ('P', 100, 4096), ('F', 0, 1044480)],
             [
              ('P', 100, 4096), ('P', 200, 4096), ('F', 0, 1040384)],
             [
              ('P', 100, 4096), ('P', 200, 4096), ('P', 300, 4096),
              ('F', 0, 1036288)],
             [
              ('P', 100, 4096), ('P', 200, 4096), ('F', 0, 1040384)],
             [
              ('F', 0, 4096), ('P', 200, 4096), ('F', 0, 1040384)],
             [
              ('F', 0, 1048576)]], 
    'timeout': 3, 
    'pts': 1},
 {'name': 'Block Size', 'input': '-1 -b 128 -a 100 -a 200 -f 1 -f 2', 
    'output': [
             [
              ('F', 0, 1048576)],
             [
              ('P', 100, 128), ('F', 0, 1048448)],
             [
              ('P', 100, 128), ('P', 200, 256), ('F', 0, 1048192)],
             [
              ('F', 0, 128), ('P', 200, 256), ('F', 0, 1048192)],
             [
              ('F', 0, 1048576)]], 
    'timeout': 3, 
    'pts': 1},
 {'name': 'Total Memory', 'input': '-1 -m 65536 -a 100 -a 200 -f 1 -f 2', 
    'output': [
             [
              ('F', 0, 65536)],
             [
              ('P', 100, 4096), ('F', 0, 61440)],
             [
              ('P', 100, 4096), ('P', 200, 4096), ('F', 0, 57344)],
             [
              ('F', 0, 4096), ('P', 200, 4096), ('F', 0, 57344)],
             [
              ('F', 0, 65536)]], 
    'timeout': 3, 
    'pts': 1},
 {'name': 'Block Size and Total Memory', 'input': '-1 -m 65536 -b 16384 -a 100 -a 200 -a 300', 
    'output': [
             [
              ('F', 0, 65536)],
             [
              ('P', 100, 16384), ('F', 0, 49152)],
             [
              ('P', 100, 16384), ('P', 200, 16384), ('F', 0, 32768)],
             [
              ('P', 100, 16384), ('P', 200, 16384), ('P', 300, 16384),
              ('F', 0, 16384)]], 
    'timeout': 3, 
    'pts': 1}]

def build_cmd(test):
    cmd = ''
    cmd += 'timeout ' + str(test['timeout']) + ' ' + PROG_NAME
    cmd += ' ' + test['input']
    if DEBUG:
        print('cmd: ' + cmd)
    return cmd


def parse_output(output):
    try:
        parsed = []
        for line in output.splitlines():
            split = line.split('[')
            split = list(map(lambda s: s.split(']')[0], split))[1:]
            line_parse = []
            for item in split:
                line_split = item.split(' ')
                status = line_split[1]
                usage = line_split[2].split('/')
                used = int(usage[0])
                free = int(usage[1])
                line_parse.append((status, used, free))

            parsed.append(line_parse)

        return parsed
    except:
        return

    return


def exec_test(test):
    prog = sp.Popen(build_cmd(test), stdin=sp.PIPE, stdout=sp.PIPE, stderr=sp.PIPE, shell=True, cwd=os.path.dirname(os.path.realpath(__file__)))
    output, err_output = prog.communicate()
    if prog.returncode == 124:
        print('Program timed out, did not termitate in time')
        fail_test(test)
        return
    if prog.returncode == 127:
        print(PROG_NAME + ' not found')
        fail_test(test)
        return
    if DEBUG:
        print('stdout:')
        print(output)
    if err_output:
        print('stderr:')
        print(err_output)
        fail_test(test)
        return
    parsed = parse_output(output)
    if parsed is None:
        print('Could not parse output. Please ensure you use the correct format: "[%p %c %d/%d] ".')
        print('Got:')
        print(output)
        fail_test(test)
        return
    if len(test['output']) != len(parsed):
        print('wrong number of lines. NOTE: Please print the memory after each allocate/free. Please print each step in one line.')
        print('All arguments: ' + test['input'])
        fail_test(test)
        return
    for i in range(len(test['output'])):
        if test['output'][i] != parsed[i]:
            print('Mismatch in line ' + str(i + 1) + ':')
            print('All arguments: ' + test['input'])
            print('Expected:')
            print(test['output'][i])
            print('Got:')
            print(parsed[i])
            fail_test(test)
            return

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
    for test in TESTS:
        try:
            exec_test(test)
        except Exception as e:
            print('Unknown Exception')
            fail_test(test)
            raise e

    print(('Points received: {0}/{1}').format(POINTS, TOTAL_POINTS))


if __name__ == '__main__':
    main()
# okay decompiling mem_tester.pyc
