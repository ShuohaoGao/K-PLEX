import json
import os
from functools import cmp_to_key


def get_files_in_dir(directory):
    ret = []
    for root, dirs, files in os.walk(directory):
        for file in files:
            ret.append(file)
    return ret


def get_format_log(dir_path):
    assert dir_path[-1] == '/'
    res = {}
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
        res[key] = [t, sz]
    return res


def get_k_value_from_name(name):
    if name[-2] == '-':
        return int(name[-1])
    else:
        return int(name[-2:])


def custom_sort_key(item):
    key = item[0]
    last_dash_index = key.rfind('-')  # 找到最后一个 '-'
    prefix = key[:last_dash_index]  # 截取前缀部分
    number = key[last_dash_index + 1:]  # 截取数字部分
    return prefix.lower(), int(number)  # 返回一个元组，元组中的第一个元素是前缀（小写），第二个元素是数字


def dump_to_json(data, name):
    data = dict(sorted(data.items(), key=lambda x: custom_sort_key(x)))
    with open('json-data/' + name + '.json', 'w', encoding='utf-8') as f:
        string = json.dumps(data, indent=2)
        f.write(string)


def get_kpbb_preprocessing_time(dir_path):
    if dir_path[-1] != '/':
        dir_path += '/'
    time = {}
    for file in get_files_in_dir(dir_path):
        key = file[:-4]
        t = -1.0
        n = 0
        m = 0
        lb = 0
        with open(dir_path + file, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                words = [s for s in line.split(' ') if s]
                if line.startswith('total-heuristic-time='):
                    t = max(float(words[1]), 0.0001)
                elif line.startswith('Afer CF-CTCP, n='):
                    n = int(words[3])
                    m = int(words[6][:-1])
                elif line.startswith('lb=') and words[3].startswith('FastHeuris-lb'):
                    lb = int(words[1])
        time[key] = [t, lb, n, m]
    return time


def get_preprocessing_time(dir_path):
    if dir_path[-1] != '/':
        dir_path += '/'
    info = {}
    # value = [time, lb, n, m]
    for file in get_files_in_dir(dir_path):
        key = file[:-4]
        with open(dir_path + file, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                words = [s for s in line.split(' ') if s]
                if line.startswith('total-heuristic-time='):
                    assert words[2] == 's,lb='
                    info[key] = [float(words[1]), int(words[3]), int(words[5]), int(words[7])]
                    break
    return info


def get_int(s):
    t = ''
    for c in s:
        if c == ',':
            continue
        t += c
    return int(t)


def get_graph_info(dir_path):
    if dir_path[-1] != '/':
        dir_path += '/'
    info = {}
    for file in get_files_in_dir(dir_path):
        key = file[:-4]
        if get_k_value_from_name(key) != 2:
            continue
        with open(dir_path + file, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                words = [s for s in line.split(' ') if s]
                if line.startswith('n ='):
                    n = get_int(words[2][:-1])
                    m = get_int(words[5])
                elif line.startswith('*** Degeneracy k-plex size:'):
                    d = get_int(words[8][:-1]) - 2
        info[key] = [n, m, d]
    return info


def overall():
    kpbb_time_size = get_format_log('log/kpbb-with-must-include/')
    gap_time_size = get_format_log('log/gap/')
    dise_time_size = get_format_log('log/disemkp/')
    chang_time_size = get_format_log('log/chang/')
    kpheuris_time_size = get_kpbb_preprocessing_time('log/kpbb-with-must-include/')
    kpheuris_ctcp_time_size = get_format_log('log/KPHeuris-CTCP-log/')
    kpbb_no_RR_time_size = get_format_log('log/kpbb-no-RR-log/')

    dump_to_json(kpbb_time_size, 'kpbb-time-size')
    dump_to_json(gap_time_size, 'gap-time-size')
    dump_to_json(dise_time_size, 'dise-time-size')
    dump_to_json(chang_time_size, 'chang-time-size')
    dump_to_json(kpheuris_time_size, 'KPHeuris-time-size')
    dump_to_json(kpheuris_ctcp_time_size, 'KPHeuris-CTCP-time-size')
    dump_to_json(kpbb_no_RR_time_size, 'kpbb-no-RR-time-size')


def parse_select_log():
    kpbb_time = get_format_log('log/select-log/kpbb-sel-log/')
    gap_time = get_format_log('log/select-log/gap-sel-log/')
    chang_time = get_format_log('log/select-log/chang-sel-log/')
    dise_time = get_format_log('log/select-log/dise-sel-log/')

    dump_to_json(kpbb_time, 'sel-kpbb-time')
    dump_to_json(gap_time, 'sel-gap-time')
    dump_to_json(chang_time, 'sel-chang-time')
    dump_to_json(dise_time, 'sel-dise-time')


def statistic():
    n_m_d = get_graph_info('log/gap/')
    dump_to_json(n_m_d, 'graph-info')


# overall()
# parse_select_log()
# statistic()
