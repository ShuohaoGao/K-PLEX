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


def get_graph(name):
    graph_types = [
        'Network-Repository-',
        '2th-DIMACS-',
        '10th-DIMACS-',
    ]
    for g_type in graph_types:
        if not name.startswith(g_type):
            continue
        name = name[len(g_type):]
        idx = name.rfind('-')
        assert idx != -1
        name = name[:idx]
        return name
    exit(1)


def compare_algorithms():
    kpbb_time = read_json('json-data/kpbb-time-size.json')
    gap_time = read_json('json-data/gap-time-size.json')
    chang_time = read_json('json-data/chang-time-size.json')
    dise_time = read_json('json-data/dise-time-size.json')
    no_RR_kpbb_time = read_json('json-data/kpbb-no-RR-time-size.json')
    no_ub_kpbb_time = read_json('json-data/kpbb-no-ub-time-size.json')
    no_StrongHeuris_kpbb_time = read_json('json-data/kpbb-no-StrongHeuris-time-size.json')

    column_name = [
        'graph', 'KPBB',
        'KPLEX', 'kPlexS', 'DiseMKP',
        'KPBB/StrongHeuris', 'KPBB/RefinedUB', 'KPBB/RR',
    ]
    for k in k_list:
        key_list = []
        for g in selected_graphs:
            key_list.append(f'{g}-{k}')
        data = [column_name]
        for key in key_list:
            row = [graph_map[get_graph(key)], kpbb_time[key][0],
                   gap_time[key][0], chang_time[key][0], dise_time[key][0],
                   no_StrongHeuris_kpbb_time[key][0],
                   no_ub_kpbb_time[key][0],
                   no_RR_kpbb_time[key][0],
                   ]
            for i in range(1, len(row)):
                if type(row[i]) is float:
                    row[i] = f'{row[i]:.2f}'
            data.append(row)
        write_to_csv(f'CSV/vary-algo-{k}.csv', data)


def grash_info():
    info = read_json('json-data/graph-info.json')
    column_name = [
        'ID', 'Dataset', '|V|', '|E|', 'Density', '$\\delta(G)$',
    ]
    data = [column_name]
    for g in selected_graphs:
        g = g + '-2'
        n, m, d = info[g]
        density = m * 2 / n / (n - 1)
        graph_name = get_graph(g)
        graph_id = f'G{len(data)}'
        graph_map[graph_name] = graph_id
        row = [
            graph_id, graph_name, n, m, f'{density:.4f}', d,
        ]
        data.append(row)
    write_to_csv(f'CSV/datasets.csv', data)
    return graph_map


def compare_CTCP():
    KPHeuris_time = read_json('json-data/KPHeuris-time-size.json')
    KPHeuris_CTCP_time = read_json('json-data/KPHeuris-CTCP-time-size.json')
    kPlexS_prepro = read_json('json-data/kPlexS-prepro-time-size.json')
    column_name = [
        'ID', 'KPHeuris', 'KPHeuris-CTCP',
        # '|V\'|', '|E\'|',
        'lb',
        'kPlexS', 'lb',
    ]
    for k in k_list:
        key_list = []
        for g in selected_graphs:
            if g.startswith('2th-DIMACS'):
                continue
            key_list.append(f'{g}-{k}')
        data = [column_name]
        for key in key_list:
            cf_ctcp, lb, n, m = KPHeuris_time[key]
            ctcp = KPHeuris_CTCP_time[key][0]
            chang_time, lb_chang = kPlexS_prepro[key]
            row = [graph_map[get_graph(key)], f'{cf_ctcp:.2f}', f'{ctcp:.2f}',
                   # n, m,
                   lb,
                   f'{chang_time:.2f}', lb_chang,
                   ]
            data.append(row)
        write_to_csv(f'CSV/prepro-{k}.csv', data)


k_list = [2, 3, 5, 8, 10, 15, 20]
selected_graphs = [
    '2th-DIMACS-johnson8-4-4',
    '2th-DIMACS-C125-9',
    '2th-DIMACS-keller4',
    '2th-DIMACS-brock200-2',
    '2th-DIMACS-san200-0-9-1',
    '2th-DIMACS-san200-0-9-2',
    '2th-DIMACS-san200-0-9-3',
    '2th-DIMACS-p-hat300-1',
    '2th-DIMACS-p-hat300-2',
    '2th-DIMACS-p-hat500-1',

    'Network-Repository-soc-BlogCatalog-ASU',
    'Network-Repository-socfb-UIllinois',
    'Network-Repository-soc-themarker',
    'Network-Repository-soc-BlogCatalog',
    'Network-Repository-soc-buzznet',
    'Network-Repository-soc-LiveMocha',
    'Network-Repository-soc-wiki-conflict',
    'Network-Repository-soc-google-plus',
    'Network-Repository-soc-FourSquare',
    'Network-Repository-rec-epinions-user-ratings',

    'Network-Repository-soc-wiki-Talk-dir',
    'Network-Repository-soc-pokec',
    'Network-Repository-tech-ip',
    'Network-Repository-ia-wiki-Talk-dir',
    'Network-Repository-sx-stackoverflow',
    'Network-Repository-web-wikipedia_link_it',
    'Network-Repository-socfb-A-anon',
    'Network-Repository-soc-livejournal-user-groups',
    'Network-Repository-soc-bitcoin',
    'Network-Repository-soc-sinaweibo',
    # 'Network-Repository-bio-grid-fission-yeast',
    # 'Network-Repository-CL-10K-1d8-L5',
    # 'Network-Repository-copresence-SFHH',
    # 'Network-Repository-copresence-Thiers13',
    # 'Network-Repository-fb-messages',
    # 'Network-Repository-ia-prosper-loans',
    # 'Network-Repository-ia-slashdot-reply-dir',
    # 'Network-Repository-ip-trace',
    # 'Network-Repository-PLC-40-30-L5',
    # 'Network-Repository-PLC-60-30-L2',
    # 'Network-Repository-SFHH-conf-sensor',
    # 'Network-Repository-sc-TSOPF-RS-b2383-c1',
    # 'Network-Repository-socfb-B-anon',
    # 'Network-Repository-web-wikipedia-growth',
]
graph_map = {}
grash_info()
compare_algorithms()
compare_CTCP()
# graphs = read_json('json-data/graph-info.json')
# kpbb = read_json('json-data/kpbb-time-size.json')
# g_list = []
#
# for key in graphs:
#     if kpbb[key][0] > 3600:
#         continue
#     n, m, d = graphs[key]
#     if m > 1e7 and n > 1e6:
#         g_list.append(key)
# print(len(g_list))
# print(g_list)
