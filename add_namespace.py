#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
自动为XVisual项目中的所有.cpp文件添加namespace XVisual
"""

import os
import re
from pathlib import Path

def process_cpp_file(file_path):
    """处理单个.cpp文件，添加namespace XVisual"""
    
    # 读取文件内容
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
    except Exception as e:
        print(f"无法读取文件 {file_path}: {e}")
        return False
    
    # 检查是否已经包含namespace XVisual
    if 'namespace XVisual' in content:
        print(f"跳过（已包含namespace）: {file_path}")
        return True
    
    # 找到最后一个#include的位置
    include_pattern = r'^#include.*?$'
    includes = list(re.finditer(include_pattern, content, re.MULTILINE))
    
    if not includes:
        print(f"跳过（没有include）: {file_path}")
        return False
    
    # 获取最后一个include的结束位置
    last_include = includes[-1]
    last_include_end = last_include.end()
    
    # 在最后一个include后添加namespace开始
    before_namespace = content[:last_include_end]
    after_namespace_start = content[last_include_end:]
    
    # 在文件末尾添加namespace结束
    new_content = before_namespace + "\nnamespace XVisual {\n" + after_namespace_start + "\n} // namespace XVisual\n"
    
    # 写回文件
    try:
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(new_content)
        print(f"已处理: {file_path}")
        return True
    except Exception as e:
        print(f"无法写入文件 {file_path}: {e}")
        return False

def main():
    """主函数：遍历src目录并处理所有.cpp文件"""
    base_path = "XVisual/src"
    
    if not os.path.exists(base_path):
        print(f"错误：目录 {base_path} 不存在")
        return
    
    processed_count = 0
    skipped_count = 0
    
    # 递归遍历src目录
    for root, dirs, files in os.walk(base_path):
        for file in files:
            # 只处理.cpp文件
            if file.endswith('.cpp'):
                file_path = os.path.join(root, file)
                if process_cpp_file(file_path):
                    processed_count += 1
                else:
                    skipped_count += 1
    
    print("\n" + "=" * 50)
    print("处理完成！")
    print(f"已处理: {processed_count} 个文件")
    print(f"跳过: {skipped_count} 个文件")
    print("=" * 50)

if __name__ == "__main__":
    main()