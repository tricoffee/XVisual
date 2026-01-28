#ifndef MyException_H
#define MyException_H
#include <iostream>
#include <string>
/*
自定义异常类
https://www.aoc.nrao.edu/php/tjuerges/ALMA/STL/html-3.4.6/classstd_1_1exception.html
*/
namespace XVisual {

class MyException : public std::exception
{
	public:
		MyException() : message("MyException Error") {}
		MyException(std::string str) : message(str) {}
		virtual ~MyException() throw () {}
		virtual const char* what() const throw ()
		{
			return message.c_str();
		}
	private:
		std::string message;
};

} // namespace XVisual

#endif //MyException_H
