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


def draw_line_instance(kpbb_time, gap_time, dise_time, chang_time, key_list):
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
            time = kpbb_time[key]
            for t in time_limits:
                if time <= t:
                    kpbb_cnt[t] += 1
            time = gap_time[key]
            for t in time_limits:
                if time <= t:
                    gap_cnt[t] += 1
            time = chang_time[key]
            for t in time_limits:
                if time <= t:
                    chang_cnt[t] += 1
            time = dise_time[key]
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
        output2line.draw_lines_solved_instance_cnt(time_limits, y, legends, file_name)

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
            time = kpbb_time[key]
            for t in time_limits:
                if time <= t:
                    kpbb_cnt[t] += 1
            time = gap_time[key]
            for t in time_limits:
                if time <= t:
                    gap_cnt[t] += 1
            time = chang_time[key]
            for t in time_limits:
                if time <= t:
                    chang_cnt[t] += 1
            time = dise_time[key]
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
        output2line.draw_lines_solved_instance_cnt(time_limits, y, legends, file_name)


kpbb_size = read_json('json-data/kpbb-size.json')
gap_size = read_json('json-data/gap-size.json')

kpbb_time = read_json('json-data/kpbb-time.json')
gap_time = read_json('json-data/gap-time.json')
dise_time = read_json('json-data/dise-time.json')
chang_time = read_json('json-data/chang-time.json')

# # 检查算法的正确性
# trivial_cnt = 0
# for key in kpbb_size:
#     if key not in gap_size:
#         continue
#     if kpbb_size[key] == -1 or gap_size[key] == -1:
#         continue
#     if gap_size[key] <= 2*get_k_value_from_name(key)-2:
#         trivial_cnt += 1
#         continue
#     if gap_size[key] != kpbb_size[key]:
#         print(key, gap_size[key], kpbb_size[key])
# print('Trivial cases:', trivial_cnt)
# exit(1)


graph_types = [
    '2th-DIMACS',
    'Network-Repository',
]
k_list = [i for i in range(2, 11)]
# k_list = [2, 5, 8, 10]
k_list += [15, 20]
time_limits = [0.5, 1, 2, 4, 8, 16, 32, 64, 128, 256, 1024, 2048, 3600]
key_list = []

for key in kpbb_time:
    if key not in gap_time:
        print(key, 'is not found in gap-log')
        continue
    kpbb = kpbb_time[key]
    gap = gap_time[key]
    # 去掉全部超时的
    if min(kpbb, gap, chang_time[key], dise_time[key]) > 3600:
        continue
    # # 去掉太简单的
    if max(kpbb, gap, chang_time[key], dise_time[key]) < 5:
        continue

    # 去掉不符合二跳的
    k = get_k_value_from_name(key)
    if gap_size[key] == 2 * k - 2:
        continue
    if 0 < kpbb_size[key] < 2 * k - 1:
        continue

    # 正确性检测
    if gap_size[key] != kpbb_size[key] and gap_size[key] != -1 and kpbb_size[key] != -1:
        print(key, "kpbb-size=", kpbb_size[key], "gap-size=", gap_size[key])
        exit(1)
    key_list.append(key)
print('合法的case个数:', len(key_list))

# draw_line_instance(kpbb_time, gap_time, dise_time, chang_time, key_list)

# # 查找慢的个例
# for key in key_list:
#     if kpbb_time[key] > gap_time[key]*2:
#         print(key, kpbb_time[key], gap_time[key])
# exit(0)

# # 绘制速度比值的分布
intervals = [
    # 1 / 128, 1 / 64, 1 / 32,
    1 / 16, 1 / 8, 1 / 4, 1 / 2, 2, 4, 8, 16, 32,
    # 64, 128,
    1e9]
for k in k_list:
    instance_cnt = {}
    for p in intervals:
        instance_cnt[p] = 0
    for key in key_list:
        if get_k_value_from_name(key) != k:
            continue
        # if not key.startswith(type + '-'):
        #     continue
        other = min(gap_time[key], dise_time[key], chang_time[key])
        ratio = other / kpbb_time[key]
        for p in intervals:
            if ratio <= p:
                instance_cnt[p] += 1
                if p < 0.01:
                    print(key)
                break
    instance_cnt = get_array(instance_cnt, intervals)
    point_str = ['0',
                 # '$2^{-7}$', '$2^{-6}$', '$2^{-5}$',
                 '$2^{-4}$', '$2^{-3}$', '$2^{-2}$', '$2^{-1}$',
                 '$2^1$', '$2^2$', '$2^3$', '$2^4$', '$2^5$',
                 # '$2^6$', '$2^7$',
                 r'$+\infty$']
    assert len(intervals) == len(point_str)-1
    x_ticks = [f'({point_str[i-1]},{point_str[i]}]' for i in range(1, len(point_str))]
    x_ticks[-1] = x_ticks[-1][:-1]+')'
    y = [instance_cnt]
    legend = [r'$\frac{T_{others}}{T_{KPBB}}$']
    file_name = f'img/time-ratio-{k}.png'
    x_name = 'time ratio'
    y_name = '#instances'
    output2bar.draw_bars(x_ticks, y, legend, file_name, x_name, y_name)
