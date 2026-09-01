import os, os.path
import re

def html2h(sname, dname):
    with open(dname, 'w', encoding='utf-8') as dfd:
        vname = re.sub(r'[^a-zA-Z_]', '_', sname)
        dfd.write(f'static const char html2h_{vname}[]={{')
        with open(sname, 'r', encoding='utf-8') as sfd:
            while True:
                line = sfd.readline()
                if not line:
                    break
                for c in line.strip().encode('utf-8'):
                    dfd.write(f'0x{c:x},')
        dfd.write('0x0};')

if __name__ == '__main__':
    import sys
    def main():
        if len(sys.argv) > 1:
            sname = sys.argv[1]
        else:
            print('no args')
            return
        if len(sys.argv) > 2:
            dname = sys.argv[2]
        else:
            dname = sname + '.h'
        print(f'convert {sname} -> {dname}')
        html2h(sname, dname)
        print('done')
    main()
