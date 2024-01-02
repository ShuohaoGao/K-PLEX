import csv
import json


# 写入CSV文件
def write_to_csv(csv_file_path, data):
    with open(csv_file_path, mode='w', newline='', encoding='utf-8') as file:
        writer = csv.writer(file)
        # 写入数据
        writer.writerows(data)
    print(f"CSV文件已成功生成：{csv_file_path}")


def read_json(path):
    with open(path, 'r', encoding='utf-8') as f:
        data = json.load(f)
    return data


def get_k_value_from_name(name):
    if name[-2] == '-':
        return int(name[-1])
    else:
        return int(name[-2:])


def compare_algorithms():
    kpbb_time = read_json('json-data/sel-kpbb-time.json')
    gap_time = read_json('json-data/sel-gap-time.json')
    chang_time = read_json('json-data/sel-chang-time.json')
    dise_time = read_json('json-data/sel-dise-time.json')
    no_RR_kpbb_time = read_json('json-data/no-RR-kpbb-time.json')
    no_StrongHeuris_kpbb_time = read_json('json-data/no-StrongHeuris-kpbb-time.json')
    no_ub_kpbb_time = read_json('json-data/no-ub-kpbb-time.json')

    k_list = [2, 5, 8, 10]
    key_list = []
    for key in kpbb_time:
        if key not in gap_time:
            print(key, 'is not in gap')
            continue
        assert key in no_RR_kpbb_time and key in no_StrongHeuris_kpbb_time
        key_list.append(key)

    column_name = [
        'graph', 'KPBB', 'KPBB/RR', 'KPBB/StrongHeuris',         'KPBB/RefinedUB',
        'KPLEX', 'kPlexS', 'DiseMKP',
    ]
    for k in k_list:
        data = [column_name]
        for key in key_list:
            if get_k_value_from_name(key) != k:
                continue
            row = [str(key)[7:], kpbb_time[key][0], no_RR_kpbb_time[key][0], no_StrongHeuris_kpbb_time[key][0],
                   no_ub_kpbb_time[key][0],
                   gap_time[key][0], chang_time[key][0], dise_time[key][0],
                   ]
            assert len(row) == len(column_name)
            data.append(row)
        write_to_csv(f'CSV/vary-algo-{k}.csv', data)


def compare_prepro():
    kpbb = read_json('json-data/kpbb-prepro-time.json')
    kpbb_ctcp = read_json('json-data/kpbb-ctcp-prepro-time.json')
    k_list = [2, 5, 8, 10]
    key_list = []
    for key in kpbb:
        key_list.append(key)

    column_name = [
        'graph', 'KPBB', 'KPBB-CTCP', 'lb', 'n', 'm',
    ]
    for k in k_list:
        data = [column_name]
        for key in key_list:
            if get_k_value_from_name(key) != k:
                continue
            a = kpbb[key]
            kpbb_ctcp_time = kpbb_ctcp[key]
            row = [key[7:], a[0], kpbb_ctcp_time, a[1], a[2], a[3]]
            assert len(row) == len(column_name)
            data.append(row)
        write_to_csv(f'CSV/prepro-{k}.csv', data)


# compare_algorithms()
# compare_prepro()
