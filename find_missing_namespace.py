#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
找出还没有添加namespace XVisual的文件
"""

import os

def find_files_without_namespace():
    """查找还没有namespace的文件"""
    missing_files = []
    
    base_path = "XVisual"
    
    # 检查include目录的.h文件
    for root, dirs, files in os.walk(os.path.join(base_path, "include")):
        for file in files:
            if file.endswith('.h'):
                file_path = os.path.join(root, file)
                try:
                    with open(file_path, 'r', encoding='utf-8') as f:
                        content = f.read()
                        if 'namespace XVisual' not in content:
                            missing_files.append(file_path)
                except:
                    pass
    
    # 检查src目录的.cpp文件
    for root, dirs, files in os.walk(os.path.join(base_path, "src")):
        for file in files:
            if file.endswith('.cpp'):
                file_path = os.path.join(root, file)
                try:
                    with open(file_path, 'r', encoding='utf-8') as f:
                        content = f.read()
                        if 'namespace XVisual' not in content:
                            missing_files.append(file_path)
                except:
                    pass
    
    if missing_files:
        print(f"发现 {len(missing_files)} 个文件还没有namespace XVisual:")
        for f in missing_files:
            print(f"  {f}")
    else:
        print("✓ 所有文件都已添加namespace XVisual!")
    
    return missing_files

if __name__ == "__main__":
    find_files_without_namespace()