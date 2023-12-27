import json
import os


def get_files_in_dir(directory):
    ret = []
    for root, dirs, files in os.walk(directory):
        for file in files:
            ret.append(file)
    return ret


def get_string(s, length=35):
    s = str(s)
    while len(s) < length:
        s += " "
    return s


class Graph:
    def __init__(self, name, n, m, full_name):
        self.name = name
        self.n = n
        self.m = m
        self.full_name = full_name

    def __lt__(self, other):
        return self.n < other.n or self.n == other.n and self.m < other.m

    def __str__(self):
        return f'[{get_string(self.name)}, {get_string(self.n, 9)}, {get_string(self.m, 10)}, {self.full_name}]'

    def __eq__(self, other):
        return self.n == other.n and self.m == other.m


def get_k_value_from_name(name):
    if name[-2] == '-':
        return int(name[-1])
    else:
        return int(name[-2:])


def get_kpbb_format_log(dir_path):
    graphs = []
    for file in get_files_in_dir(dir_path):
        key = file[:-4]
        if get_k_value_from_name(key) != 2:
            continue
        if key.startswith('2th-DIMACS'):
            continue
        with open(dir_path + file, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                words = [s for s in line.split(' ') if s]
                graphs.append(Graph(words[1], int(words[3]), int(words[5]), key))
                break
    return graphs


graph_types = [
    # '2th-DIMACS',
    '10th-DIMACS', 'real-world', 'Network-Repository'
]
graphs = get_kpbb_format_log('python-log/')
graphs = sorted(graphs)
# for g in graphs:
#     print(g)
multi_cnt = 0
for i in range(0, len(graphs)):
    for j in range(i+1, len(graphs)):
        if graphs[i] == graphs[j]:
            # if graphs[i].full_name.startswith('10th-') or graphs[j].full_name.startswith('10th-'):
            #     continue
            # if graphs[i].full_name.startswith('real') or graphs[j].full_name.startswith('real'):
            #     continue
            print(graphs[i])
            print(graphs[j])
            print()
            multi_cnt += 1
print(len(graphs), '重复个数:', multi_cnt)
