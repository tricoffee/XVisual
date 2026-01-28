#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
批量为所有还没有namespace XVisual的.cpp文件添加namespace
"""

import os
import re

def add_namespace_to_cpp_file(file_path):
    """为单个.cpp文件添加namespace XVisual"""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # 检查是否已经有namespace
        if 'namespace XVisual' in content:
            return False, "Already has namespace"
        
        # 查找所有的include语句
        include_pattern = r'#include\s*[<"][^>"]+[>"]'
        includes = re.findall(include_pattern, content)
        
        if not includes:
            return False, "No includes found"
        
        # 找到最后一个include语句的位置
        last_include_end = 0
        for match in re.finditer(include_pattern, content):
            last_include_end = match.end()
        
        # 在最后一个include后面添加namespace开始
        lines = content.split('\n')
        namespace_start_line = -1
        
        # 找到最后一个include后的非空行
        for i, line in enumerate(lines):
            if '#include' in line:
                namespace_start_line = i + 1
            elif namespace_start_line != -1 and line.strip():
                break
        
        if namespace_start_line == -1:
            return False, "Could not find position for namespace"
        
        # 在namespace_start_line位置插入namespace开始
        lines.insert(namespace_start_line, '')
        lines.insert(namespace_start_line + 1, 'namespace XVisual {')
        
        # 在文件末尾添加namespace结束
        lines.append('')
        lines.append('} // namespace XVisual')
        
        # 写回文件
        new_content = '\n'.join(lines)
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(new_content)
        
        return True, "Success"
    except Exception as e:
        return False, str(e)

def batch_add_namespace_cpp():
    """批量处理所有.cpp文件"""
    base_path = "XVisual"
    processed = 0
    failed = 0
    
    # 检查src目录的.cpp文件（不包括CJSON/cJSON.c）
    for root, dirs, files in os.walk(os.path.join(base_path, "src")):
        for file in files:
            if file.endswith('.cpp'):
                file_path = os.path.join(root, file)
                
                # 跳过CJSON目录
                if 'CJSON' in root:
                    continue
                
                success, message = add_namespace_to_cpp_file(file_path)
                if success:
                    print(f"✓ {file_path} - {message}")
                    processed += 1
                else:
                    if message != "Already has namespace":
                        print(f"✗ {file_path} - {message}")
                        failed += 1
    
    print(f"\n处理完成: 成功 {processed} 个，失败 {failed} 个")

if __name__ == "__main__":
    batch_add_namespace_cpp()