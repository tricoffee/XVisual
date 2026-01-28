#ifndef UmapKeyNoFoundException_H
#define UmapKeyNoFoundException_H
#include <iostream>
#include <string>
#include "MyException.h"

namespace XVisual {

class UmapKeyNoFoundException : public MyException
{
	public:
		UmapKeyNoFoundException() : message("UmapKeyNoFoundException Error") {}
		UmapKeyNoFoundException(std::string str) : message(str) {}
		~UmapKeyNoFoundException() throw () override {}
		const char* what() const throw () override
		{
			return message.c_str();
		}
	private:
		std::string message;
};

} // namespace XVisual

#endif //UmapKeyNoFoundException_H

