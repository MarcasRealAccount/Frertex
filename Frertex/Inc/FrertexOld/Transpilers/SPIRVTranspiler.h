#pragma once

#include "Frertex/FIL.h"
#include "Frertex/Message.h"
#include "Frertex/Utils/Buffer.h"
#include "Frertex/Utils/Profiler.h"
#include "Frertex/Utils/Utils.h"
#include "SPIR-V/CodeBuffer.h"

#include <fmt/format.h>
#include <set>
#include <unordered_map>

namespace Frertex::Transpilers::SPIRV
{
	template <class F, class R, class... Args>
	concept CallableWith = requires(F f, Args&&... args) {
		                       {
			                       f(std::forward<Args>(args)...)
			                       } -> std::convertible_to<R>;
	                       };

	struct SPIRV
	{
	public:
		std::uint32_t m_IDBound = 1;
		CodeBuffer    m_Code;
	};

	std::vector<std::uint8_t> GetSPIRVBinary(const SPIRV& spirv);

	void WriteSPIRVToFile(const std::filesystem::path& file, const SPIRV& spirv);

	class SPIRVTranspiler
	{
	public:
		SPIRVTranspiler(Sources* sources);

		SPIRV transpile(Utils::CopyMovable<FIL>&& fil);

		auto& getMessages() const { return m_Messages; }

	private:
		void requiresCapability(ECapability capability) { m_Capabilities.insert(capability); }

		template <CallableWith<std::uint32_t> F>
		std::uint32_t getOrAddResultID(const std::string& name, F callback);
		std::uint32_t getOrAddBuiltinType(ETypeIDs type, SPIRV& spirv, CodeBuffer& typesCode);
		template <CallableWith<std::pair<std::string, std::uint32_t>> F>
		std::uint32_t getOrAddPointerType(EStorageClass storageClass, SPIRV& spirv, CodeBuffer& typesCode, F callback);
		template <CallableWith<std::pair<std::string, std::uint32_t>> F>
		std::uint32_t getOrAddArrayType(SPIRV& spirv, CodeBuffer& typesCode, std::uint32_t lengthID, F callback);
		template <CallableWith<std::pair<std::string, std::uint32_t>> F, CallableWith<std::pair<std::string, std::uint32_t>>... Fs>
		std::uint32_t getOrAddFunctionType(SPIRV& spirv, CodeBuffer& typesCode, F returnType, Fs... parameterTypes);
		template <CallableWith<std::uint32_t> F>
		std::pair<std::string, std::uint32_t> getOrAddResultIDN(const std::string& name, F callback);
		std::pair<std::string, std::uint32_t> getOrAddBuiltinTypeN(ETypeIDs type, SPIRV& spirv, CodeBuffer& typesCode);

		std::uint32_t addBuiltinType(ETypeIDs type, SPIRV& spirv, CodeBuffer& typesCode);

		void addWarning(std::uint32_t fileID, std::uint32_t sourceID, std::size_t index, std::size_t length, std::size_t point, Utils::CopyMovable<std::string>&& message);
		void addError(std::uint32_t fileID, std::uint32_t sourceID, std::size_t index, std::size_t length, std::size_t point, Utils::CopyMovable<std::string>&& message);
		void addWarning(const Token& token, std::size_t point, Utils::CopyMovable<std::string>&& message)
		{
			addWarning(token.m_FileID, token.m_SourceID, token.m_Index, token.m_Length, point, message.get());
		}
		void addError(const Token& token, std::size_t point, Utils::CopyMovable<std::string>&& message)
		{
			addError(token.m_FileID, token.m_SourceID, token.m_Index, token.m_Length, point, message.get());
		}

	private:
		Sources* m_Sources;

		std::vector<Message> m_Messages;

		std::set<ECapability>                          m_Capabilities;
		std::unordered_map<std::string, std::uint32_t> m_NamedResultIDs;
	};

	template <CallableWith<std::uint32_t> F>
	std::uint32_t SPIRVTranspiler::getOrAddResultID(const std::string& name, F callback)
	{
		PROFILE_FUNC;

		auto itr = m_NamedResultIDs.find(name);
		if (itr != m_NamedResultIDs.end())
			return itr->second;

		std::uint32_t resultID = callback();
		m_NamedResultIDs.insert({ name, resultID });
		return resultID;
	}

	template <CallableWith<std::pair<std::string, std::uint32_t>> F>
	std::uint32_t SPIRVTranspiler::getOrAddPointerType(EStorageClass storageClass, SPIRV& spirv, CodeBuffer& typesCode, F callback)
	{
		PROFILE_FUNC;

		std::pair<std::string, std::uint32_t> pointerType = callback();

		return getOrAddResultID(
		    fmt::format("_ptr_{}_{}", StorageClassToString(storageClass), pointerType.first),
		    [&]() -> std::uint32_t
		    {
			    PROFILE_FUNC;

			    std::uint32_t resultID = spirv.m_IDBound++;
			    typesCode.pushOpTypePointer(resultID, storageClass, pointerType.second);
			    return resultID;
		    });
	}

	template <CallableWith<std::pair<std::string, std::uint32_t>> F>
	std::uint32_t SPIRVTranspiler::getOrAddArrayType(SPIRV& spirv, CodeBuffer& typesCode, std::uint32_t lengthID, F callback)
	{
		PROFILE_FUNC;

		std::pair<std::string, std::uint32_t> elementType = callback();

		return getOrAddResultID(
		    fmt::format("_arr_{}_{}", elementType.first, lengthID),
		    [&]() -> std::uint32_t
		    {
			    PROFILE_FUNC;

			    std::uint32_t resultID = spirv.m_IDBound++;
			    typesCode.pushOpTypeArray(resultID, elementType.second, lengthID);
			    return resultID;
		    });
	}

	template <CallableWith<std::pair<std::string, std::uint32_t>> F, CallableWith<std::pair<std::string, std::uint32_t>>... Fs>
	std::uint32_t SPIRVTranspiler::getOrAddFunctionType(SPIRV& spirv, CodeBuffer& typesCode, F returnType, Fs... parameterTypes)
	{
		PROFILE_FUNC;

		std::pair<std::string, std::uint32_t>              returnResult = returnType();
		std::vector<std::pair<std::string, std::uint32_t>> parameterResults;
		parameterResults.reserve(sizeof...(Fs));
		(parameterResults.emplace_back(parameterTypes()), ...);

		std::string name = returnResult.first + '(';
		for (std::size_t i = 0; i < parameterResults.size(); ++i)
		{
			if (i > 0)
				name += ',';
			name += parameterResults[i].first;
		}
		name += ')';

		return getOrAddResultID(
		    name,
		    [&]() -> std::uint32_t
		    {
			    PROFILE_FUNC;

			    std::vector<std::uint32_t> params(parameterResults.size());
			    for (std::size_t i = 0; i < parameterResults.size(); ++i)
				    params[i] = parameterResults[i].second;

			    std::uint32_t resultID = spirv.m_IDBound++;
			    typesCode.pushOpTypeFunction(resultID, returnResult.second, params);
			    return resultID;
		    });
	}

	template <CallableWith<std::uint32_t> F>
	std::pair<std::string, std::uint32_t> SPIRVTranspiler::getOrAddResultIDN(const std::string& name, F callback)
	{
		PROFILE_FUNC;

		auto itr = m_NamedResultIDs.find(name);
		if (itr != m_NamedResultIDs.end())
			return *itr;

		std::uint32_t resultID = callback();
		m_NamedResultIDs.insert({ name, resultID });
		return { name, resultID };
	}
} // namespace Frertex::Transpilers::SPIRV
