#ifndef MyException_H
#define MyException_H
#include <iostream>
#include <string>
/*
�Զ����쳣��
https://www.aoc.nrao.edu/php/tjuerges/ALMA/STL/html-3.4.6/classstd_1_1exception.html
*/
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

#endif //MyException_H
