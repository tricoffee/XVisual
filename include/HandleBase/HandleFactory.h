#ifndef HandleFactory_H
#define HandleFactory_H

#include<iostream>
#include<string>
#include<map>

class XBaseHandle;
class HandleRegistry
{
	public:
		typedef XBaseHandle* (*Constructor)();
		typedef std::map<std::string, Constructor> HandleClassRegistry;
		static HandleClassRegistry& RegistryInstance();
		static void putConstructor(const std::string& type, Constructor constructor);
		static XBaseHandle* createObject(const std::string& type);

	private:
		HandleRegistry();
};


class HandleClassRegisterer
{
	public:
		HandleClassRegisterer(const std::string& type,
			XBaseHandle* (*creator)());
};
#define REGISTER_HANDLE(T) \
	XBaseHandle* xhandle_##T##_constructor() \
	{ \
		return (XBaseHandle*)(new T##Handle()); \
	} \
    static HandleClassRegisterer  xhandle_##T##_register(#T, xhandle_##T##_constructor)

#endif //HandleFactory_H
