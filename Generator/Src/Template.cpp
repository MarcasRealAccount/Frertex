#include "Template.h"

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

ETemplateMacroType TemplateMacro::GetBestType(std::string_view str)
{
	// Check if str is a number
	if (str.empty())
		return ETemplateMacroType::Text;

	if (str[0] == '-')
	{
		// str might be a negative number
		for (std::size_t i = 0; i < str.size(); ++i)
			if (!std::isdigit(str[i]))
				return ETemplateMacroType::Text;
		return ETemplateMacroType::Int;
	}

	// str might be a positive number
	for (std::size_t i = 0; i < str.size(); ++i)
		if (!std::isdigit(str[i]))
			return ETemplateMacroType::Text;
	return ETemplateMacroType::UInt;
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
	                         [firstName](const std::pair<std::string, TemplateMacro>& a)
	                         {
                                return a.first == firstName;
                            });
	if (itr == map.end())
	{
		if (dot < subscriptStart)
			itr = map.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} } }).first;
		else if (subscriptStart < name.size())
			itr = map.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} } }).first;
		else
			itr = map.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Int, 0LL } }).first;
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
	                         [firstName](const std::pair<std::string, TemplateMacro>& a)
	                         {
                                return a.first == firstName;
                            });
	if (itr == map.end())
	{
		if (dot < subscriptStart)
			itr = map.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} } }).first;
		else if (subscriptStart < name.size())
			itr = map.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} } }).first;
		else
			itr = map.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::UInt, 0ULL } }).first;
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
	                         [firstName](const std::pair<std::string, TemplateMacro>& a)
	                         {
                                return a.first == firstName;
                            });
	if (itr == map.end())
	{
		if (dot < subscriptStart)
			itr = map.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} } }).first;
		else if (subscriptStart < name.size())
			itr = map.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} } }).first;
		else
			itr = map.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Text, std::string {} } }).first;
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
	                         [firstName](const std::pair<std::string, TemplateMacro>& a)
	                         {
                                return a.first == firstName;
                            });
	if (itr == map.end())
	{
		if (dot < subscriptStart)
			itr = map.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} } }).first;
		else if (subscriptStart < name.size())
			itr = map.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} } }).first;
		else
			itr = map.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} } }).first;
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
	                         [firstName](const std::pair<std::string, TemplateMacro>& a)
	                         {
                                return a.first == firstName;
                            });
	if (itr == map.end())
	{
		if (dot < subscriptStart)
			itr = map.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} } }).first;
		else if (subscriptStart < name.size())
			itr = map.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} } }).first;
		else
			itr = map.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} } }).first;
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
	                         [firstName](const std::pair<std::string, TemplateMacro>& a)
	                         {
                                return a.first == firstName;
                            });
	if (itr == map.end())
	{
		if (dot < subscriptStart)
			itr = map.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} } }).first;
		else if (subscriptStart < name.size())
			itr = map.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} } }).first;
		else
			itr = map.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Ref, nullptr } }).first;
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
	                         [name](const std::pair<std::string, TemplateMacro>& a)
	                         {
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
	                         [name](const std::pair<std::string, TemplateMacro>& a)
	                         {
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
	                         [firstName](const std::pair<std::string, TemplateMacro>& a)
	                         {
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
		referenceStart -= 2;
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
	if (m_CurrentCall != back.m_StartCallIndex)
	{
		std::cerr << "Can only erase range within the first call of the range!!\n";
		return;
	}

	auto start = back.m_StartCall->m_SourceIndex + m_Offset;
	auto end   = back.m_EndCall->m_SourceIndex + m_Offset;
	m_Source.erase(start, end - start);
	m_Offset -= end - start;
}

void TemplateEnvironment::insertText(std::string_view text)
{
	auto& call = m_Template->m_Calls[m_CurrentCall];
	m_Source.insert(call.m_SourceIndex + m_Offset, text);
	m_Offset += text.size();
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

void TemplateEnvironment::pushStack(std::size_t start, std::size_t end, void* userData)
{
	m_CallStack.emplace_back(TemplateCallStackEntry {
	    start < m_Template->m_Calls.size() ? &m_Template->m_Calls[start] : nullptr,
	    end < m_Template->m_Calls.size() ? &m_Template->m_Calls[end] : nullptr,
	    start,
	    end,
	    userData });
	m_CurrentLastCall = end;
}

void TemplateEnvironment::popStack()
{
	m_CallStack.pop_back();
	m_CurrentLastCall = m_CallStack.empty() ? 0 : m_CallStack.back().m_EndCallIndex;
}

void TemplateEnvironment::jump(std::size_t call)
{
	m_CurrentCall   = call;
	m_IncrementCall = false;
}

std::size_t TemplateEnvironment::findNextCall(std::string_view callName)
{
	if (callName == "End")
	{
		std::size_t depth = 1;
		for (std::size_t index = m_CurrentCall + 1; index < m_CurrentLastCall; ++index)
		{
			auto& name = m_Template->m_Calls[index].m_Function;
			if (name == "End")
				if (--depth == 0)
					return index;

			auto function = m_Engine->getFunction(name);
			if (function && function->hasEndCall())
				++depth;
		}
		return ~0ULL;
	}
	else
	{
		for (std::size_t index = m_CurrentCall + 1; index < m_CurrentLastCall; ++index)
			if (m_Template->m_Calls[index].m_Function == callName)
				return index;
		return ~0ULL;
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
	                        [firstName](const std::pair<std::string, TemplateMacro>& a)
	                        {
		                        return a.first == firstName;
	                        });
	if (itr == m_Macros.end())
	{
		if (dot < subscriptStart)
			itr = m_Macros.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} } }).first;
		else if (subscriptStart < name.size())
			itr = m_Macros.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} } }).first;
		else
			itr = m_Macros.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Int, 0LL } }).first;
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
	                        [firstName](const std::pair<std::string, TemplateMacro>& a)
	                        {
		                        return a.first == firstName;
	                        });
	if (itr == m_Macros.end())
	{
		if (dot < subscriptStart)
			itr = m_Macros.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} } }).first;
		else if (subscriptStart < name.size())
			itr = m_Macros.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} } }).first;
		else
			itr = m_Macros.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::UInt, 0LL } }).first;
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

TemplateMacro* TemplateEnvironment::assignTextMacro(std::string_view name, std::string_view value)
{
	std::size_t dot            = name.find_first_of('.');
	std::size_t subscriptStart = name.find_first_of('[');
	std::size_t firstNameEnd   = std::min(dot, subscriptStart);

	std::string_view firstName = name.substr(0, firstNameEnd);

	auto itr = std::find_if(m_Macros.begin(),
	                        m_Macros.end(),
	                        [firstName](const std::pair<std::string, TemplateMacro>& a)
	                        {
		                        return a.first == firstName;
	                        });
	if (itr == m_Macros.end())
	{
		if (dot < subscriptStart)
			itr = m_Macros.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} } }).first;
		else if (subscriptStart < name.size())
			itr = m_Macros.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} } }).first;
		else
			itr = m_Macros.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Text, std::string {} } }).first;
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
	                        [firstName](const std::pair<std::string, TemplateMacro>& a)
	                        {
		                        return a.first == firstName;
	                        });
	if (itr == m_Macros.end())
	{
		if (dot < subscriptStart)
			itr = m_Macros.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} } }).first;
		else if (subscriptStart < name.size())
			itr = m_Macros.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} } }).first;
		else
			itr = m_Macros.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} } }).first;
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
	                        [firstName](const std::pair<std::string, TemplateMacro>& a)
	                        {
		                        return a.first == firstName;
	                        });
	if (itr == m_Macros.end())
	{
		if (dot < subscriptStart)
			itr = m_Macros.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} } }).first;
		else if (subscriptStart < name.size())
			itr = m_Macros.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} } }).first;
		else
			itr = m_Macros.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} } }).first;
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
	                        [firstName](const std::pair<std::string, TemplateMacro>& a)
	                        {
		                        return a.first == firstName;
	                        });
	if (itr == m_Macros.end())
	{
		if (dot < subscriptStart)
			itr = m_Macros.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Struct, std::unordered_map<std::string, TemplateMacro> {} } }).first;
		else if (subscriptStart < name.size())
			itr = m_Macros.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Array, std::vector<TemplateMacro> {} } }).first;
		else
			itr = m_Macros.insert({ std::string { firstName }, TemplateMacro { ETemplateMacroType::Ref, nullptr } }).first;
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
	                        [firstName](const std::pair<std::string, TemplateMacro>& a)
	                        {
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
	                        [firstName](const std::pair<std::string, TemplateMacro>& a)
	                        {
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
	                        [firstName](const std::pair<std::string, TemplateMacro>& a)
	                        {
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
	addFunction("For", { &For, &For });
	addFunction("Foreach", { &Foreach, &Foreach });
	addFunction("If", { &If, &If });
	addFunction("Ifn", { &If, &If });
	addFunction("Ifeq", { &If, &If });
	addFunction("Ifneq", { &If, &If });
}

std::string TemplateEngine::executeTemplate(std::string_view name, TemplateEnvironment& environment)
{
	compileTemplate(name);

	auto itr = std::find_if(m_Templates.begin(),
	                        m_Templates.end(),
	                        [name](const std::pair<std::string, Template>& a)
	                        {
		                        return a.first == name;
	                        });
	if (itr == m_Templates.end())
		return {};

	environment.m_Engine   = this;
	environment.m_Template = &itr->second;
	environment.m_CallStack.clear();
	environment.m_Source = environment.m_Template->m_Text;

	environment.pushStack(0, environment.m_Template->m_Calls.size(), nullptr);

	while (!environment.m_CallStack.empty())
	{
		while (environment.m_CurrentCall < environment.m_CurrentLastCall)
		{
			environment.m_IncrementCall = true;

			auto& call = environment.m_Template->m_Calls[environment.m_CurrentCall];
			invoke(call.m_Function, environment, call);

			if (environment.m_IncrementCall)
				++environment.m_CurrentCall;
		}

		if (environment.m_CurrentCall < environment.m_Template->m_Calls.size())
		{
			auto& back = environment.m_CallStack.back();
			if (back.m_EndCall)
			{
				environment.m_IncrementCall = true;

				invokeEnd(back.m_StartCall->m_Function, environment, *back.m_EndCall);

				if (environment.m_IncrementCall)
					++environment.m_CurrentCall;
			}
			else
			{
				environment.popStack();
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
	                        [name](const std::pair<std::string, TemplateFunction>& a)
	                        {
		                        return a.first == name;
	                        });
	if (itr != m_Functions.end())
		return;

	m_Functions.insert({ std::string { name }, std::move(function) });
}

void TemplateEngine::removeFunction(std::string_view name)
{
	auto itr = std::find_if(m_Functions.begin(),
	                        m_Functions.end(),
	                        [name](const std::pair<std::string, TemplateFunction>& a)
	                        {
		                        return a.first == name;
	                        });
	if (itr != m_Functions.end())
		m_Functions.erase(itr);
}

TemplateFunction* TemplateEngine::getFunction(std::string_view name)
{
	auto itr = std::find_if(m_Functions.begin(),
	                        m_Functions.end(),
	                        [name](const std::pair<std::string, TemplateFunction>& a)
	                        {
		                        return a.first == name;
	                        });

	return itr != m_Functions.end() ? &itr->second : nullptr;
}

void TemplateEngine::invoke(std::string_view name, TemplateEnvironment& environment, const TemplateCall& call)
{
	auto function = getFunction(name);
	if (!function)
	{
		std::cerr << "Function '" << name << "' does not exist!\n";
		return;
	}

	function->m_Callback(environment, call);
}

void TemplateEngine::invokeEnd(std::string_view name, TemplateEnvironment& environment, const TemplateCall& call)
{
	auto function = getFunction(name);
	if (!function)
	{
		std::cerr << "Function '" << name << "' does not exist!\n";
		return;
	}

	function->m_EndCallback(environment, call);
}

void TemplateEngine::compileTemplate(std::string_view name)
{
	auto itr = std::find_if(m_Templates.begin(),
	                        m_Templates.end(),
	                        [name](const std::pair<std::string, Template>& a)
	                        {
		                        return a.first == name;
	                        });
	if (itr != m_Templates.end())
		return;

	std::ifstream file { std::filesystem::path { "Templates" } / name, std::ios::ate };
	if (file)
	{
		std::string source(file.tellg(), '\0');
		file.seekg(0);
		file.read(source.data(), source.size());
		file.close();

		std::vector<TemplateCall> calls;

		std::size_t offset = 0;
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
			//std::cout << source.substr(callStart, (callEnd - 2) - callStart) << '\n';

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
			for (; !failed && !end && sourceOffset > 0; --sourceOffset)
			{
				switch (source[sourceOffset])
				{
				case ' ':
				case '\t':
					break;
				case '\n':
					end = true;
					break;
				default:
					sourceOffset = callStart - 2;
					failed       = true;
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
					break;
				}
				if (failed)
					break;
			}

			calls.emplace_back(TemplateCall { source.substr(callNameStart, callNameEnd - callNameStart), std::move(arguments), sourceOffset });
			source.erase(sourceOffset, sourceEnd - sourceOffset);
			offset = sourceOffset;
		}
		m_Templates.insert({ std::string { name }, Template { std::move(source), std::move(calls) } });
	}
	else
	{
		std::cerr << "Could not find template file '" << name << "'!\n";
	}
}

void TemplateEngine::Insert(TemplateEnvironment& environment, const TemplateCall& call)
{
	auto macro = environment.getMacro(call.m_Arguments[0]);
	if (!macro)
		return;

	switch (macro->m_Type)
	{
	case ETemplateMacroType::Int:
		environment.insertText(std::to_string(std::get<std::int64_t>(macro->m_Value)));
		break;
	case ETemplateMacroType::UInt:
		environment.insertText(std::to_string(std::get<std::uint64_t>(macro->m_Value)));
		break;
	case ETemplateMacroType::Text:
		environment.insertText(std::get<std::string>(macro->m_Value));
		break;
	case ETemplateMacroType::Ref:
	{
		auto ptr = std::get<TemplateMacro*>(macro->m_Value);
		switch (ptr->m_Type)
		{
		case ETemplateMacroType::Int:
			environment.insertText(std::to_string(std::get<std::int64_t>(ptr->m_Value)));
			break;
		case ETemplateMacroType::UInt:
			environment.insertText(std::to_string(std::get<std::uint64_t>(ptr->m_Value)));
			break;
		case ETemplateMacroType::Text:
			environment.insertText(std::get<std::string>(ptr->m_Value));
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
		case ETemplateMacroType::Int:
			environment.assignIntMacro(call.m_Arguments[0], std::strtoll(str.data(), nullptr, 10));
			break;
		case ETemplateMacroType::UInt:
			environment.assignUIntMacro(call.m_Arguments[0], std::strtoull(str.data(), nullptr, 10));
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
		std::int64_t maxWidth = std::strtoll(call.m_Arguments[3].data(), nullptr, 10);
		if (maxWidth < 0 || maxWidth > 16)
		{
			std::cerr << "Casting int to hex less than 0 or wider than 16 is illegal!\n";
			return;
		}

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
		ForData* forData = reinterpret_cast<ForData*>(entry->m_UserData);
		forData->m_CurrentValue += forData->m_Increment;
		if (forData->m_CurrentValue != forData->m_EndValue)
		{
			environment.assignIntMacro(forData->m_VariableName, forData->m_CurrentValue);
			environment.insertText(forData->m_SourceText);
			environment.jump(entry->m_StartCallIndex + 1);
			return;
		}
		delete forData;
		environment.popStack();
	}
	else
	{
		auto end = environment.findNextCall("End");
		if (end >= environment.m_CurrentLastCall)
		{
			std::cerr << "'For' was unable to find end call, please add '$$End$$' after '$$For$$'\n";
			return;
		}

		ForData* forData = new ForData {
			call.m_Arguments[0],
			std::strtoll(call.m_Arguments[1].data(), nullptr, 10),
			std::strtoll(call.m_Arguments[2].data(), nullptr, 10),
			std::strtoll(call.m_Arguments[3].data(), nullptr, 10),
			{}
		};

		environment.pushStack(environment.m_CurrentCall, end, forData);
		if (forData->m_CurrentValue == forData->m_EndValue)
		{
			environment.eraseRange();
			environment.jump(end + 1);
			environment.popStack();
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
				environment.jump(entry->m_StartCallIndex + 1);
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
				environment.jump(entry->m_StartCallIndex + 1);
				return;
			}
			break;
		}
		}
		delete foreachData;
		environment.popStack();
	}
	else
	{
		auto end = environment.findNextCall("End");
		if (end >= environment.m_CurrentLastCall)
		{
			std::cerr << "'Foreach' was unable to find end call, please add '$$End$$' after '$$Foreach$$'\n";
			return;
		}


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

		environment.pushStack(environment.m_CurrentCall, end, foreachData);
		if (!macro)
		{
			std::cerr << "'" << call.m_Arguments[1] << "' is not an assigned macro\n";
			environment.eraseRange();
			environment.jump(end + 1);
			environment.popStack();
			return;
		}

		if (!foreachData)
		{
			std::cerr << "'" << call.m_Arguments[1] << "' is not an iterable macro\n";
			environment.eraseRange();
			environment.jump(end + 1);
			environment.popStack();
			return;
		}

		if (!currentValue)
		{
			delete foreachData;
			environment.eraseRange();
			environment.jump(end + 1);
			environment.popStack();
			return;
		}

		foreachData->m_SourceText = environment.getSourceRange();
		environment.assignRefMacro(foreachData->m_VariableName, *currentValue);
	}
}

void TemplateEngine::If(TemplateEnvironment& environment, const TemplateCall& call)
{
}