# uncompyle6 version 3.2.4
# Python bytecode 2.7 (62211)
# Decompiled from: Python 2.7.13 (default, Sep 26 2018, 18:42:22) 
# [GCC 6.3.0 20170516]
# Embedded file name: ./mysh_tester.py
# Compiled at: 2019-01-13 17:36:58
from __future__ import print_function
import subprocess as sp, time, sys, os
DEBUG = False
POINTS = 0.0
TOTAL_POINTS = 10.0
PROG_NAME = './mysh'
PASSED = 'Test PASSED: {} [{}/{} pts]'
FAILED = 'Test FAILED: {} [{}/{} pts]'
CWD = os.path.dirname(os.path.realpath(__file__))
USAGE = 'Usage: cd <dir>'
TESTS = [
 {'name': 'cwd in prompt', 'timeout': 2, 
    'input': 'exit\n', 
    'output': [
             CWD + ' $ '], 
    'pts': 1},
 {'name': 'change cwd with cd', 'timeout': 2, 
    'input': 'cd abcxyz314\nexit\n', 
    'output': [
             CWD + ' $ ' + CWD + '/abcxyz314 $ '], 
    'pts': 0.5},
 {'name': 'prints usage', 'timeout': 2, 
    'input': 'cd abc uvw\nexit\n', 
    'output': [
             CWD + ' $ ' + USAGE + '\n', CWD + ' $ '], 
    'pts': 0.5},
 {'name': 'without parameter', 'timeout': 2, 
    'input': 'cd\nexit\n', 
    'output': [
             CWD + ' $ ' + CWD + '\n', CWD + ' $ '], 
    'pts': 0.5},
 {'name': 'writes to $PWD', 'timeout': 2, 
    'input': 'cd abcxyz314\necho $PWD\nexit\n', 
    'output': [
             CWD + ' $ ' + CWD + '/abcxyz314 $ ' + CWD + '/abcxyz314\n',
             CWD + '/abcxyz314 $ '], 
    'pts': 0.5},
 {'name': 'wildcard at beginning of file', 'timeout': 2, 
    'input': 'ls -1 *.wild314\nexit\n', 
    'output': [
             CWD + ' $ abc.wild314\n',
             'def.wild314\n',
             'ghi.wild314\n',
             CWD + ' $ '], 
    'pts': 1},
 {'name': 'wildcard at end of file', 'timeout': 2, 
    'input': 'ls -1 abc.*\nexit\n', 
    'output': [
             CWD + ' $ abc.wild314\n',
             'abc.wild315\n',
             'abc.wild316\n',
             CWD + ' $ '], 
    'pts': 1},
 {'name': 'wildcard as directory', 'timeout': 2, 
    'input': 'ls -1 */thisIsAFile\nexit\n', 
    'output': [
             CWD + ' $ abcxyz314/thisIsAFile\n',
             'abcxyz315/thisIsAFile\n',
             CWD + ' $ '], 
    'pts': 1},
 {'name': 'wildcard in directory', 'timeout': 2, 
    'input': 'ls -1 abcxyz314/t*\nexit\n', 
    'output': [
             CWD + ' $ abcxyz314/thisIsAFile\n',
             'abcxyz314/thisIsAnotherFile\n',
             CWD + ' $ '], 
    'pts': 1},
 {'name': 'wildcard in the middle', 'timeout': 2, 
    'input': 'ls -1 abc*.middle\nexit\n', 
    'output': [
             CWD + ' $ abc123.middle\n',
             'abc456.middle\n',
             'abc789.middle\n',
             CWD + ' $ '], 
    'pts': 1},
 {'name': 'wildcard in the middle 2', 'timeout': 2, 
    'input': 'ls -1 middle*.abc\nexit\n', 
    'output': [
             CWD + ' $ middle123.abc\n',
             'middle456.abc\n',
             'middle789.abc\n',
             CWD + ' $ '], 
    'pts': 1},
 {'name': 'wildcard as file', 'timeout': 2, 
    'cdcmd': 'cd abcxyz314;', 
    'input': 'ls -1 *\nexit\n', 
    'output': [
             CWD + '/abcxyz314 $ thisIsAFile\n',
             'thisIsAnotherFile\n',
             CWD + '/abcxyz314 $ '], 
    'pts': 1}]

def prep():
    cmd = 'mkdir abcxyz314;'
    cmd += 'touch abcxyz314/thisIsAFile;'
    cmd += 'touch abcxyz314/thisIsAnotherFile;'
    cmd += 'mkdir abcxyz315;'
    cmd += 'touch abcxyz315/thisIsAFile;'
    cmd += 'touch abc.wild314;'
    cmd += 'touch def.wild314;'
    cmd += 'touch ghi.wild314;'
    cmd += 'touch abc.wild315;'
    cmd += 'touch abc.wild316;'
    cmd += 'touch abc123.middle;'
    cmd += 'touch abc456.middle;'
    cmd += 'touch abc789.middle;'
    cmd += 'touch middle123.abc;'
    cmd += 'touch middle456.abc;'
    cmd += 'touch middle789.abc;'
    try:
        sp.check_call(cmd, stdin=sp.PIPE, stdout=sp.PIPE, stderr=sp.PIPE, shell=True)
    except:
        pass


def cleanup():
    cmd = 'yes | rm -r abcxyz314;'
    cmd += 'yes | rm -r abcxyz315;'
    cmd += 'yes | rm *.wild314;'
    cmd += 'yes | rm abc*;'
    cmd += 'yes | rm middle*;'
    try:
        sp.check_call(cmd, stdin=sp.PIPE, stdout=sp.PIPE, stderr=sp.PIPE, shell=True)
    except:
        pass


def build_cmd(test):
    cmd = ''
    if 'cdcmd' in test:
        cmd += test['cdcmd'] + '\n'
        cmd += 'timeout ' + str(test['timeout']) + ' ../' + PROG_NAME
    else:
        cmd += 'timeout ' + str(test['timeout']) + ' ' + PROG_NAME
    if DEBUG:
        print('cmd: ' + cmd)
    return cmd


def exec_test(test):
    prog = sp.Popen(build_cmd(test), stdin=sp.PIPE, stdout=sp.PIPE, stderr=sp.PIPE, shell=True, cwd=os.path.dirname(os.path.realpath(__file__)))
    prog.stdin.write(test['input'])
    output = prog.stdout.readlines()
    err_output = prog.stderr.readlines()
    prog.wait()
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
        for line in output:
            print(line, end='')

        print()
    if err_output:
        print('stderr:')
        print(err_output)
        fail_test(test)
        return
    if test['output'] != output:
        print('Expected:')
        for line in test['output']:
            print(line, end='')

        print()
        print('Got:')
        for line in output:
            print(line, end='')

        print()
        fail_test(test)
        return
    pass_test(test)


def fail_test(test):
    print(FAILED.format(test['name'], 0, test['pts']))


def pass_test(test):
    global POINTS
    POINTS += test['pts']
    print(PASSED.format(test['name'], test['pts'], test['pts']))


def main():
    print(os.path.basename(os.path.realpath(__file__)) + ':')
    prep()
    time.sleep(0.1)
    print('Note: To not delete the generated files for testing, pass any argument to the tester')
    for test in TESTS:
        try:
            exec_test(test)
        except Exception as e:
            print(e)
            fail_test(test)

    if not DEBUG and len(sys.argv) < 2:
        cleanup()
    print(('Points received: {0}/{1}').format(POINTS, TOTAL_POINTS))


if __name__ == '__main__':
    main()