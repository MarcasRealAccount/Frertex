#include "Template.h"

#include <simdjson.h>

#include <filesystem>
#include <fstream>
#include <iostream>

void EscapeString(std::string& str, char escapeCharacter = '\\')
{
	for (std::size_t i = 0; i < str.size(); ++i)
	{
		if (str[i] == escapeCharacter)
		{
			if (++i >= str.size())
				break;

			switch (str[i])
			{
			case 'a':
				str.replace(i - 1, 2, "\a");
				break;
			case 'b':
				str.replace(i - 1, 2, "\b");
				break;
			case 'f':
				str.replace(i - 1, 2, "\f");
				break;
			case 'n':
				str.replace(i - 1, 2, "\n");
				break;
			case 'r':
				str.replace(i - 1, 2, "\r");
				break;
			case 't':
				str.replace(i - 1, 2, "\t");
				break;
			case 'v':
				str.replace(i - 1, 2, "\v");
				break;
			default:
				str.erase(i - 1, 1);
				break;
			}
			--i;
		}
	}
}

ETemplateMacroType CheckText(std::string_view str)
{
	if (str == "true" || str == "false")
		return ETemplateMacroType::Bool;
	if (str == "null")
		return ETemplateMacroType::Null;
	return ETemplateMacroType::Text;
}

ETemplateMacroType CheckTextForNumber(std::string_view str, bool signed_ = false)
{
	bool hasDecimalPoint = false;
	for (std::size_t i = 0; i < str.size(); ++i)
	{
		if (hasDecimalPoint)
		{
			if (!std::isdigit(str[i]))
				return ETemplateMacroType::Text;
		}
		else
		{
			if (str[i] == '.')
				hasDecimalPoint = true;
			else if (!std::isdigit(str[i]))
				return CheckText(str);
		}
	}
	if (hasDecimalPoint)
		return ETemplateMacroType::Float;
	if (signed_)
		return ETemplateMacroType::Int;
	return ETemplateMacroType::UInt;
}

ETemplateMacroType TemplateMacro::GetBestType(std::string_view str)
{
	// If str is empty, the type is Text
	if (str.empty())
		return ETemplateMacroType::Text;

	if (str[0] == '-')
		return CheckTextForNumber(str.substr(1), true);
	return CheckTextForNumber(str);
}

void TemplateMacro::pushFront(TemplateMacro& value)
{
	if (m_Type == ETemplateMacroType::Ref)
	{
		std::get<TemplateMacro*>(m_Value)->pushFront(value);
		return;
	}

	if (m_Type != ETemplateMacroType::Array)
		return;

	auto& arr = std::get<std::vector<TemplateMacro>>(m_Value);
	arr.insert(arr.begin(), value);
}

void TemplateMacro::pushBack(TemplateMacro& value)
{
	if (m_Type == ETemplateMacroType::Ref)
	{
		std::get<TemplateMacro*>(m_Value)->pushBack(value);
		return;
	}

	if (m_Type != ETemplateMacroType::Array)
		return;

	auto& arr = std::get<std::vector<TemplateMacro>>(m_Value);
	arr.push_back(value);
}

void TemplateMacro::popFront()
{
	if (m_Type == ETemplateMacroType::Ref)
	{
		std::get<TemplateMacro*>(m_Value)->popFront();
		return;
	}

	if (m_Type != ETemplateMacroType::Array)
		return;

	auto& arr = std::get<std::vector<TemplateMacro>>(m_Value);
	if (!arr.empty())
		arr.erase(arr.begin());
}

void TemplateMacro::popBack()
{
	if (m_Type == ETemplateMacroType::Ref)
	{
		std::get<TemplateMacro*>(m_Value)->popFront();
		return;
	}

	if (m_Type != ETemplateMacroType::Array)
		return;

	auto& arr = std::get<std::vector<TemplateMacro>>(m_Value);
	arr.pop_back();
}

TemplateMacro* TemplateMacro::assignNullMacro(std::size_t index)
{
	if (m_Type == ETemplateMacroType::Ref)
	{
		auto ptr = std::get<TemplateMacro*>(m_Value);
		if (ptr->m_Type != ETemplateMacroType::Array)
			return nullptr;

		auto& arr = std::get<std::vector<TemplateMacro>>(ptr->m_Value);
		if (index >= arr.size())
			arr.resize(index + 1);
		arr[index] = { ETemplateMacroType::Null, false };
		return &arr[index];
	}

	if (m_Type != ETemplateMacroType::Array)
		return nullptr;

	auto& arr = std::get<std::vector<TemplateMacro>>(m_Value);
	if (index >= arr.size())
		arr.resize(index + 1);
	arr[index] = { ETemplateMacroType::Null, false };
	return &arr[index];
}

TemplateMacro* TemplateMacro::assignBoolMacro(std::size_t index, bool value)
{
	if (m_Type == ETemplateMacroType::Ref)
	{
		auto ptr = std::get<TemplateMacro*>(m_Value);
		if (ptr->m_Type != ETemplateMacroType::Array)
			return nullptr;

		auto& arr = std::get<std::vector<TemplateMacro>>(ptr->m_Value);
		if (index >= arr.size())
			arr.resize(index + 1);
		arr[index] = { ETemplateMacroType::Bool, value };
		return &arr[index];
	}

	if (m_Type != ETemplateMacroType::Array)
		return nullptr;

	auto& arr = std::get<std::vector<TemplateMacro>>(m_Value);
	if (index >= arr.size())
		arr.resize(index + 1);
	arr[index] = { ETemplateMacroType::Bool, value };
	return &arr[index];
}

TemplateMacro* TemplateMacro::assignIntMacro(std::size_t index, std::int64_t value)
{
	if (m_Type == ETemplateMacroType::Ref)
	{
		auto ptr = std::get<TemplateMacro*>(m_Value);
		if (ptr->m_Type != ETemplateMacroType::Array)
			return nullptr;

		auto& arr = std::get<std::vector<TemplateMacro>>(ptr->m_Value);
		if (index >= arr.size())
			arr.resize(index + 1);
		arr[index] = { ETemplateMacroType::Int, value };
		return &arr[index];
	}

	if (m_Type != ETemplateMacroType::Array)
		return nullptr;

	auto& arr = std::get<std::vector<TemplateMacro>>(m_Value);
	if (index >= arr.size())
		arr.resize(index + 1);
	arr[index] = { ETemplateMacroType::Int, value };
	return &arr[index];
}

TemplateMacro* TemplateMacro::assignUIntMacro(std::size_t index, std::uint64_t value)
{
	if (m_Type == ETemplateMacroType::Ref)
	{
		auto ptr = std::get<TemplateMacro*>(m_Value);
		if (ptr->m_Type != ETemplateMacroType::Array)
			return nullptr;

		auto& arr = std::get<std::vector<TemplateMacro>>(ptr->m_Value);
		if (index >= arr.size())
			arr.resize(index + 1);
		arr[index] = { ETemplateMacroType::UInt, value };
		return &arr[index];
	}

	if (m_Type != ETemplateMacroType::Array)
		return nullptr;

	auto& arr = std::get<std::vector<TemplateMacro>>(m_Value);
	if (index >= arr.size())
		arr.resize(index + 1);
	arr[index] = { ETemplateMacroType::UInt, value };
	return &arr[index];
}

TemplateMacro* TemplateMacro::assignFloatMacro(std::size_t index, double value)
{
	if (m_Type == ETemplateMacroType::Ref)
	{
		auto ptr = std::get<TemplateMacro*>(m_Value);
		if (ptr->m_Type != ETemplateMacroType::Array)
			return nullptr;

		auto& arr = std::get<std::vector<TemplateMacro>>(ptr->m_Value);
		if (index >= arr.size())
			arr.resize(index + 1);
		arr[index] = { ETemplateMacroType::Float, value };
		return &arr[index];
	}

	if (m_Type != ETemplateMacroType::Array)
		return nullptr;

	auto& arr = std::get<std::vector<TemplateMacro>>(m_Value);
	if (index >= arr.size())
		arr.resize(index + 1);
	arr[index] = { ETemplateMacroType::Float, value };
	return &arr[index];
}

TemplateMacro* TemplateMacro::assignTextMacro(std::size_t index, std::string_view value)
{
	if (m_Type == ETemplateMacroType::Ref)
	{
		auto ptr = std::get<TemplateMacro*>(m_Value);
		if (ptr->m_Type != ETemplateMacroType::Array)
			return nullptr;

		auto& arr = std::get<std::vector<TemplateMacro>>(ptr->m_Value);
		if (index >= arr.size())
			arr.resize(index + 1);
		arr[index] = { ETemplateMacroType::Text, std::string { value } };
		return &arr[index];
	}

	if (m_Type != ETemplateMacroType::Array)
		return nullptr;

	auto& arr = std::get<std::vector<TemplateMacro>>(m_Value);
	if (index >= arr.size())
		arr.resize(index + 1);
	arr[index] = { ETemplateMacroType::Text, std::string { value } };
	return &arr[index];
}

TemplateMacro* TemplateMacro::assignArrayMacro(std::size_t index)
{
	if (m_Type == ETemplateMacroType::Ref)
	{
		auto ptr = std::get<TemplateMacro*>(m_Value);
		if (ptr->m_Type != ETemplateMacroType::Array)
			return nullptr;

		auto& arr = std::get<std::vector<TemplateMacro>>(ptr->m_Value);
		if (index >= arr.size())
			arr.resize(index + 1);
		arr[index] = { ETemplateMacroType::Array, std::vector<TemplateMacro> {} };
		return &arr[index];
	}

	if (m_Type != ETemplateMacroType::Array)
		return nullptr;

	auto& arr = std::get<std::vector<TemplateMacro>>(m_Value);
	if (index >= arr.size())
		arr.resize(index + 1);
	arr[index] = { ETemplateMacroType::Array, std::vector<TemplateMacro> {} };
	return &arr[index];
}

TemplateMacro* TemplateMacro::assignStructMacro(std::size_t index)
{
	if (m_Type == ETemplateMacroType::Ref)
	{
		auto ptr = std::get<TemplateMacro*>(m_Value);
		if (ptr->m_Type != ETemplateMacroType::Array)
			return nullptr;

		auto& arr = std::get<std::vector<TemplateMacro>>(ptr->m_Value);
		if (index >= arr.size())
			arr.resize(index + 1);
		arr[index] = { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} };
		return &arr[index];
	}

	if (m_Type != ETemplateMacroType::Array)
		return nullptr;

	auto& arr = std::get<std::vector<TemplateMacro>>(m_Value);
	if (index >= arr.size())
		arr.resize(index + 1);
	arr[index] = { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} };
	return &arr[index];
}

TemplateMacro* TemplateMacro::assignRefMacro(std::size_t index, TemplateMacro& value)
{
	if (m_Type == ETemplateMacroType::Ref)
	{
		auto ptr = std::get<TemplateMacro*>(m_Value);
		if (ptr->m_Type != ETemplateMacroType::Array)
			return nullptr;

		auto& arr = std::get<std::vector<TemplateMacro>>(ptr->m_Value);
		if (index >= arr.size())
			arr.resize(index + 1);
		arr[index] = { ETemplateMacroType::Array, &value };
		return &arr[index];
	}

	if (m_Type != ETemplateMacroType::Array)
		return nullptr;

	auto& arr = std::get<std::vector<TemplateMacro>>(m_Value);
	if (index >= arr.size())
		arr.resize(index + 1);
	arr[index] = { ETemplateMacroType::Ref, &value };
	return &arr[index];
}

TemplateMacro* TemplateMacro::assignNullMacro(std::string_view name)
{
	if (m_Type == ETemplateMacroType::Ref)
		return std::get<TemplateMacro*>(m_Value)->assignNullMacro(name);

	if (m_Type != ETemplateMacroType::Struct)
		return nullptr;

	std::size_t dot            = name.find_first_of('.');
	std::size_t subscriptStart = name.find_first_of('[');
	std::size_t firstNameEnd   = std::min(dot, subscriptStart);

	std::string_view firstName = name.substr(0, firstNameEnd);

	auto& map = std::get<std::unordered_map<std::string, TemplateMacro>>(m_Value);
	auto  itr = std::find_if(map.begin(),
                            map.end(),
                            [firstName](const std::pair<std::string, TemplateMacro>& a) {
                                return a.first == firstName;
                            });
	if (itr == map.end())
	{
		if (dot < subscriptStart)
			itr = map.insert({
								 std::string { firstName },
								 TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} }
            })
					  .first;
		else if (subscriptStart < name.size())
			itr = map.insert({
								 std::string { firstName },
								 TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} }
            })
					  .first;
		else
			itr = map.insert({
								 std::string { firstName },
								 TemplateMacro { ETemplateMacroType::Null, false }
            })
					  .first;
	}

	if (firstNameEnd >= name.size())
	{
		itr->second.m_Type  = ETemplateMacroType::Null;
		itr->second.m_Value = false;
		return &itr->second;
	}
	else
	{
		if (dot < subscriptStart)
		{
			return itr->second.assignNullMacro(name.substr(firstNameEnd + 1));
		}
		else
		{
			std::size_t      subscriptEnd = name.find_first_of(']', subscriptStart + 1);
			std::string_view subscript    = name.substr(subscriptStart + 1, subscriptEnd - subscriptStart - 1);
			std::size_t      index        = std::strtoull(subscript.data(), nullptr, 10);
			if (dot < name.size())
			{
				auto str = itr->second.assignStructMacro(index);
				if (!str)
					return nullptr;
				return str->assignNullMacro(name.substr(dot + 1));
			}
			else
			{
				return itr->second.assignNullMacro(index);
			}
		}
	}
}

TemplateMacro* TemplateMacro::assignBoolMacro(std::string_view name, bool value)
{
	if (m_Type == ETemplateMacroType::Ref)
		return std::get<TemplateMacro*>(m_Value)->assignBoolMacro(name, value);

	if (m_Type != ETemplateMacroType::Struct)
		return nullptr;

	std::size_t dot            = name.find_first_of('.');
	std::size_t subscriptStart = name.find_first_of('[');
	std::size_t firstNameEnd   = std::min(dot, subscriptStart);

	std::string_view firstName = name.substr(0, firstNameEnd);

	auto& map = std::get<std::unordered_map<std::string, TemplateMacro>>(m_Value);
	auto  itr = std::find_if(map.begin(),
                            map.end(),
                            [firstName](const std::pair<std::string, TemplateMacro>& a) {
                                return a.first == firstName;
                            });
	if (itr == map.end())
	{
		if (dot < subscriptStart)
			itr = map.insert({
								 std::string { firstName },
								 TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} }
            })
					  .first;
		else if (subscriptStart < name.size())
			itr = map.insert({
								 std::string { firstName },
								 TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} }
            })
					  .first;
		else
			itr = map.insert({
								 std::string { firstName },
								 TemplateMacro { ETemplateMacroType::Bool, false }
            })
					  .first;
	}

	if (firstNameEnd >= name.size())
	{
		itr->second.m_Type  = ETemplateMacroType::Bool;
		itr->second.m_Value = value;
		return &itr->second;
	}
	else
	{
		if (dot < subscriptStart)
		{
			return itr->second.assignBoolMacro(name.substr(firstNameEnd + 1), value);
		}
		else
		{
			std::size_t      subscriptEnd = name.find_first_of(']', subscriptStart + 1);
			std::string_view subscript    = name.substr(subscriptStart + 1, subscriptEnd - subscriptStart - 1);
			std::size_t      index        = std::strtoull(subscript.data(), nullptr, 10);
			if (dot < name.size())
			{
				auto str = itr->second.assignStructMacro(index);
				if (!str)
					return nullptr;
				return str->assignBoolMacro(name.substr(dot + 1), value);
			}
			else
			{
				return itr->second.assignBoolMacro(index, value);
			}
		}
	}
}

TemplateMacro* TemplateMacro::assignIntMacro(std::string_view name, std::int64_t value)
{
	if (m_Type == ETemplateMacroType::Ref)
		return std::get<TemplateMacro*>(m_Value)->assignIntMacro(name, value);

	if (m_Type != ETemplateMacroType::Struct)
		return nullptr;

	std::size_t dot            = name.find_first_of('.');
	std::size_t subscriptStart = name.find_first_of('[');
	std::size_t firstNameEnd   = std::min(dot, subscriptStart);

	std::string_view firstName = name.substr(0, firstNameEnd);

	auto& map = std::get<std::unordered_map<std::string, TemplateMacro>>(m_Value);
	auto  itr = std::find_if(map.begin(),
                            map.end(),
                            [firstName](const std::pair<std::string, TemplateMacro>& a) {
                                return a.first == firstName;
                            });
	if (itr == map.end())
	{
		if (dot < subscriptStart)
			itr = map.insert({
								 std::string { firstName },
								 TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} }
            })
					  .first;
		else if (subscriptStart < name.size())
			itr = map.insert({
								 std::string { firstName },
								 TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} }
            })
					  .first;
		else
			itr = map.insert({
								 std::string { firstName },
								 TemplateMacro { ETemplateMacroType::Int, 0LL }
            })
					  .first;
	}

	if (firstNameEnd >= name.size())
	{
		itr->second.m_Type  = ETemplateMacroType::Int;
		itr->second.m_Value = value;
		return &itr->second;
	}
	else
	{
		if (dot < subscriptStart)
		{
			return itr->second.assignIntMacro(name.substr(firstNameEnd + 1), value);
		}
		else
		{
			std::size_t      subscriptEnd = name.find_first_of(']', subscriptStart + 1);
			std::string_view subscript    = name.substr(subscriptStart + 1, subscriptEnd - subscriptStart - 1);
			std::size_t      index        = std::strtoull(subscript.data(), nullptr, 10);
			if (dot < name.size())
			{
				auto str = itr->second.assignStructMacro(index);
				if (!str)
					return nullptr;
				return str->assignIntMacro(name.substr(dot + 1), value);
			}
			else
			{
				return itr->second.assignIntMacro(index, value);
			}
		}
	}
}

TemplateMacro* TemplateMacro::assignUIntMacro(std::string_view name, std::uint64_t value)
{
	if (m_Type == ETemplateMacroType::Ref)
		return std::get<TemplateMacro*>(m_Value)->assignUIntMacro(name, value);

	if (m_Type != ETemplateMacroType::Struct)
		return nullptr;

	std::size_t dot            = name.find_first_of('.');
	std::size_t subscriptStart = name.find_first_of('[');
	std::size_t firstNameEnd   = std::min(dot, subscriptStart);

	std::string_view firstName = name.substr(0, firstNameEnd);

	auto& map = std::get<std::unordered_map<std::string, TemplateMacro>>(m_Value);
	auto  itr = std::find_if(map.begin(),
                            map.end(),
                            [firstName](const std::pair<std::string, TemplateMacro>& a) {
                                return a.first == firstName;
                            });
	if (itr == map.end())
	{
		if (dot < subscriptStart)
			itr = map.insert({
								 std::string { firstName },
								 TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} }
            })
					  .first;
		else if (subscriptStart < name.size())
			itr = map.insert({
								 std::string { firstName },
								 TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} }
            })
					  .first;
		else
			itr = map.insert({
								 std::string { firstName },
								 TemplateMacro { ETemplateMacroType::UInt, 0ULL }
            })
					  .first;
	}

	if (firstNameEnd >= name.size())
	{
		itr->second.m_Type  = ETemplateMacroType::UInt;
		itr->second.m_Value = value;
		return &itr->second;
	}
	else
	{
		if (dot < subscriptStart)
		{
			return itr->second.assignUIntMacro(name.substr(firstNameEnd + 1), value);
		}
		else
		{
			std::size_t      subscriptEnd = name.find_first_of(']', subscriptStart + 1);
			std::string_view subscript    = name.substr(subscriptStart + 1, subscriptEnd - subscriptStart - 1);
			std::size_t      index        = std::strtoull(subscript.data(), nullptr, 10);
			if (dot < name.size())
			{
				auto str = itr->second.assignStructMacro(index);
				if (!str)
					return nullptr;
				return str->assignUIntMacro(name.substr(dot + 1), value);
			}
			else
			{
				return itr->second.assignUIntMacro(index, value);
			}
		}
	}
}

TemplateMacro* TemplateMacro::assignFloatMacro(std::string_view name, double value)
{
	if (m_Type == ETemplateMacroType::Ref)
		return std::get<TemplateMacro*>(m_Value)->assignFloatMacro(name, value);

	if (m_Type != ETemplateMacroType::Struct)
		return nullptr;

	std::size_t dot            = name.find_first_of('.');
	std::size_t subscriptStart = name.find_first_of('[');
	std::size_t firstNameEnd   = std::min(dot, subscriptStart);

	std::string_view firstName = name.substr(0, firstNameEnd);

	auto& map = std::get<std::unordered_map<std::string, TemplateMacro>>(m_Value);
	auto  itr = std::find_if(map.begin(),
                            map.end(),
                            [firstName](const std::pair<std::string, TemplateMacro>& a) {
                                return a.first == firstName;
                            });
	if (itr == map.end())
	{
		if (dot < subscriptStart)
			itr = map.insert({
								 std::string { firstName },
								 TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} }
            })
					  .first;
		else if (subscriptStart < name.size())
			itr = map.insert({
								 std::string { firstName },
								 TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} }
            })
					  .first;
		else
			itr = map.insert({
								 std::string { firstName },
								 TemplateMacro { ETemplateMacroType::Float, 0.0 }
            })
					  .first;
	}

	if (firstNameEnd >= name.size())
	{
		itr->second.m_Type  = ETemplateMacroType::UInt;
		itr->second.m_Value = value;
		return &itr->second;
	}
	else
	{
		if (dot < subscriptStart)
		{
			return itr->second.assignFloatMacro(name.substr(firstNameEnd + 1), value);
		}
		else
		{
			std::size_t      subscriptEnd = name.find_first_of(']', subscriptStart + 1);
			std::string_view subscript    = name.substr(subscriptStart + 1, subscriptEnd - subscriptStart - 1);
			std::size_t      index        = std::strtoull(subscript.data(), nullptr, 10);
			if (dot < name.size())
			{
				auto str = itr->second.assignStructMacro(index);
				if (!str)
					return nullptr;
				return str->assignFloatMacro(name.substr(dot + 1), value);
			}
			else
			{
				return itr->second.assignFloatMacro(index, value);
			}
		}
	}
}

TemplateMacro* TemplateMacro::assignTextMacro(std::string_view name, std::string_view value)
{
	if (m_Type == ETemplateMacroType::Ref)
		return std::get<TemplateMacro*>(m_Value)->assignTextMacro(name, value);

	if (m_Type != ETemplateMacroType::Struct)
		return nullptr;

	std::size_t dot            = name.find_first_of('.');
	std::size_t subscriptStart = name.find_first_of('[');
	std::size_t firstNameEnd   = std::min(dot, subscriptStart);

	std::string_view firstName = name.substr(0, firstNameEnd);

	auto& map = std::get<std::unordered_map<std::string, TemplateMacro>>(m_Value);
	auto  itr = std::find_if(map.begin(),
                            map.end(),
                            [firstName](const std::pair<std::string, TemplateMacro>& a) {
                                return a.first == firstName;
                            });
	if (itr == map.end())
	{
		if (dot < subscriptStart)
			itr = map.insert({
								 std::string { firstName },
								 TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} }
            })
					  .first;
		else if (subscriptStart < name.size())
			itr = map.insert({
								 std::string { firstName },
								 TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} }
            })
					  .first;
		else
			itr = map.insert({
								 std::string { firstName },
								 TemplateMacro { ETemplateMacroType::Text, std::string {} }
            })
					  .first;
	}

	if (firstNameEnd >= name.size())
	{
		itr->second.m_Type  = ETemplateMacroType::Text;
		itr->second.m_Value = std::string { value };
		return &itr->second;
	}
	else
	{
		if (dot < subscriptStart)
		{
			return itr->second.assignTextMacro(name.substr(firstNameEnd + 1), value);
		}
		else
		{
			std::size_t      subscriptEnd = name.find_first_of(']', subscriptStart + 1);
			std::string_view subscript    = name.substr(subscriptStart + 1, subscriptEnd - subscriptStart - 1);
			std::size_t      index        = std::strtoull(subscript.data(), nullptr, 10);
			if (dot < name.size())
			{
				auto str = itr->second.assignStructMacro(index);
				if (!str)
					return nullptr;
				return str->assignTextMacro(name.substr(dot + 1), value);
			}
			else
			{
				return itr->second.assignTextMacro(index, value);
			}
		}
	}
}

TemplateMacro* TemplateMacro::assignArrayMacro(std::string_view name)
{
	if (m_Type == ETemplateMacroType::Ref)
		return std::get<TemplateMacro*>(m_Value)->assignArrayMacro(name);

	if (m_Type != ETemplateMacroType::Struct)
		return nullptr;

	std::size_t dot            = name.find_first_of('.');
	std::size_t subscriptStart = name.find_first_of('[');
	std::size_t firstNameEnd   = std::min(dot, subscriptStart);

	std::string_view firstName = name.substr(0, firstNameEnd);

	auto& map = std::get<std::unordered_map<std::string, TemplateMacro>>(m_Value);
	auto  itr = std::find_if(map.begin(),
                            map.end(),
                            [firstName](const std::pair<std::string, TemplateMacro>& a) {
                                return a.first == firstName;
                            });
	if (itr == map.end())
	{
		if (dot < subscriptStart)
			itr = map.insert({
								 std::string { firstName },
								 TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} }
            })
					  .first;
		else if (subscriptStart < name.size())
			itr = map.insert({
								 std::string { firstName },
								 TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} }
            })
					  .first;
		else
			itr = map.insert({
								 std::string { firstName },
								 TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} }
            })
					  .first;
	}

	if (firstNameEnd >= name.size())
	{
		return &itr->second;
	}
	else
	{
		if (dot < subscriptStart)
		{
			return itr->second.assignArrayMacro(name.substr(firstNameEnd + 1));
		}
		else
		{
			std::size_t      subscriptEnd = name.find_first_of(']', subscriptStart + 1);
			std::string_view subscript    = name.substr(subscriptStart + 1, subscriptEnd - subscriptStart - 1);
			std::size_t      index        = std::strtoull(subscript.data(), nullptr, 10);
			if (dot < name.size())
			{
				auto str = itr->second.assignStructMacro(index);
				if (!str)
					return nullptr;
				return str->assignArrayMacro(name.substr(dot + 1));
			}
			else
			{
				return itr->second.assignArrayMacro(index);
			}
		}
	}
}

TemplateMacro* TemplateMacro::assignStructMacro(std::string_view name)
{
	if (m_Type == ETemplateMacroType::Ref)
		return std::get<TemplateMacro*>(m_Value)->assignStructMacro(name);

	if (m_Type != ETemplateMacroType::Struct)
		return nullptr;

	std::size_t dot            = name.find_first_of('.');
	std::size_t subscriptStart = name.find_first_of('[');
	std::size_t firstNameEnd   = std::min(dot, subscriptStart);

	std::string_view firstName = name.substr(0, firstNameEnd);

	auto& map = std::get<std::unordered_map<std::string, TemplateMacro>>(m_Value);
	auto  itr = std::find_if(map.begin(),
                            map.end(),
                            [firstName](const std::pair<std::string, TemplateMacro>& a) {
                                return a.first == firstName;
                            });
	if (itr == map.end())
	{
		if (dot < subscriptStart)
			itr = map.insert({
								 std::string { firstName },
								 TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} }
            })
					  .first;
		else if (subscriptStart < name.size())
			itr = map.insert({
								 std::string { firstName },
								 TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} }
            })
					  .first;
		else
			itr = map.insert({
								 std::string { firstName },
								 TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} }
            })
					  .first;
	}

	if (firstNameEnd >= name.size())
	{
		return &itr->second;
	}
	else
	{
		if (dot < subscriptStart)
		{
			return itr->second.assignStructMacro(name.substr(firstNameEnd + 1));
		}
		else
		{
			std::size_t      subscriptEnd = name.find_first_of(']', subscriptStart + 1);
			std::string_view subscript    = name.substr(subscriptStart + 1, subscriptEnd - subscriptStart - 1);
			std::size_t      index        = std::strtoull(subscript.data(), nullptr, 10);
			if (dot < name.size())
			{
				auto str = itr->second.assignStructMacro(index);
				if (!str)
					return nullptr;
				return str->assignStructMacro(name.substr(dot + 1));
			}
			else
			{
				return itr->second.assignStructMacro(index);
			}
		}
	}
}

TemplateMacro* TemplateMacro::assignRefMacro(std::string_view name, TemplateMacro& value)
{
	if (m_Type == ETemplateMacroType::Ref)
		return std::get<TemplateMacro*>(m_Value)->assignRefMacro(name, value);

	if (m_Type != ETemplateMacroType::Struct)
		return nullptr;

	std::size_t dot            = name.find_first_of('.');
	std::size_t subscriptStart = name.find_first_of('[');
	std::size_t firstNameEnd   = std::min(dot, subscriptStart);

	std::string_view firstName = name.substr(0, firstNameEnd);

	auto& map = std::get<std::unordered_map<std::string, TemplateMacro>>(m_Value);
	auto  itr = std::find_if(map.begin(),
                            map.end(),
                            [firstName](const std::pair<std::string, TemplateMacro>& a) {
                                return a.first == firstName;
                            });
	if (itr == map.end())
	{
		if (dot < subscriptStart)
			itr = map.insert({
								 std::string { firstName },
								 TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} }
            })
					  .first;
		else if (subscriptStart < name.size())
			itr = map.insert({
								 std::string { firstName },
								 TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} }
            })
					  .first;
		else
			itr = map.insert({
								 std::string { firstName },
								 TemplateMacro { ETemplateMacroType::Ref, nullptr }
            })
					  .first;
	}

	if (firstNameEnd >= name.size())
	{
		itr->second.m_Type  = ETemplateMacroType::Ref;
		itr->second.m_Value = &value;
		return &itr->second;
	}
	else
	{
		if (dot < subscriptStart)
		{
			return itr->second.assignRefMacro(name.substr(firstNameEnd + 1), value);
		}
		else
		{
			std::size_t      subscriptEnd = name.find_first_of(']', subscriptStart + 1);
			std::string_view subscript    = name.substr(subscriptStart + 1, subscriptEnd - subscriptStart - 1);
			std::size_t      index        = std::strtoull(subscript.data(), nullptr, 10);
			if (dot < name.size())
			{
				auto str = itr->second.assignStructMacro(index);
				if (!str)
					return nullptr;
				return str->assignRefMacro(name.substr(dot + 1), value);
			}
			else
			{
				return itr->second.assignRefMacro(index, value);
			}
		}
	}
}

TemplateMacro* TemplateMacro::get(std::size_t index)
{
	if (m_Type == ETemplateMacroType::Ref)
		return std::get<TemplateMacro*>(m_Value)->get(index);

	if (m_Type != ETemplateMacroType::Array)
		return nullptr;

	auto& arr = std::get<std::vector<TemplateMacro>>(m_Value);
	return index < arr.size() ? &arr[index] : nullptr;
}

const TemplateMacro* TemplateMacro::get(std::size_t index) const
{
	if (m_Type == ETemplateMacroType::Ref)
		return std::get<TemplateMacro*>(m_Value)->get(index);

	if (m_Type != ETemplateMacroType::Array)
		return nullptr;

	auto& arr = std::get<std::vector<TemplateMacro>>(m_Value);
	return index < arr.size() ? &arr[index] : nullptr;
}

TemplateMacro* TemplateMacro::get(std::string_view name)
{
	if (m_Type == ETemplateMacroType::Ref)
		return std::get<TemplateMacro*>(m_Value)->get(name);

	if (m_Type != ETemplateMacroType::Struct)
		return nullptr;

	std::size_t dot            = name.find_first_of('.');
	std::size_t subscriptStart = name.find_first_of('[');
	std::size_t firstNameEnd   = std::min(dot, subscriptStart);

	std::string_view firstName = name.substr(0, firstNameEnd);

	auto& map = std::get<std::unordered_map<std::string, TemplateMacro>>(m_Value);
	auto  itr = std::find_if(map.begin(),
                            map.end(),
                            [name](const std::pair<std::string, TemplateMacro>& a) {
                                return a.first == name;
                            });
	if (itr == map.end())
		return nullptr;

	if (firstNameEnd >= name.size())
	{
		if (itr->second.m_Type == ETemplateMacroType::Ref)
			return std::get<TemplateMacro*>(itr->second.m_Value);
		return &itr->second;
	}
	else
	{
		if (dot < subscriptStart)
		{
			if (itr->second.m_Type != ETemplateMacroType::Struct)
				return nullptr;

			return itr->second.get(name.substr(firstNameEnd + 1));
		}
		else
		{
			if (itr->second.m_Type != ETemplateMacroType::Array)
				return nullptr;

			std::size_t      subscriptEnd = name.find_first_of(']', subscriptStart + 1);
			std::string_view subscript    = name.substr(subscriptStart + 1, subscriptEnd - subscriptStart - 1);
			auto             subscripted  = itr->second.get(std::strtoull(subscript.data(), nullptr, 10));
			if (!subscripted)
				return nullptr;

			return dot < name.size() ? subscripted->get(name.substr(dot + 1)) : subscripted;
		}
	}
}

const TemplateMacro* TemplateMacro::get(std::string_view name) const
{
	if (m_Type == ETemplateMacroType::Ref)
		return std::get<TemplateMacro*>(m_Value)->get(name);

	if (m_Type != ETemplateMacroType::Struct)
		return nullptr;

	std::size_t dot            = name.find_first_of('.');
	std::size_t subscriptStart = name.find_first_of('[');
	std::size_t firstNameEnd   = std::min(dot, subscriptStart);

	std::string_view firstName = name.substr(0, firstNameEnd);

	auto& map = std::get<std::unordered_map<std::string, TemplateMacro>>(m_Value);
	auto  itr = std::find_if(map.begin(),
                            map.end(),
                            [name](const std::pair<std::string, TemplateMacro>& a) {
                                return a.first == name;
                            });
	if (itr == map.end())
		return nullptr;

	if (firstNameEnd >= name.size())
	{
		if (itr->second.m_Type == ETemplateMacroType::Ref)
			return std::get<TemplateMacro*>(itr->second.m_Value);
		return &itr->second;
	}
	else
	{
		if (dot < subscriptStart)
		{
			if (itr->second.m_Type != ETemplateMacroType::Struct)
				return nullptr;

			return itr->second.get(name.substr(firstNameEnd + 1));
		}
		else
		{
			if (itr->second.m_Type != ETemplateMacroType::Array)
				return nullptr;

			std::size_t      subscriptEnd = name.find_first_of(']', subscriptStart + 1);
			std::string_view subscript    = name.substr(subscriptStart + 1, subscriptEnd - subscriptStart - 1);
			auto             subscripted  = itr->second.get(std::strtoull(subscript.data(), nullptr, 10));
			if (!subscripted)
				return nullptr;

			return dot < name.size() ? subscripted->get(name.substr(dot + 1)) : subscripted;
		}
	}
}

void TemplateMacro::remove(std::size_t index)
{
	if (m_Type == ETemplateMacroType::Ref)
	{
		std::get<TemplateMacro*>(m_Value)->remove(index);
		return;
	}

	if (m_Type != ETemplateMacroType::Array)
		return;

	auto& arr = std::get<std::vector<TemplateMacro>>(m_Value);
	if (index < arr.size())
		arr.erase(arr.begin() + index);
}

void TemplateMacro::remove(std::string_view name)
{
	if (m_Type == ETemplateMacroType::Ref)
	{
		std::get<TemplateMacro*>(m_Value)->remove(name);
		return;
	}

	if (m_Type != ETemplateMacroType::Struct)
		return;

	std::size_t dot            = name.find_first_of('.');
	std::size_t subscriptStart = name.find_first_of('[');
	std::size_t firstNameEnd   = std::min(dot, subscriptStart);

	std::string_view firstName = name.substr(0, firstNameEnd);

	auto& map = std::get<std::unordered_map<std::string, TemplateMacro>>(m_Value);
	auto  itr = std::find_if(map.begin(),
                            map.end(),
                            [firstName](const std::pair<std::string, TemplateMacro>& a) {
                                return a.first == firstName;
                            });
	if (itr == map.end())
		return;

	if (firstNameEnd >= name.size())
	{
		map.erase(itr);
	}
	else
	{
		if (dot < subscriptStart)
		{
			if (itr->second.m_Type != ETemplateMacroType::Struct)
				return;

			return itr->second.remove(name.substr(firstNameEnd + 1));
		}
		else
		{
			if (itr->second.m_Type != ETemplateMacroType::Array)
				return;

			std::size_t      subscriptEnd = name.find_first_of(']', subscriptStart + 1);
			std::string_view subscript    = name.substr(subscriptStart + 1, subscriptEnd - subscriptStart - 1);
			std::size_t      index        = std::strtoull(subscript.data(), nullptr, 10);

			if (dot < name.size())
			{
				auto subscripted = itr->second.get(index);
				if (!subscripted)
					return;
				subscripted->remove(name.substr(dot + 1));
			}
			else
			{
				itr->second.remove(index);
			}
		}
	}
}

void TemplateEnvironment::resolveReferences(std::string& str)
{
	std::size_t offset = 0;
	while (offset < str.size())
	{
		std::size_t referenceStart = offset;
		std::size_t state          = 0;
		for (; state < 2 && referenceStart < str.size(); ++referenceStart)
		{
			if (str[referenceStart] == '\\')
			{
				++referenceStart;
				continue;
			}

			switch (state)
			{
			case 0:
				if (str[referenceStart] == m_Engine->m_CommandChar)
					state = 1;
				break;
			case 1:
				if (str[referenceStart] == m_Engine->m_OpenReference)
					state = 2;
				else
					state = 0;
				break;
			}
		}
		if (referenceStart >= str.size())
			break;
		std::size_t referenceEnd = referenceStart;
		state                    = 0;
		referenceStart           -= 2;
		for (; state < 2 && referenceEnd < str.size(); ++referenceEnd)
		{
			if (str[referenceEnd] == '\\')
			{
				++referenceEnd;
				continue;
			}

			switch (state)
			{
			case 0:
				if (str[referenceEnd] == m_Engine->m_CloseReference)
					state = 1;
				break;
			case 1:
				if (str[referenceEnd] == m_Engine->m_CommandChar)
					state = 2;
				else
					state = 0;
				break;
			}
		}
		if (referenceEnd > str.size())
			break;

		std::string_view macroReference { str };
		macroReference = macroReference.substr(referenceStart + 2, referenceEnd - referenceStart - 4);
		auto macro     = getMacro(macroReference);
		if (macro)
		{
			if (macro->m_Type == ETemplateMacroType::Ref)
				macro = std::get<TemplateMacro*>(macro->m_Value);

			switch (macro->m_Type)
			{
			case ETemplateMacroType::Int:
			{
				std::string replacement = std::to_string(std::get<std::int64_t>(macro->m_Value));
				str.replace(referenceStart, referenceEnd - referenceStart, replacement);
				offset = referenceStart + replacement.size();
				break;
			}
			case ETemplateMacroType::UInt:
			{
				std::string replacement = std::to_string(std::get<std::uint64_t>(macro->m_Value));
				str.replace(referenceStart, referenceEnd - referenceStart, replacement);
				offset = referenceStart + replacement.size();
				break;
			}
			case ETemplateMacroType::Text:
			{
				auto& replacement = std::get<std::string>(macro->m_Value);
				str.replace(referenceStart, referenceEnd - referenceStart, replacement);
				offset = referenceStart + replacement.size();
				break;
			}
			}
		}
		else
		{
			std::cerr << "Trying to reference macro '" << macroReference << "' which doesn't exist!!\n";
			str.replace(referenceStart, referenceEnd - referenceStart, "");
			offset = referenceStart;
		}
	}
}

void TemplateEnvironment::eraseRange()
{
	if (m_CallStack.size() <= 1)
	{
		std::cerr << "Can't erase global range!!\n";
		return;
	}

	auto& back = m_CallStack.back();
	if (m_CurrentCall != back.m_Scope->m_OpenCall)
	{
		std::cerr << "Can only erase range within the first call of the range!!\n";
		return;
	}

	auto start = back.m_StartCall->m_SourceIndex + m_Offset;
	auto end   = back.m_EndCall->m_SourceIndex + m_Offset;
	m_Source.erase(start, end - start);
	m_Offset -= end - start;
}

void TemplateEnvironment::insertText(std::string_view text, bool includeSpaces)
{
	if (text.empty())
		return;

	auto&       call = m_Template->m_Calls[m_CurrentCall];
	std::string str(text);
	if (includeSpaces && call.m_Spaces)
	{
		if (call.m_HasNewline)
			str = '\n' + str;
		std::string spaces(call.m_Spaces, ' ');
		str = spaces + str;
		{
			std::size_t offset = 0;
			while (offset < str.size())
			{
				std::size_t newline = str.find_first_of('\n', offset);
				if (newline >= str.size())
					break;
				str.insert(newline + 1, spaces);
				offset = newline + 1 + spaces.size();
			}
		}
	}
	m_Source.insert(call.m_SourceIndex + m_Offset, str);
	m_Offset += str.size();
}

std::string TemplateEnvironment::getSourceRange()
{
	if (m_CallStack.empty())
		return "";

	auto& back  = m_CallStack.back();
	auto  start = back.m_StartCall->m_SourceIndex + m_Offset;
	auto  end   = back.m_EndCall->m_SourceIndex + m_Offset;
	return m_Source.substr(start, end - start);
}

TemplateCallStackEntry* TemplateEnvironment::currentStack()
{
	return m_CallStack.empty() ? nullptr : &m_CallStack.back();
}

const TemplateScope* TemplateEnvironment::currentScope()
{
	return m_CallStack.empty() ? nullptr : m_CallStack.back().m_Scope;
}

const TemplateScope* TemplateEnvironment::getScope(std::size_t scope)
{
	return scope < m_Template->m_Scopes.size() ? &m_Template->m_Scopes[scope] : nullptr;
}

void TemplateEnvironment::exitScope()
{
	if (m_CallStack.size() <= 1)
	{
		std::cerr << "Trying to exit the global scope!!\n";
		return;
	}

	auto& back = m_CallStack.back();
	jump(back.m_Scope->m_CloseCall + 1);
	popScope();
	m_PopScope = false;
}

void TemplateEnvironment::exitScopes()
{
	if (m_CallStack.size() <= 1)
	{
		std::cerr << "Trying to exit the global scope!!\n";
		return;
	}

	auto& back      = m_CallStack.back();
	auto  lastScope = back.m_Scope->m_LastScope != ~0ULL ? getScope(back.m_Scope->m_LastScope) : back.m_Scope;
	jump(lastScope->m_CloseCall + 1);
	popScope();
	m_PopScope = false;
}

void TemplateEnvironment::nextScope()
{
	if (m_CallStack.size() <= 1)
	{
		std::cerr << "Trying to exit the global scope!!\n";
		return;
	}

	auto& back = m_CallStack.back();
	if (back.m_Scope->m_NextScope != ~0ULL)
	{
		// Jump to start of next scope
		auto newScope       = getScope(back.m_Scope->m_NextScope);
		back.m_StartCall    = &m_Template->m_Calls[newScope->m_OpenCall];
		back.m_EndCall      = &m_Template->m_Calls[newScope->m_CloseCall];
		back.m_ScopeIndex   = back.m_Scope->m_NextScope;
		back.m_Scope        = newScope;
		m_CurrentScopeIndex = back.m_ScopeIndex;
		m_CurrentLastCall   = back.m_Scope->m_CloseCall;
		jump(back.m_Scope->m_OpenCall);
	}
	else
	{
		// Exit the scope
		exitScope();
	}
	m_PopScope = false;
}

void TemplateEnvironment::lastScope()
{
	if (m_CallStack.size() <= 1)
	{
		std::cerr << "Trying to exit the global scope!!\n";
		return;
	}

	auto& back = m_CallStack.back();
	if (back.m_Scope->m_LastScope != ~0ULL)
	{
		// Jump to start of last scope
		auto newScope       = getScope(back.m_Scope->m_LastScope);
		back.m_StartCall    = &m_Template->m_Calls[newScope->m_OpenCall];
		back.m_EndCall      = &m_Template->m_Calls[newScope->m_CloseCall];
		back.m_ScopeIndex   = back.m_Scope->m_LastScope;
		back.m_Scope        = newScope;
		m_CurrentScopeIndex = back.m_ScopeIndex;
		m_CurrentLastCall   = back.m_Scope->m_CloseCall;
		jump(back.m_Scope->m_OpenCall);
	}
	else
	{
		// Exit the scope
		exitScope();
	}
	m_PopScope = false;
}

void TemplateEnvironment::setStackUserData(void* userData)
{
	if (m_CallStack.size() <= 1)
	{
		std::cerr << "Can't set user data for global stack!!\n";
		return;
	}

	auto& back = m_CallStack.back();
	if (m_CurrentCall != back.m_Scope->m_OpenCall)
	{
		std::cerr << "Can only set user data for stack within the first call of the stack!!\n";
		return;
	}

	back.m_UserData = userData;
}

void TemplateEnvironment::jumpToStartOfScope()
{
	auto scope = currentScope();
	jump(scope->m_OpenCall + 1);
	m_PopScope = false;
}

void TemplateEnvironment::jump(std::size_t call)
{
	m_CurrentCall   = call;
	m_IncrementCall = false;
}

TemplateMacro* TemplateEnvironment::assignNullMacro(std::string_view name)
{
	std::size_t dot            = name.find_first_of('.');
	std::size_t subscriptStart = name.find_first_of('[');
	std::size_t firstNameEnd   = std::min(dot, subscriptStart);

	std::string_view firstName = name.substr(0, firstNameEnd);

	auto itr = std::find_if(m_Macros.begin(),
							m_Macros.end(),
							[firstName](const std::pair<std::string, TemplateMacro>& a) {
								return a.first == firstName;
							});
	if (itr == m_Macros.end())
	{
		if (dot < subscriptStart)
			itr = m_Macros.insert({
									  std::string { firstName },
									  TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} }
            })
					  .first;
		else if (subscriptStart < name.size())
			itr = m_Macros.insert({
									  std::string { firstName },
									  TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} }
            })
					  .first;
		else
			itr = m_Macros.insert({
									  std::string { firstName },
									  TemplateMacro { ETemplateMacroType::Null, false }
            })
					  .first;
	}

	if (firstNameEnd >= name.size())
	{
		itr->second.m_Type  = ETemplateMacroType::Null;
		itr->second.m_Value = false;
		return &itr->second;
	}
	else
	{
		if (dot < subscriptStart)
		{
			return itr->second.assignNullMacro(name.substr(firstNameEnd + 1));
		}
		else
		{
			std::size_t      subscriptEnd = name.find_first_of(']', subscriptStart + 1);
			std::string_view subscript    = name.substr(subscriptStart + 1, subscriptEnd - subscriptStart - 1);
			std::size_t      index        = std::strtoull(subscript.data(), nullptr, 10);
			if (dot < name.size())
			{
				auto str = itr->second.assignStructMacro(index);
				if (!str)
					return nullptr;
				return str->assignNullMacro(name.substr(dot + 1));
			}
			else
			{
				return itr->second.assignNullMacro(index);
			}
		}
	}
}

TemplateMacro* TemplateEnvironment::assignBoolMacro(std::string_view name, bool value)
{
	std::size_t dot            = name.find_first_of('.');
	std::size_t subscriptStart = name.find_first_of('[');
	std::size_t firstNameEnd   = std::min(dot, subscriptStart);

	std::string_view firstName = name.substr(0, firstNameEnd);

	auto itr = std::find_if(m_Macros.begin(),
							m_Macros.end(),
							[firstName](const std::pair<std::string, TemplateMacro>& a) {
								return a.first == firstName;
							});
	if (itr == m_Macros.end())
	{
		if (dot < subscriptStart)
			itr = m_Macros.insert({
									  std::string { firstName },
									  TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} }
            })
					  .first;
		else if (subscriptStart < name.size())
			itr = m_Macros.insert({
									  std::string { firstName },
									  TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} }
            })
					  .first;
		else
			itr = m_Macros.insert({
									  std::string { firstName },
									  TemplateMacro { ETemplateMacroType::Bool, false }
            })
					  .first;
	}

	if (firstNameEnd >= name.size())
	{
		itr->second.m_Type  = ETemplateMacroType::Bool;
		itr->second.m_Value = value;
		return &itr->second;
	}
	else
	{
		if (dot < subscriptStart)
		{
			return itr->second.assignBoolMacro(name.substr(firstNameEnd + 1), value);
		}
		else
		{
			std::size_t      subscriptEnd = name.find_first_of(']', subscriptStart + 1);
			std::string_view subscript    = name.substr(subscriptStart + 1, subscriptEnd - subscriptStart - 1);
			std::size_t      index        = std::strtoull(subscript.data(), nullptr, 10);
			if (dot < name.size())
			{
				auto str = itr->second.assignStructMacro(index);
				if (!str)
					return nullptr;
				return str->assignBoolMacro(name.substr(dot + 1), value);
			}
			else
			{
				return itr->second.assignBoolMacro(index, value);
			}
		}
	}
}

TemplateMacro* TemplateEnvironment::assignIntMacro(std::string_view name, std::int64_t value)
{
	std::size_t dot            = name.find_first_of('.');
	std::size_t subscriptStart = name.find_first_of('[');
	std::size_t firstNameEnd   = std::min(dot, subscriptStart);

	std::string_view firstName = name.substr(0, firstNameEnd);

	auto itr = std::find_if(m_Macros.begin(),
							m_Macros.end(),
							[firstName](const std::pair<std::string, TemplateMacro>& a) {
								return a.first == firstName;
							});
	if (itr == m_Macros.end())
	{
		if (dot < subscriptStart)
			itr = m_Macros.insert({
									  std::string { firstName },
									  TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} }
            })
					  .first;
		else if (subscriptStart < name.size())
			itr = m_Macros.insert({
									  std::string { firstName },
									  TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} }
            })
					  .first;
		else
			itr = m_Macros.insert({
									  std::string { firstName },
									  TemplateMacro { ETemplateMacroType::Int, 0LL }
            })
					  .first;
	}

	if (firstNameEnd >= name.size())
	{
		itr->second.m_Type  = ETemplateMacroType::Int;
		itr->second.m_Value = value;
		return &itr->second;
	}
	else
	{
		if (dot < subscriptStart)
		{
			return itr->second.assignIntMacro(name.substr(firstNameEnd + 1), value);
		}
		else
		{
			std::size_t      subscriptEnd = name.find_first_of(']', subscriptStart + 1);
			std::string_view subscript    = name.substr(subscriptStart + 1, subscriptEnd - subscriptStart - 1);
			std::size_t      index        = std::strtoull(subscript.data(), nullptr, 10);
			if (dot < name.size())
			{
				auto str = itr->second.assignStructMacro(index);
				if (!str)
					return nullptr;
				return str->assignIntMacro(name.substr(dot + 1), value);
			}
			else
			{
				return itr->second.assignIntMacro(index, value);
			}
		}
	}
}

TemplateMacro* TemplateEnvironment::assignUIntMacro(std::string_view name, std::uint64_t value)
{
	std::size_t dot            = name.find_first_of('.');
	std::size_t subscriptStart = name.find_first_of('[');
	std::size_t firstNameEnd   = std::min(dot, subscriptStart);

	std::string_view firstName = name.substr(0, firstNameEnd);

	auto itr = std::find_if(m_Macros.begin(),
							m_Macros.end(),
							[firstName](const std::pair<std::string, TemplateMacro>& a) {
								return a.first == firstName;
							});
	if (itr == m_Macros.end())
	{
		if (dot < subscriptStart)
			itr = m_Macros.insert({
									  std::string { firstName },
									  TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} }
            })
					  .first;
		else if (subscriptStart < name.size())
			itr = m_Macros.insert({
									  std::string { firstName },
									  TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} }
            })
					  .first;
		else
			itr = m_Macros.insert({
									  std::string { firstName },
									  TemplateMacro { ETemplateMacroType::UInt, 0LL }
            })
					  .first;
	}

	if (firstNameEnd >= name.size())
	{
		itr->second.m_Type  = ETemplateMacroType::UInt;
		itr->second.m_Value = value;
		return &itr->second;
	}
	else
	{
		if (dot < subscriptStart)
		{
			return itr->second.assignUIntMacro(name.substr(firstNameEnd + 1), value);
		}
		else
		{
			std::size_t      subscriptEnd = name.find_first_of(']', subscriptStart + 1);
			std::string_view subscript    = name.substr(subscriptStart + 1, subscriptEnd - subscriptStart - 1);
			std::size_t      index        = std::strtoull(subscript.data(), nullptr, 10);
			if (dot < name.size())
			{
				auto str = itr->second.assignStructMacro(index);
				if (!str)
					return nullptr;
				return str->assignUIntMacro(name.substr(dot + 1), value);
			}
			else
			{
				return itr->second.assignUIntMacro(index, value);
			}
		}
	}
}

TemplateMacro* TemplateEnvironment::assignFloatMacro(std::string_view name, double value)
{
	std::size_t dot            = name.find_first_of('.');
	std::size_t subscriptStart = name.find_first_of('[');
	std::size_t firstNameEnd   = std::min(dot, subscriptStart);

	std::string_view firstName = name.substr(0, firstNameEnd);

	auto itr = std::find_if(m_Macros.begin(),
							m_Macros.end(),
							[firstName](const std::pair<std::string, TemplateMacro>& a) {
								return a.first == firstName;
							});
	if (itr == m_Macros.end())
	{
		if (dot < subscriptStart)
			itr = m_Macros.insert({
									  std::string { firstName },
									  TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} }
            })
					  .first;
		else if (subscriptStart < name.size())
			itr = m_Macros.insert({
									  std::string { firstName },
									  TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} }
            })
					  .first;
		else
			itr = m_Macros.insert({
									  std::string { firstName },
									  TemplateMacro { ETemplateMacroType::Float, 0.0 }
            })
					  .first;
	}

	if (firstNameEnd >= name.size())
	{
		itr->second.m_Type  = ETemplateMacroType::Float;
		itr->second.m_Value = value;
		return &itr->second;
	}
	else
	{
		if (dot < subscriptStart)
		{
			return itr->second.assignFloatMacro(name.substr(firstNameEnd + 1), value);
		}
		else
		{
			std::size_t      subscriptEnd = name.find_first_of(']', subscriptStart + 1);
			std::string_view subscript    = name.substr(subscriptStart + 1, subscriptEnd - subscriptStart - 1);
			std::size_t      index        = std::strtoull(subscript.data(), nullptr, 10);
			if (dot < name.size())
			{
				auto str = itr->second.assignStructMacro(index);
				if (!str)
					return nullptr;
				return str->assignFloatMacro(name.substr(dot + 1), value);
			}
			else
			{
				return itr->second.assignFloatMacro(index, value);
			}
		}
	}
}

TemplateMacro* TemplateEnvironment::assignTextMacro(std::string_view name, std::string_view value)
{
	std::size_t dot            = name.find_first_of('.');
	std::size_t subscriptStart = name.find_first_of('[');
	std::size_t firstNameEnd   = std::min(dot, subscriptStart);

	std::string_view firstName = name.substr(0, firstNameEnd);

	auto itr = std::find_if(m_Macros.begin(),
							m_Macros.end(),
							[firstName](const std::pair<std::string, TemplateMacro>& a) {
								return a.first == firstName;
							});
	if (itr == m_Macros.end())
	{
		if (dot < subscriptStart)
			itr = m_Macros.insert({
									  std::string { firstName },
									  TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} }
            })
					  .first;
		else if (subscriptStart < name.size())
			itr = m_Macros.insert({
									  std::string { firstName },
									  TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} }
            })
					  .first;
		else
			itr = m_Macros.insert({
									  std::string { firstName },
									  TemplateMacro { ETemplateMacroType::Text, std::string {} }
            })
					  .first;
	}

	if (firstNameEnd >= name.size())
	{
		itr->second.m_Type  = ETemplateMacroType::Text;
		itr->second.m_Value = std::string { value };
		return &itr->second;
	}
	else
	{
		if (dot < subscriptStart)
		{
			return itr->second.assignTextMacro(name.substr(firstNameEnd + 1), value);
		}
		else
		{
			std::size_t      subscriptEnd = name.find_first_of(']', subscriptStart + 1);
			std::string_view subscript    = name.substr(subscriptStart + 1, subscriptEnd - subscriptStart - 1);
			std::size_t      index        = std::strtoull(subscript.data(), nullptr, 10);
			if (dot < name.size())
			{
				auto str = itr->second.assignStructMacro(index);
				if (!str)
					return nullptr;
				return str->assignTextMacro(name.substr(dot + 1), value);
			}
			else
			{
				return itr->second.assignTextMacro(index, value);
			}
		}
	}
}

TemplateMacro* TemplateEnvironment::assignArrayMacro(std::string_view name)
{
	std::size_t dot            = name.find_first_of('.');
	std::size_t subscriptStart = name.find_first_of('[');
	std::size_t firstNameEnd   = std::min(dot, subscriptStart);

	std::string_view firstName = name.substr(0, firstNameEnd);

	auto itr = std::find_if(m_Macros.begin(),
							m_Macros.end(),
							[firstName](const std::pair<std::string, TemplateMacro>& a) {
								return a.first == firstName;
							});
	if (itr == m_Macros.end())
	{
		if (dot < subscriptStart)
			itr = m_Macros.insert({
									  std::string { firstName },
									  TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} }
            })
					  .first;
		else if (subscriptStart < name.size())
			itr = m_Macros.insert({
									  std::string { firstName },
									  TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} }
            })
					  .first;
		else
			itr = m_Macros.insert({
									  std::string { firstName },
									  TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} }
            })
					  .first;
	}

	if (firstNameEnd >= name.size())
	{
		return &itr->second;
	}
	else
	{
		if (dot < subscriptStart)
		{
			return itr->second.assignArrayMacro(name.substr(firstNameEnd + 1));
		}
		else
		{
			std::size_t      subscriptEnd = name.find_first_of(']', subscriptStart + 1);
			std::string_view subscript    = name.substr(subscriptStart + 1, subscriptEnd - subscriptStart - 1);
			std::size_t      index        = std::strtoull(subscript.data(), nullptr, 10);
			if (dot < name.size())
			{
				auto str = itr->second.assignStructMacro(index);
				if (!str)
					return nullptr;
				return str->assignArrayMacro(name.substr(dot + 1));
			}
			else
			{
				return itr->second.assignArrayMacro(index);
			}
		}
	}
}

TemplateMacro* TemplateEnvironment::assignStructMacro(std::string_view name)
{
	std::size_t dot            = name.find_first_of('.');
	std::size_t subscriptStart = name.find_first_of('[');
	std::size_t firstNameEnd   = std::min(dot, subscriptStart);

	std::string_view firstName = name.substr(0, firstNameEnd);

	auto itr = std::find_if(m_Macros.begin(),
							m_Macros.end(),
							[firstName](const std::pair<std::string, TemplateMacro>& a) {
								return a.first == firstName;
							});
	if (itr == m_Macros.end())
	{
		if (dot < subscriptStart)
			itr = m_Macros.insert({
									  std::string { firstName },
									  TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} }
            })
					  .first;
		else if (subscriptStart < name.size())
			itr = m_Macros.insert({
									  std::string { firstName },
									  TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} }
            })
					  .first;
		else
			itr = m_Macros.insert({
									  std::string { firstName },
									  TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} }
            })
					  .first;
	}

	if (firstNameEnd >= name.size())
	{
		return &itr->second;
	}
	else
	{
		if (dot < subscriptStart)
		{
			return itr->second.assignStructMacro(name.substr(firstNameEnd + 1));
		}
		else
		{
			std::size_t      subscriptEnd = name.find_first_of(']', subscriptStart + 1);
			std::string_view subscript    = name.substr(subscriptStart + 1, subscriptEnd - subscriptStart - 1);
			std::size_t      index        = std::strtoull(subscript.data(), nullptr, 10);
			if (dot < name.size())
			{
				auto str = itr->second.assignStructMacro(index);
				if (!str)
					return nullptr;
				return str->assignStructMacro(name.substr(dot + 1));
			}
			else
			{
				return itr->second.assignStructMacro(index);
			}
		}
	}
}

TemplateMacro* TemplateEnvironment::assignRefMacro(std::string_view name, TemplateMacro& value)
{
	std::size_t dot            = name.find_first_of('.');
	std::size_t subscriptStart = name.find_first_of('[');
	std::size_t firstNameEnd   = std::min(dot, subscriptStart);

	std::string_view firstName = name.substr(0, firstNameEnd);

	auto itr = std::find_if(m_Macros.begin(),
							m_Macros.end(),
							[firstName](const std::pair<std::string, TemplateMacro>& a) {
								return a.first == firstName;
							});
	if (itr == m_Macros.end())
	{
		if (dot < subscriptStart)
			itr = m_Macros.insert({
									  std::string { firstName },
									  TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} }
            })
					  .first;
		else if (subscriptStart < name.size())
			itr = m_Macros.insert({
									  std::string { firstName },
									  TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} }
            })
					  .first;
		else
			itr = m_Macros.insert({
									  std::string { firstName },
									  TemplateMacro { ETemplateMacroType::Ref, nullptr }
            })
					  .first;
	}

	if (firstNameEnd >= name.size())
	{
		itr->second.m_Type  = ETemplateMacroType::Ref;
		itr->second.m_Value = &value;
		return &itr->second;
	}
	else
	{
		if (dot < subscriptStart)
		{
			return itr->second.assignRefMacro(name.substr(firstNameEnd + 1), value);
		}
		else
		{
			std::size_t      subscriptEnd = name.find_first_of(']', subscriptStart + 1);
			std::string_view subscript    = name.substr(subscriptStart + 1, subscriptEnd - subscriptStart - 1);
			std::size_t      index        = std::strtoull(subscript.data(), nullptr, 10);
			if (dot < name.size())
			{
				auto str = itr->second.assignStructMacro(index);
				if (!str)
					return nullptr;
				return str->assignRefMacro(name.substr(dot + 1), value);
			}
			else
			{
				return itr->second.assignRefMacro(index, value);
			}
		}
	}
}

TemplateMacro* TemplateEnvironment::getMacro(std::string_view name)
{
	std::size_t dot            = name.find_first_of('.');
	std::size_t subscriptStart = name.find_first_of('[');
	std::size_t firstNameEnd   = std::min(dot, subscriptStart);

	std::string_view firstName = name.substr(0, firstNameEnd);

	auto itr = std::find_if(m_Macros.begin(),
							m_Macros.end(),
							[firstName](const std::pair<std::string, TemplateMacro>& a) {
								return a.first == firstName;
							});
	if (itr == m_Macros.end())
		return nullptr;

	if (firstNameEnd >= name.size())
	{
		return &itr->second;
	}
	else
	{
		if (dot < subscriptStart)
		{
			if (itr->second.m_Type == ETemplateMacroType::Ref)
			{
				auto ptr = std::get<TemplateMacro*>(itr->second.m_Value);
				if (!ptr)
					return nullptr;

				if (ptr->m_Type != ETemplateMacroType::Struct)
					return nullptr;

				return ptr->get(name.substr(firstNameEnd + 1));
			}
			else
			{
				if (itr->second.m_Type != ETemplateMacroType::Struct)
					return nullptr;

				return itr->second.get(name.substr(firstNameEnd + 1));
			}
		}
		else
		{
			if (itr->second.m_Type == ETemplateMacroType::Ref)
			{
				auto ptr = std::get<TemplateMacro*>(itr->second.m_Value);
				if (!ptr)
					return nullptr;

				if (ptr->m_Type != ETemplateMacroType::Array)
					return nullptr;

				std::size_t      subscriptEnd = name.find_first_of(']', subscriptStart + 1);
				std::string_view subscript    = name.substr(subscriptStart + 1, subscriptEnd - subscriptStart - 1);
				auto             subscripted  = ptr->get(std::strtoull(subscript.data(), nullptr, 10));
				if (!subscripted)
					return nullptr;

				return dot < name.size() ? subscripted->get(name.substr(dot + 1)) : subscripted;
			}
			else
			{
				if (itr->second.m_Type != ETemplateMacroType::Array)
					return nullptr;

				std::size_t      subscriptEnd = name.find_first_of(']', subscriptStart + 1);
				std::string_view subscript    = name.substr(subscriptStart + 1, subscriptEnd - subscriptStart - 1);
				auto             subscripted  = itr->second.get(std::strtoull(subscript.data(), nullptr, 10));
				if (!subscripted)
					return nullptr;

				return dot < name.size() ? subscripted->get(name.substr(dot + 1)) : subscripted;
			}
		}
	}
}

const TemplateMacro* TemplateEnvironment::getMacro(std::string_view name) const
{
	std::size_t dot            = name.find_first_of('.');
	std::size_t subscriptStart = name.find_first_of('[');
	std::size_t firstNameEnd   = std::min(dot, subscriptStart);

	std::string_view firstName = name.substr(0, firstNameEnd);

	auto itr = std::find_if(m_Macros.begin(),
							m_Macros.end(),
							[firstName](const std::pair<std::string, TemplateMacro>& a) {
								return a.first == firstName;
							});
	if (itr == m_Macros.end())
		return nullptr;

	if (firstNameEnd >= name.size())
	{
		return &itr->second;
	}
	else
	{
		if (dot < subscriptStart)
		{
			if (itr->second.m_Type == ETemplateMacroType::Ref)
			{
				auto ptr = std::get<TemplateMacro*>(itr->second.m_Value);
				if (!ptr)
					return nullptr;

				if (ptr->m_Type != ETemplateMacroType::Struct)
					return nullptr;

				return ptr->get(name.substr(firstNameEnd + 1));
			}
			else
			{
				if (itr->second.m_Type != ETemplateMacroType::Struct)
					return nullptr;

				return itr->second.get(name.substr(firstNameEnd + 1));
			}
		}
		else
		{
			if (itr->second.m_Type == ETemplateMacroType::Ref)
			{
				auto ptr = std::get<TemplateMacro*>(itr->second.m_Value);
				if (!ptr)
					return nullptr;

				if (ptr->m_Type != ETemplateMacroType::Array)
					return nullptr;

				std::size_t      subscriptEnd = name.find_first_of(']', subscriptStart + 1);
				std::string_view subscript    = name.substr(subscriptStart + 1, subscriptEnd - subscriptStart - 1);
				auto             subscripted  = ptr->get(std::strtoull(subscript.data(), nullptr, 10));
				if (!subscripted)
					return nullptr;

				return dot < name.size() ? subscripted->get(name.substr(dot + 1)) : subscripted;
			}
			else
			{
				if (itr->second.m_Type != ETemplateMacroType::Array)
					return nullptr;

				std::size_t      subscriptEnd = name.find_first_of(']', subscriptStart + 1);
				std::string_view subscript    = name.substr(subscriptStart + 1, subscriptEnd - subscriptStart - 1);
				auto             subscripted  = itr->second.get(std::strtoull(subscript.data(), nullptr, 10));
				if (!subscripted)
					return nullptr;

				return dot < name.size() ? subscripted->get(name.substr(dot + 1)) : subscripted;
			}
		}
	}
}

void TemplateEnvironment::removeMacro(std::string_view name)
{
	std::size_t dot            = name.find_first_of('.');
	std::size_t subscriptStart = name.find_first_of('[');
	std::size_t firstNameEnd   = std::min(dot, subscriptStart);

	std::string_view firstName = name.substr(0, firstNameEnd);

	auto itr = std::find_if(m_Macros.begin(),
							m_Macros.end(),
							[firstName](const std::pair<std::string, TemplateMacro>& a) {
								return a.first == firstName;
							});
	if (itr == m_Macros.end())
		return;

	if (firstNameEnd >= name.size())
	{
		m_Macros.erase(itr);
	}
	else
	{
		if (dot < subscriptStart)
		{
			if (itr->second.m_Type == ETemplateMacroType::Ref)
			{
				auto ptr = std::get<TemplateMacro*>(itr->second.m_Value);
				if (!ptr)
					return;

				if (ptr->m_Type != ETemplateMacroType::Struct)
					return;

				ptr->remove(name.substr(firstNameEnd + 1));
			}
			else
			{
				if (itr->second.m_Type != ETemplateMacroType::Struct)
					return;

				itr->second.remove(name.substr(firstNameEnd + 1));
			}
		}
		else
		{
			if (itr->second.m_Type == ETemplateMacroType::Ref)
			{
				auto ptr = std::get<TemplateMacro*>(itr->second.m_Value);
				if (!ptr)
					return;

				if (ptr->m_Type != ETemplateMacroType::Array)
					return;

				std::size_t      subscriptEnd = name.find_first_of(']', subscriptStart + 1);
				std::string_view subscript    = name.substr(subscriptStart + 1, subscriptEnd - subscriptStart - 1);
				std::size_t      index        = std::strtoull(subscript.data(), nullptr, 10);

				if (dot < name.size())
				{
					auto subscripted = ptr->get(index);
					if (!subscripted)
						return;
					subscripted->remove(name.substr(dot + 1));
				}
				else
				{
					ptr->remove(index);
				}
			}
			else
			{
				if (itr->second.m_Type != ETemplateMacroType::Array)
					return;

				std::size_t      subscriptEnd = name.find_first_of(']', subscriptStart + 1);
				std::string_view subscript    = name.substr(subscriptStart + 1, subscriptEnd - subscriptStart - 1);
				std::size_t      index        = std::strtoull(subscript.data(), nullptr, 10);

				if (dot < name.size())
				{
					auto subscripted = itr->second.get(index);
					if (!subscripted)
						return;
					subscripted->remove(name.substr(dot + 1));
				}
				else
				{
					itr->second.remove(index);
				}
			}
		}
	}
}

void TemplateEnvironment::pushScope(std::size_t scopeIndex)
{
	if (scopeIndex == ~0ULL)
	{
		std::cerr << "TemplateEngine broke with new scope '" << scopeIndex << "'!!\n";
		return;
	}

	const TemplateScope& scope = m_Template->m_Scopes[scopeIndex];
	m_CallStack.emplace_back(TemplateCallStackEntry {
		&m_Template->m_Calls[scope.m_OpenCall],
		&m_Template->m_Calls[scope.m_CloseCall],
		&scope,
		scopeIndex,
		nullptr });
	m_CurrentScopeIndex = scopeIndex;
	m_CurrentLastCall   = scope.m_CloseCall;
}

void TemplateEnvironment::popScope()
{
	m_CallStack.pop_back();
	if (m_CallStack.size() <= 1)
	{
		m_CurrentLastCall   = m_Template->m_Calls.size();
		m_CurrentScopeIndex = ~0ULL;
	}
	else
	{
		m_CurrentLastCall   = m_CallStack.back().m_Scope->m_CloseCall;
		m_CurrentScopeIndex = m_CallStack.back().m_ScopeIndex;
	}
}

TemplateEngine::TemplateEngine()
{
	addFunction("Insert", { &Insert });
	addFunction("Assign", { &Assign });
	addFunction("Unassign", { &Unassign });

	addFunction("Append", { &Append });
	addFunction("PushFront", { &PushFront });
	addFunction("PushBack", { &PushBack });
	addFunction("PopFront", { &PopFront });
	addFunction("PopBack", { &PopBack });

	addFunction("Cast", { &Cast });

	addFunction("Function", { &Function, "End" });
	addFunction("Return", { &Return });

	addFunction("For", { &For, "End" });
	addFunction("Foreach", { &Foreach, "End" });
	addFunction("Break", { &Break });
	addFunction("Continue", { &Continue });

	std::vector<std::string> ifStatements { "If", "Ifn", "Ifeq", "Ifneq" };
	std::vector<std::string> elifStatements { "Else", "Elif", "Elifn", "Elifeq", "Elifneq" };
	for (auto& ifStatement : ifStatements)
		addFunction(ifStatement, { &If, "End", elifStatements });

	addFunction("Add", { &Add });

	addFunction("LoadObject", { &LoadObject });
	addFunction("Template", { &CallTemplate });
	addFunction("Dofile", { &Dofile });
}

std::string TemplateEngine::executeTemplate(std::string_view name, TemplateEnvironment& environment)
{
	compileTemplate(name);
	updateScopesAndFunctionLUT(name);

	auto itr = std::find_if(m_Templates.begin(),
							m_Templates.end(),
							[name](const std::pair<std::string, Template>& a) {
								return a.first == name;
							});
	if (itr == m_Templates.end())
		return {};

	environment.m_Engine   = this;
	environment.m_Template = &itr->second;
	environment.m_CallStack.clear();
	environment.m_CWD    = name.substr(0, name.find_last_of('/'));
	environment.m_Source = environment.m_Template->m_Text;
	environment.m_CallStack.emplace_back(TemplateCallStackEntry { &environment.m_Template->m_Calls[0], nullptr, nullptr, ~0ULL, nullptr });
	environment.m_CurrentLastCall = environment.m_Template->m_Calls.size();

	while (!environment.m_CallStack.empty())
	{
		while (environment.m_CurrentCall < environment.m_CurrentLastCall)
		{
			environment.m_IncrementCall = true;

			auto& call = environment.m_Template->m_Calls[environment.m_CurrentCall];
			if (call.m_FunctionPtr)
			{
				if (call.m_ScopeIndex != environment.m_CurrentScopeIndex)
					environment.pushScope(call.m_ScopeIndex);

				call.m_FunctionPtr->m_Callback(environment, call);
			}

			if (environment.m_IncrementCall)
				++environment.m_CurrentCall;
		}

		if (environment.m_CurrentCall < environment.m_Template->m_Calls.size())
		{
			auto& back = environment.m_CallStack.back();
			if (back.m_EndCall)
			{
				environment.m_IncrementCall = true;
				environment.m_PopScope      = true;
				back.m_StartCall->m_FunctionPtr->m_Callback(environment, *back.m_EndCall);

				if (environment.m_PopScope)
					environment.popScope();

				if (environment.m_IncrementCall)
					++environment.m_CurrentCall;
			}
			else
			{
				environment.popScope();
			}
		}
		else
		{
			break;
		}
	}

	return std::move(environment.m_Source);
}

void TemplateEngine::addFunction(std::string_view name, TemplateFunction function)
{
	auto itr = std::find_if(m_Functions.begin(),
							m_Functions.end(),
							[name](const std::pair<std::string, TemplateFunction>& a) {
								return a.first == name;
							});
	if (itr != m_Functions.end())
		return;

	m_Functions.insert({ std::string { name }, std::move(function) });

	if (m_UpdateFunctionVersion)
	{
		++m_FunctionVersion;
		m_UpdateFunctionVersion = false;
	}
}

void TemplateEngine::removeFunction(std::string_view name)
{
	auto itr = std::find_if(m_Functions.begin(),
							m_Functions.end(),
							[name](const std::pair<std::string, TemplateFunction>& a) {
								return a.first == name;
							});
	if (itr != m_Functions.end())
		m_Functions.erase(itr);

	if (m_UpdateFunctionVersion)
	{
		++m_FunctionVersion;
		m_UpdateFunctionVersion = false;
	}
}

TemplateFunction* TemplateEngine::getFunction(std::string_view name)
{
	auto itr = std::find_if(m_Functions.begin(),
							m_Functions.end(),
							[name](const std::pair<std::string, TemplateFunction>& a) {
								return a.first == name;
							});

	return itr != m_Functions.end() ? &itr->second : nullptr;
}

void TemplateEngine::compileTemplate(std::string_view name)
{
	auto itr = std::find_if(m_Templates.begin(),
							m_Templates.end(),
							[name](const std::pair<std::string, Template>& a) {
								return a.first == name;
							});
	if (itr != m_Templates.end())
		return;

	std::ifstream file { std::filesystem::path { name }, std::ios::ate };
	if (file)
	{
		std::string source(file.tellg(), '\0');
		file.seekg(0);
		file.read(source.data(), source.size());
		file.close();

		{
			std::size_t end = source.find_last_not_of('\0');
			source.resize(end + 1);
		}

		std::vector<TemplateCall> calls;

		std::size_t offset = 0;
		if (source.size() >= (offset + 2) && source[offset] == '#' && source[offset + 1] == '!')
		{
			// Might be options
			std::size_t end          = source.find_first_of('\n', offset + 2);
			char        commandChar  = '$';
			char        openRefChar  = '<';
			char        closeRefChar = '<';
			char        escapeChar   = '\\';
			if (end > offset + 2)
				commandChar = source[offset + 2];
			if (end > offset + 3)
				openRefChar = source[offset + 3];
			if (end > offset + 4)
				closeRefChar = source[offset + 4];
			if (end > offset + 5)
				escapeChar = source[offset + 5];
			setChars(commandChar, openRefChar, closeRefChar, escapeChar);
			source.erase(offset, end - offset + 1);
		}

		while (offset < source.size())
		{
			std::size_t   callStart = offset;
			std::uint32_t state     = 0;
			for (; state < 2 && callStart < source.size(); ++callStart)
			{
				if (source[callStart] == '\\')
					++callStart;
				else if (source[callStart] == m_CommandChar)
					++state;
			}
			if (callStart >= source.size())
				break;

			state = 0;

			std::size_t   callEnd = callStart + 2;
			std::uint32_t depth   = 0;
			for (; state < 3 && callEnd < source.size(); ++callEnd)
			{
				if (source[callEnd] == '\\')
				{
					++callEnd;
					state = 0;
					continue;
				}

				switch (state)
				{
				case 0:
					if (source[callEnd] == m_CommandChar)
						state = 1;
					else if (source[callEnd] == '>')
						state = 2;
					break;
				case 1:
					if (depth == 0 && source[callEnd] == m_CommandChar)
					{
						state = 3;
					}
					else if (source[callEnd] == '<')
					{
						++depth;
						state = 0;
					}
					else
					{
						state = 0;
					}
					break;
				case 2:
					if (source[callEnd] == m_CommandChar)
					{
						--depth;
						state = 0;
					}
					else
					{
						state = 0;
					}
					break;
				}
			}
			if (callEnd >= source.size())
			{
				std::cerr << "Call started at " << callStart << " did not end, add '" << m_CommandChar << m_CommandChar << "' at the end of the call!\n";
				break;
			}
			// std::cout << source.substr(callStart, (callEnd - 2) - callStart) << '\n';

			std::size_t callNameStart = callStart;

			std::size_t callNameEnd = callNameStart;
			for (; callNameEnd < callEnd - 2; ++callNameEnd)
			{
				if (source[callNameEnd] == '\\')
					++callNameEnd;
				else if (source[callNameEnd] == ':')
					break;
			}

			std::vector<std::string> arguments;

			std::size_t argumentsStart = callNameEnd + 1;
			std::size_t argumentsEnd   = callEnd - 2;
			std::size_t argumentOffset = argumentsStart;
			while (argumentOffset < argumentsEnd)
			{
				std::size_t argumentStart = argumentOffset;
				std::size_t argumentEnd   = argumentStart;
				for (; argumentEnd < argumentsEnd; ++argumentEnd)
				{
					if (source[argumentEnd] == '\\')
						++argumentEnd;
					else if (source[argumentEnd] == ',')
						break;
				}
				if (argumentEnd == argumentStart)
					break;

				arguments.emplace_back(source.substr(argumentStart, argumentEnd - argumentStart));
				argumentOffset = argumentEnd + 1;
			}

			std::size_t sourceOffset = callStart >= 3 ? callStart - 3 : 0;
			bool        failed       = false;
			bool        end          = false;
			bool        hasNewline   = false;
			std::size_t spaces       = 0;
			for (; !failed && !end && sourceOffset > 0; --sourceOffset)
			{
				switch (source[sourceOffset])
				{
				case ' ':
					++spaces;
					break;
				case '\t':
					spaces += 4;
					break;
				case '\n':
					hasNewline = true;
					end        = true;
					break;
				default:
					sourceOffset = callStart - 2;
					failed       = true;
					hasNewline   = false;
					spaces       = 0;
					break;
				}
				if (end || failed)
					break;
			}
			std::size_t sourceEnd = callEnd;
			for (; !failed && !end && sourceEnd < source.size(); ++sourceEnd)
			{
				switch (source[sourceEnd])
				{
				case ' ':
				case '\t':
					break;
				case '\n':
					end = true;
					break;
				default:
					sourceOffset = callStart - 2;
					sourceEnd    = callEnd;
					failed       = true;
					hasNewline   = false;
					spaces       = 0;
					break;
				}
				if (failed)
					break;
			}

			calls.emplace_back(TemplateCall { source.substr(callNameStart, callNameEnd - callNameStart), nullptr, std::move(arguments), sourceOffset, hasNewline, spaces });
			source.erase(sourceOffset, sourceEnd - sourceOffset);
			offset = sourceOffset;
		}
		m_Templates.insert({
			std::string { name },
			Template { std::move(source), std::move(calls) }
        });
	}
	else
	{
		std::cerr << "Could not find template file '" << name << "'!\n";
	}
}

void TemplateEngine::updateScopesAndFunctionLUT(std::string_view name)
{
	auto itr = std::find_if(m_Templates.begin(),
							m_Templates.end(),
							[name](const std::pair<std::string, Template>& a) {
								return a.first == name;
							});
	if (itr == m_Templates.end() || itr->second.m_LUTVersion == m_FunctionVersion)
		return;

	struct ScopeStackEntry
	{
		std::size_t       scopeIndex;
		std::size_t       start;
		TemplateFunction* stackStart;
	};

	std::vector<ScopeStackEntry> scopeStack;

	std::size_t currentScope      = ~0ULL;
	std::size_t currentFirstScope = ~0ULL;
	std::size_t currentCall       = 0;

	for (; currentCall < itr->second.m_Calls.size(); ++currentCall)
	{
		bool  canCreateScope = true;
		auto& call           = itr->second.m_Calls[currentCall];
		auto  function       = getFunction(call.m_Function);
		if (!scopeStack.empty())
		{
			auto& back = scopeStack.back();
			if (back.stackStart->m_LastCall == call.m_Function)
			{
				// End scope
				call.m_FunctionPtr                             = back.stackStart;
				canCreateScope                                 = false;
				itr->second.m_Scopes[currentScope].m_CloseCall = currentCall;
				if (currentFirstScope != ~0ULL && currentFirstScope != currentScope)
				{
					auto* scope = &itr->second.m_Scopes[currentFirstScope];
					while (scope->m_NextScope != ~0ULL)
					{
						scope->m_LastScope = currentScope;

						scope = &itr->second.m_Scopes[scope->m_NextScope];
					}
				}
				call.m_ScopeIndex = currentScope;
				scopeStack.pop_back();
				if (scopeStack.empty())
				{
					currentScope      = ~0ULL;
					currentFirstScope = ~0ULL;
				}
				else
				{
					currentScope      = scopeStack.back().scopeIndex;
					currentFirstScope = scopeStack.back().start;
				}
			}
			else
			{
				for (auto& interCall : back.stackStart->m_InterCalls)
				{
					if (interCall == call.m_Function)
					{
						// End scope
						auto scopeFunction                             = back.stackStart;
						call.m_FunctionPtr                             = scopeFunction;
						canCreateScope                                 = false;
						itr->second.m_Scopes[currentScope].m_CloseCall = currentCall;
						itr->second.m_Scopes[currentScope].m_NextScope = itr->second.m_Scopes.size();
						std::size_t previousScope                      = currentScope;
						scopeStack.pop_back();

						// Create new scope
						scopeStack.emplace_back(ScopeStackEntry { itr->second.m_Scopes.size(), currentFirstScope, scopeFunction });
						currentScope = itr->second.m_Scopes.size();
						itr->second.m_Scopes.emplace_back(TemplateScope { currentCall, ~0ULL, previousScope, ~0ULL, currentFirstScope, ~0ULL });
						call.m_ScopeIndex = currentScope;
						break;
					}
				}
			}
		}

		if (canCreateScope)
		{
			call.m_FunctionPtr = function;
			if (function)
			{
				if (function->hasLastCall())
				{
					// Create new scope
					currentScope = itr->second.m_Scopes.size();
					scopeStack.emplace_back(ScopeStackEntry { itr->second.m_Scopes.size(), currentScope, function });
					itr->second.m_Scopes.emplace_back(TemplateScope { currentCall, ~0ULL, ~0ULL, ~0ULL, ~0ULL, ~0ULL });
					currentFirstScope = currentScope;
				}
			}
			else
			{
				std::cerr << "Trying to use unknown function '" << call.m_Function << "', skipping said function\n";
			}

			call.m_ScopeIndex = currentScope;
		}
	}

	itr->second.m_LUTVersion = m_FunctionVersion;
}

void TemplateEngine::Insert(TemplateEnvironment& environment, const TemplateCall& call)
{
	auto macro = environment.getMacro(call.m_Arguments[0]);
	if (!macro)
		return;

	switch (macro->m_Type)
	{
	case ETemplateMacroType::Null:
		environment.insertText("null");
		break;
	case ETemplateMacroType::Bool:
		environment.insertText(std::get<bool>(macro->m_Value) ? "true" : "false");
		break;
	case ETemplateMacroType::Int:
		environment.insertText(std::to_string(std::get<std::int64_t>(macro->m_Value)), true);
		break;
	case ETemplateMacroType::UInt:
		environment.insertText(std::to_string(std::get<std::uint64_t>(macro->m_Value)), true);
		break;
	case ETemplateMacroType::Float:
		environment.insertText(std::to_string(std::get<double>(macro->m_Value)), true);
		break;
	case ETemplateMacroType::Text:
		environment.insertText(std::get<std::string>(macro->m_Value), true);
		break;
	case ETemplateMacroType::Ref:
	{
		auto ptr = std::get<TemplateMacro*>(macro->m_Value);
		switch (ptr->m_Type)
		{
		case ETemplateMacroType::Null:
			environment.insertText("null");
			break;
		case ETemplateMacroType::Bool:
			environment.insertText(std::get<bool>(ptr->m_Value) ? "true" : "false");
			break;
		case ETemplateMacroType::Int:
			environment.insertText(std::to_string(std::get<std::int64_t>(ptr->m_Value)), true);
			break;
		case ETemplateMacroType::UInt:
			environment.insertText(std::to_string(std::get<std::uint64_t>(ptr->m_Value)), true);
			break;
		case ETemplateMacroType::Float:
			environment.insertText(std::to_string(std::get<double>(ptr->m_Value)), true);
			break;
		case ETemplateMacroType::Text:
			environment.insertText(std::get<std::string>(ptr->m_Value), true);
			break;
		}
		break;
	}
	}
}

void TemplateEngine::Assign(TemplateEnvironment& environment, const TemplateCall& call)
{
	if (call.m_Arguments.size() > 1)
	{
		std::string str = call.m_Arguments[1];
		environment.resolveReferences(str);
		EscapeString(str, environment.m_Engine->m_EscapeCharacter);
		auto type = TemplateMacro::GetBestType(str);
		switch (type)
		{
		case ETemplateMacroType::Null:
			environment.assignNullMacro(call.m_Arguments[0]);
			break;
		case ETemplateMacroType::Bool:
			environment.assignBoolMacro(call.m_Arguments[0], str == "true");
			break;
		case ETemplateMacroType::Int:
			environment.assignIntMacro(call.m_Arguments[0], std::strtoll(str.data(), nullptr, 10));
			break;
		case ETemplateMacroType::UInt:
			environment.assignUIntMacro(call.m_Arguments[0], std::strtoull(str.data(), nullptr, 10));
			break;
		case ETemplateMacroType::Float:
			environment.assignFloatMacro(call.m_Arguments[0], std::strtod(str.data(), nullptr));
			break;
		case ETemplateMacroType::Text:
			environment.assignTextMacro(call.m_Arguments[0], str);
			break;
		}
	}
	else if (call.m_Arguments.size() == 1)
	{
		environment.assignTextMacro(call.m_Arguments[0], "");
	}
}

void TemplateEngine::Unassign(TemplateEnvironment& environment, const TemplateCall& call)
{
	environment.removeMacro(call.m_Arguments[0]);
}

void TemplateEngine::Append(TemplateEnvironment& environment, const TemplateCall& call)
{
	if (call.m_Arguments.size() < 2)
	{
		std::cerr << "'Append' requires 2 arguments!!\n";
		return;
	}

	auto macro = environment.getMacro(call.m_Arguments[0]);
	if (!macro)
		macro = environment.assignTextMacro(call.m_Arguments[0], "");

	if (macro->m_Type != ETemplateMacroType::Text)
	{
		std::cerr << "Macro '" << call.m_Arguments[0] << "' is not a text macro!!\n";
		return;
	}

	std::string str = call.m_Arguments[1];
	environment.resolveReferences(str);
	EscapeString(str, environment.m_Engine->m_EscapeCharacter);
	std::get<std::string>(macro->m_Value) += str;
}

void TemplateEngine::PushFront(TemplateEnvironment& environment, const TemplateCall& call)
{
	auto macro2 = environment.getMacro(call.m_Arguments[1]);
	if (!macro2)
	{
		std::cerr << "'" << call.m_Arguments[1] << "' is not an assigned macro\n";
		return;
	}

	auto macro = environment.getMacro(call.m_Arguments[0]);
	if (!macro)
		macro = environment.assignArrayMacro(call.m_Arguments[0]);

	macro->pushFront(*macro2);
}

void TemplateEngine::PushBack(TemplateEnvironment& environment, const TemplateCall& call)
{
	auto macro2 = environment.getMacro(call.m_Arguments[1]);
	if (!macro2)
	{
		std::cerr << "'" << call.m_Arguments[1] << "' is not an assigned macro\n";
		return;
	}

	auto macro = environment.getMacro(call.m_Arguments[0]);
	if (!macro)
		macro = environment.assignArrayMacro(call.m_Arguments[0]);

	macro->pushBack(*macro2);
}

void TemplateEngine::PopFront(TemplateEnvironment& environment, const TemplateCall& call)
{
	auto macro = environment.getMacro(call.m_Arguments[0]);
	if (!macro)
	{
		std::cerr << "'" << call.m_Arguments[0] << "' is not an assigned macro\n";
		return;
	}

	macro->popFront();
}

void TemplateEngine::PopBack(TemplateEnvironment& environment, const TemplateCall& call)
{
	auto macro = environment.getMacro(call.m_Arguments[0]);
	if (!macro)
	{
		std::cerr << "'" << call.m_Arguments[0] << "' is not an assigned macro\n";
		return;
	}

	macro->popBack();
}

void TemplateEngine::Cast(TemplateEnvironment& environment, const TemplateCall& call)
{
	// src, dst, new type, args...

	auto macro = environment.getMacro(call.m_Arguments[0]);
	if (!macro)
	{
		std::cerr << "'" << call.m_Arguments[0] << "' is not an assigned macro\n";
		return;
	}

	auto& newType = call.m_Arguments[2];
	if (newType == "hexstr")
	{
		std::int64_t maxWidthS = std::strtoll(call.m_Arguments[3].data(), nullptr, 10);
		if (maxWidthS < 0 || maxWidthS > 16)
		{
			std::cerr << "Casting int to hex less than 0 or wider than 16 is illegal!\n";
			return;
		}
		std::uint64_t maxWidth = static_cast<std::uint64_t>(maxWidthS);

		static constexpr char s_HexDigits[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

		char* buf       = new char[maxWidth + 1];
		buf[maxWidth]   = '\0';
		std::uint64_t v = 0;
		switch (macro->m_Type)
		{
		case ETemplateMacroType::Int:
			v = static_cast<std::uint64_t>(std::get<std::int64_t>(macro->m_Value));
			break;
		case ETemplateMacroType::UInt:
			v = std::get<std::uint64_t>(macro->m_Value);
			break;
		}

		for (std::size_t i = 0; i < maxWidth; ++i)
			buf[i] = s_HexDigits[(v >> ((maxWidth - 1 - i) * 8)) & 0xF];
		environment.assignTextMacro(call.m_Arguments[1], buf);
		delete[] buf;
	}
	else
	{
		std::cerr << "Unknown cast type '" << newType << "'\n";
	}
}

void TemplateEngine::For(TemplateEnvironment& environment, const TemplateCall& call)
{
	struct ForData
	{
		std::string  m_VariableName;
		std::int64_t m_CurrentValue;
		std::int64_t m_EndValue;
		std::int64_t m_Increment;
		std::string  m_SourceText;
	};

	if (call.m_Function == "End")
	{
		auto entry = environment.currentStack();
		if (!entry || !entry->m_UserData)
		{
			std::cerr << "'End' was hit, but stack was broken\n";
			return;
		}
		ForData* forData        = reinterpret_cast<ForData*>(entry->m_UserData);
		forData->m_CurrentValue += forData->m_Increment;
		if (forData->m_CurrentValue != forData->m_EndValue)
		{
			environment.assignIntMacro(forData->m_VariableName, forData->m_CurrentValue);
			environment.insertText(forData->m_SourceText);
			environment.jumpToStartOfScope();
			return;
		}
		delete forData;
	}
	else
	{
		ForData* forData = new ForData {
			call.m_Arguments[0],
			std::strtoll(call.m_Arguments[1].data(), nullptr, 10),
			std::strtoll(call.m_Arguments[2].data(), nullptr, 10),
			std::strtoll(call.m_Arguments[3].data(), nullptr, 10),
			{}
		};
		environment.setStackUserData(forData);
		if (forData->m_CurrentValue == forData->m_EndValue)
		{
			environment.eraseRange();
			environment.exitScope();
			delete forData;
			return;
		}

		forData->m_SourceText = environment.getSourceRange();
		environment.assignIntMacro(forData->m_VariableName, forData->m_CurrentValue);
	}
}

void TemplateEngine::Foreach(TemplateEnvironment& environment, const TemplateCall& call)
{
	enum class EForeachType : std::uint32_t
	{
		Array,
		Struct
	};

	struct ForeachData
	{
		struct ArrayIterator
		{
			std::vector<TemplateMacro>::iterator m_Current, m_End;
		};

		struct StructIterator
		{
			std::unordered_map<std::string, TemplateMacro>::iterator m_Current, m_End;
		};

		std::string  m_VariableName;
		EForeachType m_Type;
		std::string  m_SourceText;

		std::variant<ArrayIterator, StructIterator> m_Iterator;
	};

	if (call.m_Function == "End")
	{
		auto entry = environment.currentStack();
		if (!entry || !entry->m_UserData)
		{
			std::cerr << "'End' was hit, but stack was broken\n";
			return;
		}
		ForeachData* foreachData = reinterpret_cast<ForeachData*>(entry->m_UserData);
		switch (foreachData->m_Type)
		{
		case EForeachType::Array:
		{
			auto& arr = std::get<ForeachData::ArrayIterator>(foreachData->m_Iterator);
			++arr.m_Current;
			if (arr.m_Current != arr.m_End)
			{
				environment.assignRefMacro(foreachData->m_VariableName, *arr.m_Current);
				environment.insertText(foreachData->m_SourceText);
				environment.jumpToStartOfScope();
				return;
			}
			break;
		}
		case EForeachType::Struct:
		{
			auto& map = std::get<ForeachData::StructIterator>(foreachData->m_Iterator);
			++map.m_Current;
			if (map.m_Current != map.m_End)
			{
				environment.assignRefMacro(foreachData->m_VariableName, map.m_Current->second);
				environment.insertText(foreachData->m_SourceText);
				environment.jumpToStartOfScope();
				return;
			}
			break;
		}
		}
		delete foreachData;
	}
	else
	{
		auto           macro        = environment.getMacro(call.m_Arguments[1]);
		ForeachData*   foreachData  = nullptr;
		TemplateMacro* currentValue = nullptr;
		if (macro)
		{
			if (macro->m_Type == ETemplateMacroType::Array)
			{
				auto& arr   = std::get<std::vector<TemplateMacro>>(macro->m_Value);
				foreachData = new ForeachData {
					call.m_Arguments[0],
					EForeachType::Array,
					{},
					ForeachData::ArrayIterator {
							 arr.begin(),
							 arr.end() }
				};
				currentValue = &*arr.begin();
			}
			else if (macro->m_Type == ETemplateMacroType::Struct)
			{
				auto& map   = std::get<std::unordered_map<std::string, TemplateMacro>>(macro->m_Value);
				foreachData = new ForeachData {
					call.m_Arguments[0],
					EForeachType::Struct,
					{},
					ForeachData::StructIterator {
							  map.begin(),
							  map.end() }
				};
				currentValue = &map.begin()->second;
			}
		}

		environment.setStackUserData(foreachData);
		if (!macro)
		{
			std::cerr << "'" << call.m_Arguments[1] << "' is not an assigned macro\n";
			environment.eraseRange();
			environment.exitScope();
			return;
		}

		if (!foreachData)
		{
			std::cerr << "'" << call.m_Arguments[1] << "' is not an iterable macro\n";
			environment.eraseRange();
			environment.exitScope();
			return;
		}

		if (!currentValue)
		{
			delete foreachData;
			environment.eraseRange();
			environment.exitScope();
			return;
		}

		foreachData->m_SourceText = environment.getSourceRange();
		environment.assignRefMacro(foreachData->m_VariableName, *currentValue);
	}
}

void TemplateEngine::If(TemplateEnvironment& environment, const TemplateCall& call)
{
	struct IfData
	{
		bool m_Handled;
	};

	bool enter = false;

	IfData* ifData;
	if (call.m_Function == "End")
	{
		auto entry = environment.currentStack();
		if (!entry || !entry->m_UserData)
		{
			std::cerr << "'" << call.m_Function << "' was hit, but stack was broken\n";
			environment.exitScope();
			return;
		}
		ifData = reinterpret_cast<IfData*>(entry->m_UserData);
		environment.exitScope();
		delete ifData;
		return;
	}
	else if (call.m_Function == "Else" ||
			 call.m_Function == "Elif" ||
			 call.m_Function == "Elifn" ||
			 call.m_Function == "Elifeq" ||
			 call.m_Function == "Elfneq")
	{
		auto entry = environment.currentStack();
		if (!entry || !entry->m_UserData)
		{
			std::cerr << "'" << call.m_Function << "' was hit, but stack was broken\n";
			environment.exitScopes();
			return;
		}
		ifData = reinterpret_cast<IfData*>(entry->m_UserData);

		if (ifData->m_Handled)
		{
			// Jump to last scope end
			environment.exitScopes();
			delete ifData;
			return;
		}
	}
	else
	{
		ifData = new IfData { false };
		environment.setStackUserData(ifData);
	}

	if (call.m_Function == "If" ||
		call.m_Function == "Elif")
	{
		// Checks if argument is non zero (defined, non empty, non '0' int)
		auto macro = environment.getMacro(call.m_Arguments[0]);
		if (macro)
		{
			if (macro->m_Type == ETemplateMacroType::Ref)
				macro = std::get<TemplateMacro*>(macro->m_Value);

			switch (macro->m_Type)
			{
			case ETemplateMacroType::Null:
				enter = false;
				break;
			case ETemplateMacroType::Bool:
				enter = std::get<bool>(macro->m_Value);
				break;
			case ETemplateMacroType::Int:
				enter = std::get<std::int64_t>(macro->m_Value) != 0;
				break;
			case ETemplateMacroType::UInt:
				enter = std::get<std::uint64_t>(macro->m_Value) != 0;
				break;
			case ETemplateMacroType::Float:
				enter = std::get<double>(macro->m_Value) != 0.0;
				break;
			case ETemplateMacroType::Text:
				enter = !std::get<std::string>(macro->m_Value).empty();
				break;
			case ETemplateMacroType::Array:
				enter = !std::get<std::vector<TemplateMacro>>(macro->m_Value).empty();
				break;
			case ETemplateMacroType::Struct:
				enter = !std::get<std::unordered_map<std::string, TemplateMacro>>(macro->m_Value).empty();
				break;
			}
		}
	}
	else if (call.m_Function == "Ifn" ||
			 call.m_Function == "Elifn")
	{
		// Checks if argument is zero (not defined, empty, '0' int)
		auto macro = environment.getMacro(call.m_Arguments[0]);
		if (macro)
		{
			if (macro->m_Type == ETemplateMacroType::Ref)
				macro = std::get<TemplateMacro*>(macro->m_Value);

			switch (macro->m_Type)
			{
			case ETemplateMacroType::Null:
				enter = false;
				break;
			case ETemplateMacroType::Bool:
				enter = std::get<bool>(macro->m_Value);
				break;
			case ETemplateMacroType::Int:
				enter = std::get<std::int64_t>(macro->m_Value) == 0;
				break;
			case ETemplateMacroType::UInt:
				enter = std::get<std::uint64_t>(macro->m_Value) == 0;
				break;
			case ETemplateMacroType::Float:
				enter = std::get<double>(macro->m_Value) != 0.0;
				break;
			case ETemplateMacroType::Text:
				enter = std::get<std::string>(macro->m_Value).empty();
				break;
			case ETemplateMacroType::Array:
				enter = std::get<std::vector<TemplateMacro>>(macro->m_Value).empty();
				break;
			case ETemplateMacroType::Struct:
				enter = std::get<std::unordered_map<std::string, TemplateMacro>>(macro->m_Value).empty();
				break;
			}
		}
		else
		{
			enter = true;
		}
	}
	else if (call.m_Function == "Ifeq" ||
			 call.m_Function == "Elifeq")
	{
		auto macro = environment.getMacro(call.m_Arguments[0]);
		if (macro)
		{
			if (macro->m_Type == ETemplateMacroType::Ref)
				macro = std::get<TemplateMacro*>(macro->m_Value);

			auto& text = call.m_Arguments[1];
			switch (macro->m_Type)
			{
			case ETemplateMacroType::Null:
				enter = false;
				break;
			case ETemplateMacroType::Bool:
				enter = std::get<bool>(macro->m_Value) == (text == "true");
				break;
			case ETemplateMacroType::Int:
				enter = std::get<std::int64_t>(macro->m_Value) == std::strtoll(text.data(), nullptr, 10);
				break;
			case ETemplateMacroType::UInt:
				enter = std::get<std::uint64_t>(macro->m_Value) == std::strtoull(text.data(), nullptr, 10);
				break;
			case ETemplateMacroType::Float:
				enter = std::get<double>(macro->m_Value) == std::strtod(text.data(), nullptr);
				break;
			case ETemplateMacroType::Text:
				enter = std::get<std::string>(macro->m_Value) == text;
				break;
			}
		}
	}
	else if (call.m_Function == "Ifeqn" ||
			 call.m_Function == "Elifeqn")
	{
		auto macro = environment.getMacro(call.m_Arguments[0]);
		if (macro)
		{
			if (macro->m_Type == ETemplateMacroType::Ref)
				macro = std::get<TemplateMacro*>(macro->m_Value);

			auto& text = call.m_Arguments[1];
			switch (macro->m_Type)
			{
			case ETemplateMacroType::Null:
				enter = false;
				break;
			case ETemplateMacroType::Bool:
				enter = std::get<bool>(macro->m_Value) != (text == "true");
				break;
			case ETemplateMacroType::Int:
				enter = std::get<std::int64_t>(macro->m_Value) != std::strtoll(text.data(), nullptr, 10);
				break;
			case ETemplateMacroType::UInt:
				enter = std::get<std::uint64_t>(macro->m_Value) != std::strtoull(text.data(), nullptr, 10);
				break;
			case ETemplateMacroType::Float:
				enter = std::get<double>(macro->m_Value) != std::strtod(text.data(), nullptr);
				break;
			case ETemplateMacroType::Text:
				enter = std::get<std::string>(macro->m_Value) != text;
				break;
			}
		}
		else
		{
			enter = true;
		}
	}
	else if (call.m_Function == "Else")
	{
		enter = true;
	}
	else
	{
		std::cerr << "WTF ARE YOU DOING!\n";
		return;
	}

	if (enter)
		ifData->m_Handled = true;
	else
		environment.nextScope();
}

void TemplateEngine::Add(TemplateEnvironment& environment, const TemplateCall& call)
{
	if (call.m_Arguments.size() < 2)
	{
		std::cerr << "'Add' requires 2 arguments!!\n";
		return;
	}

	auto macro = environment.getMacro(call.m_Arguments[0]);
	if (!macro)
		macro = environment.assignIntMacro(call.m_Arguments[0], 0);

	if (macro->m_Type != ETemplateMacroType::Int &&
		macro->m_Type != ETemplateMacroType::UInt &&
		macro->m_Type != ETemplateMacroType::Float)
	{
		std::cerr << "Macro '" << call.m_Arguments[0] << "' is not a numeric macro!!\n";
		return;
	}

	std::string str = call.m_Arguments[1];
	environment.resolveReferences(str);
	switch (macro->m_Type)
	{
	case ETemplateMacroType::Int:
		std::get<std::int64_t>(macro->m_Value) += std::strtoll(str.data(), nullptr, 10);
		break;
	case ETemplateMacroType::UInt:
		std::get<std::uint64_t>(macro->m_Value) += std::strtoull(str.data(), nullptr, 10);
		break;
	case ETemplateMacroType::Float:
		std::get<double>(macro->m_Value) += std::strtod(str.data(), nullptr);
		break;
	}
}

void AddJSONArrayValues(simdjson::ondemand::array arr, TemplateMacro* macro);
void AddJSONObjectValues(simdjson::ondemand::object obj, TemplateMacro* macro);

void AddJSONArrayValues(simdjson::ondemand::array arr, TemplateMacro* macro)
{
	std::size_t index = 0;
	for (auto value : arr)
	{
		simdjson::ondemand::json_type type = value.type();
		switch (type)
		{
		case simdjson::ondemand::json_type::array:
		{
			auto amacro = macro->assignArrayMacro(index);
			AddJSONArrayValues(value.get_array(), amacro);
			break;
		}
		case simdjson::ondemand::json_type::object:
		{
			auto smacro = macro->assignStructMacro(index);
			AddJSONObjectValues(value.get_object(), smacro);
			break;
		}
		case simdjson::ondemand::json_type::number:
		{
			simdjson::ondemand::number_type numberType = value.get_number_type();
			switch (numberType)
			{
			case simdjson::ondemand::number_type::floating_point_number:
				macro->assignFloatMacro(index, value.get_double());
				break;
			case simdjson::ondemand::number_type::signed_integer:
				macro->assignIntMacro(index, value.get_int64());
				break;
			case simdjson::ondemand::number_type::unsigned_integer:
				macro->assignUIntMacro(index, value.get_uint64());
				break;
			}
			break;
		}
		case simdjson::ondemand::json_type::string:
		{
			macro->assignTextMacro(index, value.get_string());
			break;
		}
		case simdjson::ondemand::json_type::boolean:
		{
			macro->assignBoolMacro(index, value.get_bool());
			break;
		}
		case simdjson::ondemand::json_type::null:
		{
			macro->assignNullMacro(index);
			break;
		}
		default:
			break;
		}
		++index;
	}
}

void AddJSONObjectValues(simdjson::ondemand::object obj, TemplateMacro* macro)
{
	for (auto field : obj)
	{
		auto value = field.value();

		simdjson::ondemand::json_type type = value.type();
		switch (type)
		{
		case simdjson::ondemand::json_type::array:
		{
			auto amacro = macro->assignArrayMacro(field.unescaped_key());
			AddJSONArrayValues(value.get_array(), amacro);
			break;
		}
		case simdjson::ondemand::json_type::object:
		{
			auto smacro = macro->assignStructMacro(field.unescaped_key());
			AddJSONObjectValues(value.get_object(), smacro);
			break;
		}
		case simdjson::ondemand::json_type::number:
		{
			simdjson::ondemand::number_type numberType = value.get_number_type();
			switch (numberType)
			{
			case simdjson::ondemand::number_type::floating_point_number:
				macro->assignFloatMacro(field.unescaped_key(), value.get_double());
				break;
			case simdjson::ondemand::number_type::signed_integer:
				macro->assignIntMacro(field.unescaped_key(), value.get_int64());
				break;
			case simdjson::ondemand::number_type::unsigned_integer:
				macro->assignUIntMacro(field.unescaped_key(), value.get_uint64());
				break;
			}
			break;
		}
		case simdjson::ondemand::json_type::string:
		{
			macro->assignTextMacro(field.unescaped_key(), value.get_string());
			break;
		}
		case simdjson::ondemand::json_type::boolean:
		{
			macro->assignBoolMacro(field.unescaped_key(), value.get_bool());
			break;
		}
		case simdjson::ondemand::json_type::null:
		{
			macro->assignNullMacro(field.unescaped_key());
			break;
		}
		default:
			break;
		}
	}
}

void TemplateEngine::LoadObject(TemplateEnvironment& environment, const TemplateCall& call)
{
	std::string objectFile = call.m_Arguments[1].substr(1, call.m_Arguments[1].size() - 2);
	EscapeString(objectFile, environment.m_Engine->m_EscapeCharacter);
	std::filesystem::path objectFilepath = objectFile;

	if (!std::filesystem::exists(objectFilepath))
	{
		std::cerr << "'" << objectFilepath << "' doesn't exist!!\n";
		return;
	}

	auto& type = call.m_Arguments[2];
	if (type == "json")
	{
		simdjson::ondemand::parser   parser;
		simdjson::padded_string      json = simdjson::padded_string::load(objectFilepath.string());
		simdjson::ondemand::document root = parser.iterate(json);

		// Convert json object to macros
		auto rootMacro = environment.assignStructMacro(call.m_Arguments[0]);
		AddJSONObjectValues(root.get_object(), rootMacro);
	}
	else
	{
		std::cerr << "'" << type << "' Unknown object type!!\n";
		return;
	}
}

void TemplateEngine::CallTemplate(TemplateEnvironment& environment, const TemplateCall& call)
{
	TemplateEngine      subEngine;
	TemplateEnvironment subEnvironment;

	for (std::size_t i = 2; i < call.m_Arguments.size(); ++i)
	{
		auto&       arg    = call.m_Arguments[i];
		std::size_t equals = arg.find_first_of('=');
		if (equals < arg.size())
		{
			// Set new macro name
			std::string_view name      = std::string_view { arg }.substr(0, equals);
			std::string_view macroName = std::string_view { arg }.substr(equals + 1);
			auto             macro     = environment.getMacro(macroName);
			if (!macro)
			{
				std::cerr << "'" << macroName << "' is not an assigned macro\n";
				continue;
			}
			subEnvironment.assignRefMacro(name, *macro);
		}
		else
		{
			auto macro = environment.getMacro(arg);
			if (!macro)
			{
				std::cerr << "'" << arg << "' is not an assigned macro\n";
				continue;
			}
			subEnvironment.assignRefMacro(arg, *macro);
		}
	}

	auto&       target       = call.m_Arguments[0];
	std::string templateFile = call.m_Arguments[1].substr(1, call.m_Arguments[1].size() - 2);
	EscapeString(templateFile, environment.m_Engine->m_EscapeCharacter);
	templateFile = environment.m_CWD + '/' + templateFile;

	char previousCommandChar     = environment.m_Engine->m_CommandChar;
	char previousOpenReference   = environment.m_Engine->m_OpenReference;
	char previousCloseReference  = environment.m_Engine->m_CloseReference;
	char previousEscapeCharacter = environment.m_Engine->m_EscapeCharacter;

	auto result = environment.m_Engine->executeTemplate(templateFile, subEnvironment);

	environment.m_Engine->m_CommandChar     = previousCommandChar;
	environment.m_Engine->m_OpenReference   = previousOpenReference;
	environment.m_Engine->m_CloseReference  = previousCloseReference;
	environment.m_Engine->m_EscapeCharacter = previousEscapeCharacter;

	if (target == "Insert")
	{
		environment.insertText(result, true);
	}
	else if (target.starts_with("\"") && target.ends_with("\""))
	{
		std::string str = target.substr(1, target.size() - 2);
		EscapeString(str, environment.m_Engine->m_EscapeCharacter);
		std::filesystem::path path = str;
		std::filesystem::create_directories(path.parent_path());
		std::ofstream file { path };
		if (file)
		{
			file.write(result.data(), result.size());
			file.close();
		}
	}
}