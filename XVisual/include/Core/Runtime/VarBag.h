#ifndef XVISUAL_CORE_RUNTIME_VARBAG_H
#define XVISUAL_CORE_RUNTIME_VARBAG_H

#include <any>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>

#include "XVariable/SourceFrom.h"

namespace XVisual {

/**
 * VarBag: per-instance 变量容器，接口与旧 Param 保持一致，
 *         可直接替换 Source/Dest singleton。
 */
class VarBag
{
public:
	VarBag() = default;
	~VarBag() = default;

	// 禁止拷贝（保持语义明确，可按需开启移动）
	VarBag(const VarBag&) = delete;
	VarBag& operator=(const VarBag&) = delete;
	VarBag(VarBag&&) = default;
	VarBag& operator=(VarBag&&) = default;

	// ============ 注册 ============
	template <typename T>
	void registerMember(const std::string& name, const T& value)
	{
		members_[name] = value;
	}

	template <typename T>
	void registerMember(const std::string& name, const T& value, bool fromOutside)
	{
		members_[name] = value;
		fromOutside_[name] = fromOutside;
	}

	template <typename T>
	void registerType(const std::string& name, const T& /*value*/)
	{
		types_[name] = &typeid(T);
	}

	// ============ 获取 ============
	template <typename T>
	T& getMemberWithT(const std::string& name)
	{
		auto it = members_.find(name);
		if (it == members_.end())
			throw std::runtime_error("VarBag: member not found: " + name);
		return std::any_cast<T&>(it->second);
	}

	std::any& getMember(const std::string& name)
	{
		auto it = members_.find(name);
		if (it == members_.end())
			throw std::runtime_error("VarBag: member not found: " + name);
		return it->second;
	}

	const std::type_info* getMemberType(const std::string& name) const
	{
		auto it = types_.find(name);
		if (it == types_.end())
			throw std::runtime_error("VarBag: type not found: " + name);
		return it->second;
	}

	bool isFromOutside(const std::string& name) const
	{
		auto it = fromOutside_.find(name);
		if (it == fromOutside_.end())
			return false; // 默认非外部
		return it->second;
	}

	std::vector<std::string> acquireNames() const
	{
		std::vector<std::string> keys;
		keys.reserve(members_.size());
		for (const auto& kv : members_)
			keys.push_back(kv.first);
		return keys;
	}

	// ============ SourceFrom ============
	void setSourceFrom(const std::string& name, const SourceFrom& sf)
	{
		sourceMap_[name] = sf;
	}

	SourceFrom& getSourceFrom(const std::string& name)
	{
		auto it = sourceMap_.find(name);
		if (it == sourceMap_.end())
			throw std::runtime_error("VarBag: sourceFrom not found: " + name);
		return it->second;
	}

private:
	std::unordered_map<std::string, std::any> members_;
	std::unordered_map<std::string, const std::type_info*> types_;
	std::unordered_map<std::string, bool> fromOutside_;
	std::unordered_map<std::string, SourceFrom> sourceMap_;
};

} // namespace XVisual

#endif // XVISUAL_CORE_RUNTIME_VARBAG_H
