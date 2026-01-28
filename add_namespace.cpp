#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

// 处理单个.cpp文件，添加namespace XVisual
bool processCppFile(const fs::path& filePath) {
    // 读取文件内容
    std::ifstream inFile(filePath);
    if (!inFile.is_open()) {
        std::cerr << "Cannot open file: " << filePath << std::endl;
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(inFile)),
                        std::istreambuf_iterator<char>());
    inFile.close();

    // 检查是否已经包含namespace XVisual
    if (content.find("namespace XVisual") != std::string::npos) {
        std::cout << "Skipped (already has namespace): " << filePath << std::endl;
        return true;
    }

    // 找到第一个#include之后的位置
    size_t pos = 0;
    size_t lastIncludePos = 0;
    while ((pos = content.find("#include", pos)) != std::string::npos) {
        size_t endLine = content.find('\n', pos);
        lastIncludePos = endLine;
        pos = endLine + 1;
    }

    // 如果找到include，在其后添加namespace开始
    if (lastIncludePos > 0) {
        // 插入namespace XVisual开始
        content.insert(lastIncludePos + 1, "\nnamespace XVisual {\n");
        
        // 在文件末尾添加namespace结束
        if (content.back() == '\n') {
            content += "\n} // namespace XVisual\n";
        } else {
            content += "\n\n} // namespace XVisual\n";
        }
        
        // 写回文件
        std::ofstream outFile(filePath);
        if (!outFile.is_open()) {
            std::cerr << "Cannot write to file: " << filePath << std::endl;
            return false;
        }
        outFile << content;
        outFile.close();
        
        std::cout << "Processed: " << filePath << std::endl;
        return true;
    }

    std::cout << "Skipped (no includes): " << filePath << std::endl;
    return false;
}

int main() {
    std::string basePath = "XVisual/src";
    
    int processedCount = 0;
    int skippedCount = 0;
    
    // 递归遍历src目录
    for (const auto& entry : fs::recursive_directory_iterator(basePath)) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            // 只处理.cpp文件，跳过cJSON.c和ui文件
            if (ext == ".cpp") {
                if (processCppFile(entry.path())) {
                    processedCount++;
                } else {
                    skippedCount++;
                }
            }
        }
    }
    
    std::cout << "\n=================================\n";
    std::cout << "Processing completed!\n";
    std::cout << "Processed: " << processedCount << " files\n";
    std::cout << "Skipped: " << skippedCount << " files\n";
    std::cout << "=================================\n";
    
    return 0;
}