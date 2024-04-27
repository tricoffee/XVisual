#ifndef PARAM_H
#define PARAM_H
#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include <iostream>
#include <unordered_map>
#include <string>
#include <any>
#include <stdexcept>
#include <QDebug>
#include "SourceFrom.h"
#include "Exception/UmapKeyNoFoundException.h"
#include "Exception/ExceptionUtils.h"
class Param
{
	public:
		static Param& getInstance();

		template<typename T>
		void registerMember(const std::string& variableName, const T& value)
		{
			memberMap[variableName] = value;
			XLOG_INFO("Param,registerMember,variableName = " + variableName, CURRENT_THREAD_ID);
		}

		template<typename T>
		void registerMember(const std::string& variableName, const T& value, bool from_outside)
		{
			qDebug() << "void registerMember(const std::string& name, const T& value, bool from_outside)";
			memberMap[variableName] = value;
			memberFrom[variableName] = from_outside;
		}

		template<typename T>
		void registerType(const std::string& variableName, const T& value)
		{
			memberTypes[variableName] = &typeid(T);
			const std::type_info* typeinfo_x = &typeid(T);
			std::string typename_x = (*typeinfo_x).name();
			XLOG_INFO("Param,registerMember,variableName = " + variableName, CURRENT_THREAD_ID);
			XLOG_INFO("Param,registerMember,typename_x = " + typename_x, CURRENT_THREAD_ID);
		}

		template<typename T>
		T& getMemberWithT(const std::string& variableName)
		{
			try
			{
				return std::any_cast<T&>(memberMap.at(variableName));
			}
			catch (const std::out_of_range& e)
			{
				std::cerr << "Member not found: " << variableName << std::endl;
				std::cout << "Error code: " << -1 << std::endl;
				throw std::runtime_error("Member not found: " + variableName);
			}
			catch (const std::bad_any_cast& e)
			{
				std::cerr << "Invalid member type for: " << variableName << std::endl;
				std::cout << "Error code: " << -1 << std::endl;
				throw std::runtime_error("Invalid member type for: " + variableName);
			}
		}

		std::any& getMember(const std::string& variableName)
		{
			try
			{
				return memberMap.at(variableName);
			}
			catch (const std::out_of_range& e)
			{
				std::cerr << "Member not found: " << variableName << std::endl;
				std::cout << "Error code: " << -1 << std::endl;
				throw std::runtime_error("Member not found: " + variableName);
			}
			catch (const std::bad_any_cast& e)
			{
				std::cerr << "Invalid member type for: " << variableName << std::endl;
				std::cout << "Error code: " << -1 << std::endl;
				throw std::runtime_error("Invalid member type for: " + variableName);
			}
		}

		const std::type_info* getMemberType(const std::string& variableName)
		{
			try
			{
				qDebug() << "memberTypes.at(variableName) " << memberTypes.at(variableName);
				qDebug() << "(*memberTypes.at(variableName)).name() " << (*memberTypes.at(variableName)).name();
				return memberTypes.at(variableName);
			}
			catch (const std::out_of_range& e)
			{
				std::cerr << "Member not found: " << variableName << std::endl;
				std::cout << "Error code: " << -1 << std::endl;
				throw std::runtime_error("Member not found: " + variableName);
			}
		}

		bool isFromOutside(const std::string& variableName)
		{
			try
			{
				return std::any_cast<bool>(memberFrom.at(variableName));
			}
			catch (const std::out_of_range& e)
			{
				std::cerr << "Member not found: " << variableName << std::endl;
				std::cout << "Error code: " << -1 << std::endl;
				throw std::runtime_error("Member not found: " + variableName);
			}
			catch (const std::bad_any_cast& e)
			{
				std::cerr << "Invalid member type for: " << variableName << std::endl;
				std::cout << "Error code: " << -1 << std::endl;
				throw std::runtime_error("Invalid member type for: " + variableName);
			}
		}

		void setSourceFrom(const std::string& variableName, const SourceFrom& sourceFrom)
		{
			sourceMap[variableName] = sourceFrom;
		}

		SourceFrom& getSourceFrom(const std::string& variableName)
		{
			qDebug() << "SourceFrom& getSourceFrom(const std::string& variableName) \n variableName " << QString::fromStdString(variableName);
			try
			{	
				bool b;
				keyExists(sourceMap, variableName, b);
			}
			catch (UmapKeyNoFoundException& e)
			{
				//        std::cout << "=== === ===" << e.what() << "=== === ===" << std::endl;
				throw UmapKeyNoFoundException(e.what());
			}
			SourceFrom sfrom = sourceMap.at(variableName);
			qDebug() << "sourceMap.at(variableName) itemId  " << QString::fromStdString(sfrom.itemId);
			qDebug() << "sourceMap.at(variableName) name  " << QString::fromStdString(sfrom.variableName);
			return sourceMap.at(variableName);

			/*catch (const std::out_of_range& e)
			{
				std::cerr << "Member not found: " << variableName << std::endl;
				std::cout << "Error code: " << -1 << std::endl;
				throw std::runtime_error("Member not found: " + variableName);
			}
			catch (const std::bad_any_cast& e)
			{
				std::cerr << "Invalid member type for: " << variableName << std::endl;
				std::cout << "Error code: " << -1 << std::endl;
				throw std::runtime_error("Invalid member type for: " + variableName);
			}*/
		}

		std::vector<std::string> acquireNames()
		{
			XLOG_INFO("Param::acquireNames...... ", CURRENT_THREAD_ID);
			std::vector<std::string> keys;
			for (const auto& pair : memberMap)
			{
				XLOG_INFO("Param::acquireNames = " + pair.first, CURRENT_THREAD_ID);
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
