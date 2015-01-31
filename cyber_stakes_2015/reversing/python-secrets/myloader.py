# 2015.01.29 04:34:38 EST
#Embedded file name: loader.py
"""
PyStub Loader
"""
import argparse, os
import functools
import py_compile, marshal

def xor(buf, key, mod = 34):
    encrypted = ''
    for i, current in enumerate(buf):
        k = key[i % len(key)]
        encrypted += chr(ord(current) ^ ord(k) ^ mod)

    return encrypted


def run_stub(password, f):
    data = f.read()
    if password:
        data = xor(data, password)
    try:
        exec marshal.loads(data[8:])
        exit(0)
    except Exception:
        print 'Invalid password or python file.'
        exit(1)


def generate_stub(password, f):
    if f.name.endswith('.py'):
        base = f.name[:-3]
        compiled_name = base + '.pyc'
        py_compile.compile(f.name, compiled_name)
        compiled_f = open(compiled_name, 'rb')
        contents = compiled_f.read()
        compiled_f.close()
        stub_name = base + '.stub'
        compiled_f = open(stub_name, 'wb')
        if password:
            contents = xor(contents, password)
        compiled_f.write(contents)
        os.unlink(compiled_name)
    else:
        print '%s does not look like a python file.' % f.name


def main():
    parser = argparse.ArgumentParser(description='Python Stub Loader')
    parser.add_argument('-g', '--generate', action='store_true', help='generate pyc')
    parser.add_argument('-p', '--password', action='store', help='encrypt pyc with given password')
    parser.add_argument('-r', '--run', action='store_true', help='run stub')
    parser.add_argument('file', action='store', help='target py/pyc')
    args = parser.parse_args()
    if not os.path.exists(args.file):
        print 'File: %s does not exist\r\n' % args.file
        exit(1)
    f = open(args.file, 'rb')
    action = None
    if args.generate:
        action = generate_stub
    elif args.run:
        action = run_stub
    else:
        print 'You did not specify an action.'
        exit(1)
    action = functools.partial(action, args.password, f)
    action()


if __name__ == '__main__':
    main()
+++ okay decompyling loader.pyc 
# decompiled 1 files: 1 okay, 0 failed, 0 verify failed
# 2015.01.29 04:34:38 EST
