#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
检查XVisual项目中所有.cpp文件是否包含namespace XVisual
"""

import os

def check_cpp_files():
    """检查所有cpp文件是否包含namespace XVisual"""
    base_path = "XVisual/src"
    
    if not os.path.exists(base_path):
        print(f"错误：目录 {base_path} 不存在")
        return
    
    with_namespace = []
    without_namespace = []
    total_files = 0
    
    print("开始检查cpp文件...\n")
    
    # 递归遍历src目录
    for root, dirs, files in os.walk(base_path):
        for file in files:
            if file.endswith('.cpp'):
                total_files += 1
                file_path = os.path.join(root, file)
                try:
                    with open(file_path, 'r', encoding='utf-8') as f:
                        content = f.read()
                        if 'namespace XVisual' in content:
                            with_namespace.append(file_path)
                        else:
                            without_namespace.append(file_path)
                except Exception as e:
                    print(f"无法读取文件 {file_path}: {e}")
    
    print("=" * 80)
    print("检查完成！")
    print(f"总文件数: {total_files}")
    print(f"已有namespace XVisual: {len(with_namespace)} 个文件")
    print(f"缺少namespace XVisual: {len(without_namespace)} 个文件")
    print("=" * 80)
    
    if without_namespace:
        print("\n缺少namespace的文件列表：")
        for file_path in without_namespace:
            print(f"  - {file_path}")
    
    return with_namespace, without_namespace

if __name__ == "__main__":
    check_cpp_files()