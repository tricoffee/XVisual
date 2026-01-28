#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
批量为XVisual项目中的所有.cpp文件添加namespace XVisual
改进版：处理所有缺失namespace的文件
"""

import os
import re

def process_cpp_file(file_path):
    """处理单个.cpp文件，添加namespace XVisual"""
    
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
    except Exception as e:
        print(f"无法读取文件 {file_path}: {e}")
        return False
    
    # 检查是否已经包含namespace XVisual
    if 'namespace XVisual' in content:
        return False  # 已有namespace，跳过
    
    # 找到最后一个#include的位置
    include_pattern = r'^#include.*?$'
    includes = list(re.finditer(include_pattern, content, re.MULTILINE))
    
    if not includes:
        return False  # 没有include，跳过
    
    # 获取最后一个include的结束位置
    last_include_end = includes[-1].end()
    
    # 找到最后一个include后的第一个非空行
    remaining_content = content[last_include_end:]
    first_non_empty_pos = 0
    for i, char in enumerate(remaining_content):
        if char != '\n':
            first_non_empty_pos = i
            break
    
    namespace_start_pos = last_include_end + first_non_empty_pos
    
    # 在namespace前后添加
    before_namespace = content[:namespace_start_pos]
    after_namespace_start = content[namespace_start_pos:]
    
    # 添加namespace
    new_content = before_namespace + "\nnamespace XVisual {\n" + after_namespace_start.rstrip() + "\n} // namespace XVisual\n"
    
    try:
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(new_content)
        print(f"✓ 已处理: {file_path}")
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
    total_files = 0
    
    print("开始批量处理cpp文件...\n")
    
    # 递归遍历src目录
    for root, dirs, files in os.walk(base_path):
        for file in files:
            if file.endswith('.cpp'):
                total_files += 1
                file_path = os.path.join(root, file)
                if process_cpp_file(file_path):
                    processed_count += 1
    
    print("\n" + "=" * 60)
    print("处理完成！")
    print(f"总文件数: {total_files}")
    print(f"已处理: {processed_count} 个文件")
    print(f"已跳过（已有namespace或其他原因）: {total_files - processed_count} 个文件")
    print("=" * 60)

if __name__ == "__main__":
    main()