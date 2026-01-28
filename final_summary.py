#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
生成XVisual项目namespace XVisual添加情况总结
"""

import os

def check_files_with_namespace():
    """检查所有文件是否包含namespace XVisual"""
    base_path = "XVisual"
    
    # 检查include目录的.h文件
    include_files = []
    include_with_namespace = []
    for root, dirs, files in os.walk(os.path.join(base_path, "include")):
        for file in files:
            if file.endswith('.h'):
                file_path = os.path.join(root, file)
                include_files.append(file_path)
                try:
                    with open(file_path, 'r', encoding='utf-8') as f:
                        content = f.read()
                        if 'namespace XVisual' in content:
                            include_with_namespace.append(file_path)
                except:
                    pass
    
    # 检查src目录的.cpp文件
    src_files = []
    src_with_namespace = []
    for root, dirs, files in os.walk(os.path.join(base_path, "src")):
        for file in files:
            if file.endswith('.cpp'):
                file_path = os.path.join(root, file)
                src_files.append(file_path)
                try:
                    with open(file_path, 'r', encoding='utf-8') as f:
                        content = f.read()
                        if 'namespace XVisual' in content:
                            src_with_namespace.append(file_path)
                except:
                    pass
    
    print("=" * 80)
    print("XVisual项目 namespace XVisual 添加情况总结")
    print("=" * 80)
    print(f"\n头文件 (.h):")
    print(f"  总数: {len(include_files)}")
    print(f"  已添加namespace XVisual: {len(include_with_namespace)}")
    print(f"  未添加namespace XVisual: {len(include_files) - len(include_with_namespace)}")
    
    if len(include_files) - len(include_with_namespace) > 0:
        print(f"\n未添加namespace的头文件:")
        for f in include_files:
            if f not in include_with_namespace:
                print(f"  - {f}")
    
    print(f"\n源文件 (.cpp):")
    print(f"  总数: {len(src_files)}")
    print(f"  已添加namespace XVisual: {len(src_with_namespace)}")
    print(f"  未添加namespace XVisual: {len(src_files) - len(src_with_namespace)}")
    
    if len(src_files) - len(src_with_namespace) > 0:
        print(f"\n未添加namespace的源文件:")
        for f in src_files:
            if f not in src_with_namespace:
                print(f"  - {f}")
    
    print(f"\n总计:")
    total_files = len(include_files) + len(src_files)
    total_with_namespace = len(include_with_namespace) + len(src_with_namespace)
    print(f"  文件总数: {total_files}")
    print(f"  已添加namespace: {total_with_namespace}")
    print(f"  未添加namespace: {total_files - total_with_namespace}")
    print(f"  完成率: {total_with_namespace / total_files * 100:.2f}%")
    print("=" * 80)

if __name__ == "__main__":
    check_files_with_namespace()