#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace XVisual {

int readClassNames(const std::string& classNamesPath, std::vector<std::string>& classNames);

void getParentPathStr(const std::string& str1, std::string& parentPathStr);

} // namespace XVisual
