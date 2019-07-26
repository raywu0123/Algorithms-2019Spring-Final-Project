from sys import argv
import random
import subprocess
from subprocess import DEVNULL
from shutil import copyfile


def file_len(fname):
    with open(fname) as f:
        for i, l in enumerate(f):
            pass
    return i + 1


def drop_lines(lines: list, drop_num):
    selected_indexes = random.sample(list(range(len(lines))), drop_num)
    new_lines = []
    for idx, line in enumerate(lines):
        if idx in selected_indexes and line.startswith('POLYGON'):
            continue
        new_lines.append(line)
    return new_lines


def verify(exec_path):
    subprocess.run([exec_path, 'tmp.in', 'tmp_o.out'], stdout=DEVNULL)
    num_o = file_len('tmp_o.out')

    subprocess.run(['sed', '-i', 's/SO/SV/g', 'tmp.in'])
    subprocess.run([exec_path, 'tmp.in', 'tmp_v.out'], stdout=DEVNULL)
    num_v = file_len('tmp_v.out')

    subprocess.run(['sed', '-i', 's/SV/SH/g', 'tmp.in'])
    subprocess.run([exec_path, 'tmp.in', 'tmp_h.out'], stdout=DEVNULL)
    num_h = file_len('tmp_h.out')

    print(num_o, num_v, num_h)
    return num_o > min(num_v, num_h)


if __name__ == '__main__':
    exec_path = argv[1]
    filename = argv[2]
    iterations = int(argv[3])
    drop_num = int(argv[4])

    copyfile(filename, 'tmp.in')
    has_error = verify(exec_path)
    if not has_error:
        print('no error in original testcase')
        exit()

    lines = []
    with open(filename, 'r') as file:
        for line in file:
            lines.append(line)

    print(f'{len(lines)} lines in original testcase')

    for it in range(iterations):
        new_lines = drop_lines(lines, drop_num)
        with open('tmp.in', 'w+') as file:
            for l in new_lines:
                file.write(l)

        has_error = verify(exec_path)
        if has_error:
            lines = new_lines
            print(f'shrinked to {len(lines)} lines')
