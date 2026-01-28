#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
批量为XVisual项目中的所有.cpp文件添加namespace XVisual（最终版）
"""

import os
import re

def add_namespace_to_file(file_path):
    """为单个文件添加namespace XVisual"""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            lines = f.readlines()
    except:
        return False
    
    # 检查是否已有namespace
    content = ''.join(lines)
    if 'namespace XVisual' in content:
        return False
    
    # 找到所有include行
    include_end_index = 0
    for i, line in enumerate(lines):
        if line.strip().startswith('#include'):
            include_end_index = i + 1
    
    # 如果没有include行，跳过
    if include_end_index == 0:
        return False
    
    # 找到第一个非空、非注释的代码行
    first_code_index = include_end_index
    for i in range(include_end_index, len(lines)):
        stripped = lines[i].strip()
        if stripped and not stripped.startswith('//') and not stripped.startswith('/*'):
            first_code_index = i
            break
    
    # 插入namespace开始
    lines.insert(first_code_index, 'namespace XVisual {\n')
    
    # 在文件末尾添加namespace结束
    lines.append('\n} // namespace XVisual\n')
    
    # 写回文件
    try:
        with open(file_path, 'w', encoding='utf-8') as f:
            f.writelines(lines)
        return True
    except:
        return False

def main():
    base_path = "XVisual/src"
    
    if not os.path.exists(base_path):
        print("错误：目录不存在")
        return
    
    processed = []
    skipped = []
    
    for root, dirs, files in os.walk(base_path):
        for file in files:
            if file.endswith('.cpp'):
                file_path = os.path.join(root, file)
                if add_namespace_to_file(file_path):
                    processed.append(file_path)
                    print(f"✓ {file_path}")
                else:
                    skipped.append(file_path)
    
    print("\n" + "=" * 80)
    print(f"处理完成！")
    print(f"已处理: {len(processed)} 个文件")
    print(f"已跳过（已有namespace或其他原因）: {len(skipped)} 个文件")
    print("=" * 80)

if __name__ == "__main__":
    main()