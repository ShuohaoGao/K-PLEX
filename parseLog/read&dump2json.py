import json
import os


def get_files_in_dir(directory):
    ret = []
    for root, dirs, files in os.walk(directory):
        for file in files:
            ret.append(file)
    return ret


def get_format_log(dir_path):
    assert dir_path[-1] == '/'
    time = {}
    size = {}
    for file in get_files_in_dir(dir_path):
        key = file[:-4]
        t = 3620
        sz = -1
        with open(dir_path + file, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                words = [s for s in line.split(' ') if s]
                if line.startswith('ground truth=') and words[5].startswith('time'):
                    t = max(float(words[6]), 0.0001)
                    sz = int(words[2])
        time[key] = t
        size[key] = sz
    return time, size


def get_k_value_from_name(name):
    if name[-2] == '-':
        return int(name[-1])
    else:
        return int(name[-2:])


def dump_to_json(data, name):
    data = dict(sorted(data.items()))
    with open('json-data/' + name + '.json', 'w', encoding='utf-8') as f:
        string = json.dumps(data, indent=2)
        f.write(string)


kpbb_time, kpbb_size = get_format_log('kpbb/')
gap_time, gap_size = get_format_log('gap/')
chang_time, chang_size = get_format_log('chang/')
dise_time, dise_size = get_format_log('disemkp/')

dump_to_json(kpbb_time, 'kpbb-time')
dump_to_json(kpbb_size, 'kpbb-size')
dump_to_json(gap_time, 'gap-time')
dump_to_json(gap_size, 'gap-size')
dump_to_json(chang_time, 'chang-time')
dump_to_json(chang_size, 'chang-size')
dump_to_json(dise_time, 'dise-time')
dump_to_json(dise_size, 'dise-size')

graph_types = [
    '2th-DIMACS',
    '10th-DIMACS',
    'Network-Repository',
]

# for key in gap_time:
#     if key not in kpbb_time:
#         continue
#     k = get_k_value_from_name(key)
#     if gap_size[key] > 2*k -2:
#         if gap_time[key]*2 < kpbb_time[key] and kpbb_time[key] > 1:
#             print(key, gap_time[key], kpbb_time[key], gap_size[key])
#
# for k in [2, 5, 10, 15, 20]:
#     kpbb_fast = 0
#     gap_fast = 0
#     for key in gap_time:
#         if get_k_value_from_name(key) != k:
#             continue
#         if max(kpbb_time[key], gap_time[key]) < 0.5:
#             continue
#         if kpbb_time[key] * 4 < gap_time[key]:
#             kpbb_fast += 1
#         elif gap_time[key] * 4 < kpbb_time[key]:
#             gap_fast += 1
#     print(k, kpbb_fast, gap_fast, len(gap_size)//11)
