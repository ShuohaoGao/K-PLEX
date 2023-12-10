import json


def read_json(path):
    with open(path, 'r', encoding='utf-8') as f:
        data = json.load(f)
    return data


def get_k_value_from_name(name):
    if name[-2] == '-':
        return int(name[-1])
    else:
        return int(name[-2:])


kpbb_size = read_json('json-data/kpbb-size.json')
gap_size = read_json('json-data/gap-size.json')
for key in gap_size:
    if gap_size[key] != -1 and kpbb_size[key] != -1:
        k = get_k_value_from_name(key)
        if gap_size[key] != kpbb_size[key]:
            if max(gap_size[key], kpbb_size[key]) > 2*k-2:
                print(key, gap_size[key], kpbb_size[key])
