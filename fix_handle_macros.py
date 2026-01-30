#!/usr/bin/env python3
"""
批量修复 Handle 子类中 sources/dests 宏调用，改为 (*sources) / (*dests)
"""
import re
import os

handle_dir = "XVisual/src/Handle"

# 所有需要替换的宏模式（sources / dests 作为第一个参数）
patterns = [
    (r'\bREGISTER_MEMBER_STR\(sources,',         'REGISTER_MEMBER_STR((*sources),'),
    (r'\bREGISTER_MEMBER_STR\(dests,',           'REGISTER_MEMBER_STR((*dests),'),
    (r'\bREGISTER_MEMBER_ATTR_STR\(sources,',    'REGISTER_MEMBER_ATTR_STR((*sources),'),
    (r'\bREGISTER_MEMBER_ATTR_STR\(dests,',      'REGISTER_MEMBER_ATTR_STR((*dests),'),
    (r'\bREGISTER_TYPE_STR\(sources,',           'REGISTER_TYPE_STR((*sources),'),
    (r'\bREGISTER_TYPE_STR\(dests,',             'REGISTER_TYPE_STR((*dests),'),
    (r'\bGET_MEMBER_WITH_TYPE_STR\(sources,',    'GET_MEMBER_WITH_TYPE_STR((*sources),'),
    (r'\bGET_MEMBER_WITH_TYPE_STR\(dests,',      'GET_MEMBER_WITH_TYPE_STR((*dests),'),
    (r'\bIS_MEMBER_FROM_OUTSIDE_STR\(sources,',  'IS_MEMBER_FROM_OUTSIDE_STR((*sources),'),
    (r'\bIS_MEMBER_FROM_OUTSIDE_STR\(dests,',    'IS_MEMBER_FROM_OUTSIDE_STR((*dests),'),
    (r'\bSET_SOURCEFROM_STR\(sources,',          'SET_SOURCEFROM_STR((*sources),'),
    (r'\bGET_SOURCEFROM_STR\(sources,',          'GET_SOURCEFROM_STR((*sources),'),
    (r'\bACQUIRE_NAMES\(sources\)',              'ACQUIRE_NAMES((*sources))'),
    (r'\bACQUIRE_NAMES\(dests\)',                'ACQUIRE_NAMES((*dests))'),
]

for fname in os.listdir(handle_dir):
    if not fname.endswith('.cpp'):
        continue
    fpath = os.path.join(handle_dir, fname)
    with open(fpath, 'r', encoding='utf-8') as f:
        content = f.read()
    
    original = content
    for pat, repl in patterns:
        content = re.sub(pat, repl, content)
    
    if content != original:
        with open(fpath, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f"Fixed: {fpath}")
    else:
        print(f"No change: {fpath}")

print("Done.")
