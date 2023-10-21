#ifndef PARAM_H
#define PARAM_H
#include <iostream>
#include <unordered_map>
#include <string>
#include <any>
#include <stdexcept>
#include <QDebug>
#include "SourceFrom.h"
class Param
{
	public:
		static Param& getInstance();

		template<typename T>
		void registerMember(const std::string& name, const T& value)
		{
			memberMap[name] = value;
			memberTypes[name] = &typeid(T);
		}

		template<typename T>
		void registerMember(const std::string& name, const T& value, bool from_inner)
		{
			qDebug() << "void registerMember(const std::string& name, const T& value, bool from_inner)";
			memberMap[name] = value;
			memberTypes[name] = &typeid(T);
			memberFrom[name] = from_inner;
		}

		template<typename T>
		T& getMemberWithT(const std::string& name)
		{
			try
			{
				return std::any_cast<T&>(memberMap.at(name));
			}
			catch (const std::out_of_range& e)
			{
				std::cerr << "Member not found: " << name << std::endl;
				std::cout << "Error code: " << -1 << std::endl;
				throw std::runtime_error("Member not found: " + name);
			}
			catch (const std::bad_any_cast& e)
			{
				std::cerr << "Invalid member type for: " << name << std::endl;
				std::cout << "Error code: " << -1 << std::endl;
				throw std::runtime_error("Invalid member type for: " + name);
			}
		}

		std::any& getMember(const std::string& name)
		{
			try
			{
				return memberMap.at(name);
			}
			catch (const std::out_of_range& e)
			{
				std::cerr << "Member not found: " << name << std::endl;
				std::cout << "Error code: " << -1 << std::endl;
				throw std::runtime_error("Member not found: " + name);
			}
			catch (const std::bad_any_cast& e)
			{
				std::cerr << "Invalid member type for: " << name << std::endl;
				std::cout << "Error code: " << -1 << std::endl;
				throw std::runtime_error("Invalid member type for: " + name);
			}
		}

		const std::type_info* getMemberType(const std::string& name)
		{
			try
			{
				qDebug() << "memberTypes.at(name) " << memberTypes.at(name);
				qDebug() << "(*memberTypes.at(name)).name() " << (*memberTypes.at(name)).name();
				return memberTypes.at(name);
			}
			catch (const std::out_of_range& e)
			{
				std::cerr << "Member not found: " << name << std::endl;
				std::cout << "Error code: " << -1 << std::endl;
				throw std::runtime_error("Member not found: " + name);
			}
		}

		bool isFromInner(const std::string& name)
		{
			try
			{
				return std::any_cast<bool>(memberFrom.at(name));
			}
			catch (const std::out_of_range& e)
			{
				std::cerr << "Member not found: " << name << std::endl;
				std::cout << "Error code: " << -1 << std::endl;
				throw std::runtime_error("Member not found: " + name);
			}
			catch (const std::bad_any_cast& e)
			{
				std::cerr << "Invalid member type for: " << name << std::endl;
				std::cout << "Error code: " << -1 << std::endl;
				throw std::runtime_error("Invalid member type for: " + name);
			}
		}

		void setSourceFrom(const std::string& name, const SourceFrom& sourceFrom)
		{
			sourceMap[name] = sourceFrom;
		}

		SourceFrom& getSourceFrom(const std::string& name)
		{
			try
			{
				qDebug() << "SourceFrom& getSourceFrom(const std::string& name) \n name " << QString::fromStdString(name);
				SourceFrom sfrom = sourceMap.at(name);
				qDebug() << "sourceMap.at(name) itemId  " << QString::fromStdString(sfrom.itemId);
				qDebug() << "sourceMap.at(name) name  " << QString::fromStdString(sfrom.name);
				return sourceMap.at(name);
			}
			catch (const std::out_of_range& e)
			{
				std::cerr << "Member not found: " << name << std::endl;
				std::cout << "Error code: " << -1 << std::endl;
				throw std::runtime_error("Member not found: " + name);
			}
			catch (const std::bad_any_cast& e)
			{
				std::cerr << "Invalid member type for: " << name << std::endl;
				std::cout << "Error code: " << -1 << std::endl;
				throw std::runtime_error("Invalid member type for: " + name);
			}
		}

		std::vector<std::string> acquireNames()
		{
			std::vector<std::string> keys;
			qDebug() << "const std::vector<std::string>& acquireNames() const ";
			for (const auto& pair : memberMap) 
			{
				keys.push_back(pair.first);
			}
			return keys;
		}
	
	protected:
		Param();
		
		std::unordered_map<std::string, std::any> memberMap;
		std::unordered_map<std::string, const std::type_info*> memberTypes;
		std::unordered_map<std::string, bool> memberFrom;
		std::unordered_map<std::string, SourceFrom> sourceMap;
};
#endif //PARAM_H
