import json
import numpy as np

import output2bar
import output2line


def read_json(path):
    with open(path, 'r', encoding='utf-8') as f:
        data = json.load(f)
    return data


def get_k_value_from_name(name):
    if name[-2] == '-':
        return int(name[-1])
    else:
        return int(name[-2:])


def get_array(map_v, keys):
    ret = []
    for key in keys:
        ret.append(map_v[key])
    return np.array(ret)


def draw_line_instance(kpbb, gap, dise, chang, key_list):
    y_range_map = {
        '2th-DIMACS-2': [0, 18],
        '2th-DIMACS-3': [0, 20],
        '2th-DIMACS-4': [0, 20],
        '2th-DIMACS-5': [0, 20],
        '2th-DIMACS-6': [0, 15],
        '2th-DIMACS-7': [0, 10],
        '2th-DIMACS-8': [0, 10],
        '2th-DIMACS-9': [0, 12],
        '2th-DIMACS-10': [0, 12],
        '2th-DIMACS-15': [0, 10],
        '2th-DIMACS-20': [0, 8],
        'real-world-2': [0, 120],
        'real-world-3': [0, 150],
        'real-world-4': [0, 150],
        'real-world-5': [0, 150],
        'real-world-6': [0, 150],
        'real-world-7': [0, 150],
        'real-world-8': [0, 150],
        'real-world-9': [0, 150],
        'real-world-10': [0, 150],
        'real-world-15': [0, 160],
        'real-world-20': [0, 160],
    }
    y_tick_map = {
        '2th-DIMACS-2': range(0, 19, 3),
        '2th-DIMACS-3': range(0, 21, 4),
        '2th-DIMACS-4': range(0, 21, 4),
        '2th-DIMACS-5': range(0, 21, 4),
        '2th-DIMACS-6': range(0, 16, 3),
        '2th-DIMACS-7': range(0, 11, 2),
        '2th-DIMACS-8': range(0, 11, 2),
        '2th-DIMACS-9': range(0, 13, 2),
        '2th-DIMACS-10': range(0, 13, 2),
        '2th-DIMACS-15': range(0, 11, 2),
        '2th-DIMACS-20': range(0, 9, 2),
        'real-world-2': range(0, 121, 20),
        'real-world-3': range(0, 151, 30),
        'real-world-4': range(0, 151, 30),
        'real-world-5': range(0, 151, 30),
        'real-world-6': range(0, 151, 30),
        'real-world-7': range(0, 151, 30),
        'real-world-8': range(0, 151, 30),
        'real-world-9': range(0, 151, 30),
        'real-world-10': range(0, 151, 30),
        'real-world-15': range(0, 161, 40),
        'real-world-20': range(0, 161, 40),
    }
    # # 人造图
    type = '2th-DIMACS'
    for k in k_list:
        kpbb_cnt = {}
        gap_cnt = {}
        dise_cnt = {}
        chang_cnt = {}
        for t in time_limits:
            kpbb_cnt[t] = 0
            gap_cnt[t] = 0
            dise_cnt[t] = 0
            chang_cnt[t] = 0
        for key in key_list:
            if not key.startswith(type + '-') or get_k_value_from_name(key) != k:
                continue
            time = kpbb[key][0]
            for t in time_limits:
                if time <= t:
                    kpbb_cnt[t] += 1
            time = gap[key][0]
            for t in time_limits:
                if time <= t:
                    gap_cnt[t] += 1
            time = chang[key][0]
            for t in time_limits:
                if time <= t:
                    chang_cnt[t] += 1
            time = dise[key][0]
            for t in time_limits:
                if time <= t:
                    dise_cnt[t] += 1
        kpbb_cnt = get_array(kpbb_cnt, time_limits)
        gap_cnt = get_array(gap_cnt, time_limits)
        chang_cnt = get_array(chang_cnt, time_limits)
        dise_cnt = get_array(dise_cnt, time_limits)
        y = np.array([kpbb_cnt, gap_cnt, chang_cnt, dise_cnt])
        legends = ['KPBB(ours)', 'KPLEX', 'kPlexS', 'DiseMKP']
        file_name = f'img/{type}-{k}.pdf'
        output2line.draw_lines_solved_instance_cnt(time_limits, y, legends, file_name,
                                                   y_range=y_range_map[f'{type}-{k}'], y_tick=y_tick_map[f'{type}-{k}'])

    # # real-world graph
    type = '2th-DIMACS'
    for k in k_list:
        kpbb_cnt = {}
        gap_cnt = {}
        dise_cnt = {}
        chang_cnt = {}
        for t in time_limits:
            kpbb_cnt[t] = 0
            gap_cnt[t] = 0
            dise_cnt[t] = 0
            chang_cnt[t] = 0
        for key in key_list:
            if key.startswith(type + '-') or get_k_value_from_name(key) != k:
                continue
            time = kpbb[key][0]
            for t in time_limits:
                if time <= t:
                    kpbb_cnt[t] += 1
            time = gap[key][0]
            for t in time_limits:
                if time <= t:
                    gap_cnt[t] += 1
            time = chang[key][0]
            for t in time_limits:
                if time <= t:
                    chang_cnt[t] += 1
            time = dise[key][0]
            for t in time_limits:
                if time <= t:
                    dise_cnt[t] += 1
        kpbb_cnt = get_array(kpbb_cnt, time_limits)
        gap_cnt = get_array(gap_cnt, time_limits)
        chang_cnt = get_array(chang_cnt, time_limits)
        dise_cnt = get_array(dise_cnt, time_limits)
        y = np.array([kpbb_cnt, gap_cnt, chang_cnt, dise_cnt])
        legends = ['KPBB(ours)', 'KPLEX', 'kPlexS', 'DiseMKP']
        file_name = f'img/real-world-{k}.pdf'
        output2line.draw_lines_solved_instance_cnt(time_limits, y, legends, file_name,
                                                   y_range=y_range_map[f'real-world-{k}'],
                                                   y_tick=y_tick_map[f'real-world-{k}'])


kpbb = read_json('json-data/kpbb-time-size.json')
gap = read_json('json-data/gap-time-size.json')
dise = read_json('json-data/dise-time-size.json')
chang = read_json('json-data/chang-time-size.json')
kpbb_no_RR = read_json('json-data/kpbb-no-RR-time-size.json')

# # 检查算法的正确性
# trivial_cnt = 0
# for key in kpbb:
#     if key not in gap:
#         continue
#     if kpbb[key][1] == -1 or gap[key][1] == -1:
#         continue
#     if gap[key][1] <= 2*get_k_value_from_name(key)-2:
#         trivial_cnt += 1
#         continue
#     if gap[key][1] != kpbb[key][1]:
#         print(key, gap[key][1], kpbb[key][1])
# print('Trivial cases:', trivial_cnt)
# exit(1)


graph_types = [
    '2th-DIMACS',
    'Network-Repository',
]
k_list = [i for i in range(2, 11)]
# k_list = [2, 5, 8, 10]
k_list += [15, 20]
# time_limits = [0.5, 1, 2, 4, 8, 16, 32, 64, 128, 256, 1024, 2048, 3600]
time_limits = [1, 5, 20, 100, 500, 1000, 1800, 3600]
key_list = []

for key in kpbb:
    if key not in gap:
        print(key, 'is not found in gap-log')
        continue
    kpbb_time = kpbb[key][0]
    gap_time = gap[key][0]
    chang_time = chang[key][0]
    dise_time = dise[key][0]
    # 去掉全部超时的
    if min(kpbb_time, gap_time, chang_time, dise_time) > 3600:
        continue
    # # 去掉太简单的
    if max(kpbb_time, gap_time, chang_time, dise_time) < 5:
        continue

    # 去掉不符合二跳的
    k = get_k_value_from_name(key)
    if gap[key][1] == 2 * k - 2:
        continue
    if 0 < kpbb[key][1] < 2 * k - 1:
        continue

    # 正确性检测
    if gap[key][1] != kpbb[key][1] and gap[key][1] != -1 and kpbb[key][1] != -1:
        print(key, "kpbb-size=", kpbb[key][1], "gap-size=", gap[key][1])
        exit(1)
    key_list.append(key)
print('合法的case个数:', len(key_list), '原本个数:', len(kpbb))

draw_line_instance(kpbb, gap, dise, chang, key_list)

# # # 查找慢的个例
# for key in key_list:
#     if get_k_value_from_name(key) != 2:
#         continue
#     if kpbb[key][0] > min(gap[key][0], dise[key][0])*2:
#         print(key, '\nkpbb:', kpbb[key], '\ngap:', gap[key], '\ndise:', dise[key], '\n')
# exit(0)

# # # 绘制速度比值的分布
# intervals = [
#     # 1 / 128, 1 / 64, 1 / 32,
#     1 / 16, 1 / 8, 1 / 4, 1 / 2, 2, 4, 8, 16, 32,
#     # 64, 128,
#     1e9]
# for k in k_list:
#     instance_cnt = {}
#     for p in intervals:
#         instance_cnt[p] = 0
#     for key in key_list:
#         if get_k_value_from_name(key) != k:
#             continue
#         # if not key.startswith(type + '-'):
#         #     continue
#         other = min(gap_time[key], dise_time[key], chang_time[key])
#         ratio = other / kpbb_time[key]
#         for p in intervals:
#             if ratio <= p:
#                 instance_cnt[p] += 1
#                 if p < 0.01:
#                     print(key)
#                 break
#     instance_cnt = get_array(instance_cnt, intervals)
#     point_str = ['0',
#                  # '$2^{-7}$', '$2^{-6}$', '$2^{-5}$',
#                  '$2^{-4}$', '$2^{-3}$', '$2^{-2}$', '$2^{-1}$',
#                  '$2^1$', '$2^2$', '$2^3$', '$2^4$', '$2^5$',
#                  # '$2^6$', '$2^7$',
#                  r'$+\infty$']
#     assert len(intervals) == len(point_str)-1
#     x_ticks = [f'({point_str[i-1]},{point_str[i]}]' for i in range(1, len(point_str))]
#     x_ticks[-1] = x_ticks[-1][:-1]+')'
#     y = [instance_cnt]
#     legend = [r'$\frac{T_{others}}{T_{KPBB}}$']
#     file_name = f'img/time-ratio-{k}.png'
#     x_name = 'time ratio'
#     y_name = '#instances'
#     output2bar.draw_bars(x_ticks, y, legend, file_name, x_name, y_name)
