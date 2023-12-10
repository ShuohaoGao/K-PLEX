import json
import os


def get_files_in_dir(directory):
    ret = []
    for root, dirs, files in os.walk(directory):
        for file in files:
            ret.append(file)
    return ret


def get_gap(file_name):
    time = {}
    size = {}
    file_type = file_name[:-4]
    with open('gap/' + file_name, 'r', encoding='utf-8') as f:
        key = ' '
        done = False
        for line in f:
            line = line.strip()
            words = [s for s in line.split(' ') if s]
            if line.startswith('============== '):
                key = file_type + '-' + words[1] + '-' + words[2]
            elif line.startswith('ground truth='):
                time[key] = float(words[6])
                size[key] = int(words[2])
                done = True
            elif line.startswith('-------------- '):
                if not done:
                    print('gap OOT', key)
                    time[key] = 3620
                    size[key] = -1
                done = False
    return time, size


def get_dise(file_name):
    time = {}
    size = {}
    file_type = file_name[:-4]
    with open('dise/' + file_name, 'r', encoding='utf-8') as f:
        key = ' '
        done = False
        for line in f:
            line = line.strip()
            words = [s for s in line.split(' ') if s]
            if line.startswith('============== '):
                key = file_type + '-' + words[1] + '-' + words[2]
            elif line.startswith('ground truth='):
                time[key] = float(words[6])
                size[key] = int(words[2])
                done = True
            elif line.startswith('-------------- '):
                if not done:
                    print('dise OOT', key)
                    time[key] = 3620
                    size[key] = -1
                done = False
    return time, size


def get_chang(file_name):
    time = {}
    size = {}
    file_type = file_name[:-4]
    with open('chang/' + file_name, 'r', encoding='utf-8') as f:
        key = ' '
        done = False
        for line in f:
            line = line.strip()
            words = [s for s in line.split(' ') if s]
            if line.startswith('============== '):
                key = file_type + '-' + words[1] + '-' + words[2]
            elif line.startswith('ground truth='):
                time[key] = float(words[6])
                size[key] = int(words[2])
                done = True
            elif line.startswith('-------------- '):
                if not done:
                    print('chang OOT', key)
                    time[key] = 3620
                    size[key] = -1
                done = False
    return time, size


def get_format_log(dir_path):
    time = {}
    size = {}
    for file in get_files_in_dir(dir_path):
        key = file[:-4]
        t = 3620
        sz = -1
        done = 0
        with open(dir_path + file, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                words = [s for s in line.split(' ') if s]
                if line == '------------------{whole procedure: kpbb}---------------------':
                    done = True
                elif line.startswith('ground truth=') and done:
                    t = float(words[6])
                    sz = int(words[2])
        if sz == -1:
            print('kpbb OOT', key)
        time[key] = t
        size[key] = sz
    return time, size


def get_gap_all():
    time = {}
    size = {}
    list = ['DIMACS.txt', 'DIMACS10.txt', 'real-world.txt', 'facebook.txt']
    for g in list:
        ret = get_gap(g)
        for key in ret[0]:
            time[key] = ret[0][key]
            size[key] = ret[1][key]
    return time, size


def get_dise_all():
    time = {}
    size = {}
    list = ['DIMACS.txt', 'DIMACS10.txt', 'real-world.txt', 'facebook.txt']
    for g in list:
        ret = get_dise(g)
        for key in ret[0]:
            time[key] = ret[0][key]
            size[key] = ret[1][key]
    return time, size


def get_chang_all():
    time = {}
    size = {}
    list = ['DIMACS.txt', 'DIMACS10.txt', 'real-world.txt', 'facebook.txt']
    for g in list:
        ret = get_chang(g)
        for key in ret[0]:
            time[key] = ret[0][key]
            size[key] = ret[1][key]
    return time, size


def get_k_value_from_name(name):
    if name[-2] == '-':
        return int(name[-1])
    else:
        return int(name[-2:])


def dump_to_json(data, name):
    data = dict(sorted(data.items()))
    with open('json-data/'+name+'.json', 'w', encoding='utf-8') as f:
        string = json.dumps(data, indent=2)
        f.write(string)


kpbb_time, kpbb_size = get_format_log('kpbb/')
gap_time, gap_size = get_gap_all()
chang_time, chang_size = get_chang_all()
dise_time, dise_size = get_dise_all()

dump_to_json(kpbb_time, 'kpbb-time')
dump_to_json(kpbb_size, 'kpbb-size')
dump_to_json(gap_time, 'gap-time')
dump_to_json(gap_size, 'gap-size')
dump_to_json(chang_time, 'chang-time')
dump_to_json(chang_size, 'chang-size')
dump_to_json(dise_time, 'dise-time')
dump_to_json(dise_size, 'dise-size')

graph_types = ['DIMACS', 'DIMACS10', 'real-world', 'facebook']

# for key in gap_time:
#     if key not in kpbb_time:
#         continue
#     k = get_k_value_from_name(key)
#     if gap_size[key] > 2*k -2:
#         if gap_time[key]*2 < kpbb_time[key] and kpbb_time[key] > 1:
#             print(key, gap_time[key], kpbb_time[key], gap_size[key])

for k in [2, 5, 10, 15, 20]:
    kpbb_fast = 0
    gap_fast = 0
    for key in gap_time:
        if get_k_value_from_name(key) != k:
            continue
        if max(kpbb_time[key], gap_time[key]) < 0.5:
            continue
        if kpbb_time[key] * 4 < gap_time[key]:
            kpbb_fast += 1
        elif gap_time[key] * 4 < kpbb_time[key]:
            gap_fast += 1
    print(k, kpbb_fast, gap_fast, len(gap_size)//11)
