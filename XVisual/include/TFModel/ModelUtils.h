#ifndef ModelUtils_H
#define ModelUtils_H

#include <iostream>
#include <string>

namespace XVisual {

void acquireModelInfo(std::string saved_model_txt_fnm,
	std::string& inputKey1, std::string& inputType1Str, std::string& inputShape1Str, std::string& inputName1Str,
	std::string& outputKey1, std::string& outputType1Str, std::string& outputShape1Str, std::string& outputName1Str
);

} // namespace XVisual

#endif //ModelUtils_H
