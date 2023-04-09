#include "Frertex/Source.h"
#include "Frertex/Utils/Profiler.h"

#include <sstream>

namespace Frertex::Source
{
	std::string SourcePointToString(const SourcePoint& point)
	{
		std::ostringstream str;
		str << (point.m_Line + 1) << ':' << (point.m_Column + 1);
		return str.str();
	}

	std::string SourceSpanToString(const SourceSpan& span)
	{
		std::ostringstream str;
		str << (span.m_StartLine + 1) << ':' << (span.m_StartColumn + 1) << " -> " << (span.m_EndLine + 1) << ':' << (span.m_EndColumn + 1);
		return str.str();
	}

	Source::Source(Utils::CopyMovable<std::string>&& name, Utils::CopyMovable<std::string>&& str, std::uint32_t id)
	    : m_Name(name.get()), m_Str(str.get()), m_ID(id)
	{
		PROFILE_FUNC;

		m_Indices.emplace_back(0);
		for (std::size_t i = 0; i < m_Str.size(); ++i)
			if (m_Str[i] == '\n')
				m_Indices.emplace_back(i + 1);
	}

	std::size_t Source::getLineNumber(std::size_t index) const
	{
		PROFILE_FUNC;

		for (std::size_t i = 0; i < m_Indices.size(); ++i)
			if (m_Indices[i] > index)
				return i - 1;

		return m_Indices.size() - 1;
	}

	SourcePoint Source::getPoint(std::size_t index) const
	{
		PROFILE_FUNC;

		std::size_t line   = getLineNumber(index);
		std::size_t column = index - m_Indices[line];

		return SourcePoint { index, line, column, m_ID, m_ID };
	}

	SourceSpan Source::getSpan(std::size_t index, std::size_t length) const
	{
		PROFILE_FUNC;

		std::size_t startLine   = getLineNumber(index);
		std::size_t startColumn = index - m_Indices[startLine];
		std::size_t endIndex    = index + length;
		std::size_t endLine     = getLineNumber(endIndex);
		std::size_t endColumn   = endIndex - m_Indices[endLine];

		return { index, startLine, startColumn, endIndex, endLine, endColumn, m_ID, m_ID };
	}

	std::uint32_t Sources::addSource(Utils::CopyMovable<std::string>&& name, Utils::CopyMovable<std::string>&& str)
	{
		PROFILE_FUNC;

		return m_Sources.emplace_back(name.get(), str.get(), static_cast<std::uint32_t>(m_Sources.size() + 1)).getID();
	}

	std::uint32_t Sources::getSourceID(std::string_view name) const
	{
		PROFILE_FUNC;

		for (auto& source : m_Sources)
			if (source.getName() == name)
				return source.getID();
		return 0;
	}

	Source* Sources::getSource(std::string_view name)
	{
		PROFILE_FUNC;

		for (auto& source : m_Sources)
			if (source.getName() == name)
				return &source;
		return nullptr;
	}

	const Source* Sources::getSource(std::string_view name) const
	{
		PROFILE_FUNC;

		for (auto& source : m_Sources)
			if (source.getName() == name)
				return &source;
		return nullptr;
	}

	Source* Sources::getSource(std::uint32_t id)
	{
		PROFILE_FUNC;

		return (id - 1) < m_Sources.size() ? &m_Sources[id - 1] : nullptr;
	}

	const Source* Sources::getSource(std::uint32_t id) const
	{
		PROFILE_FUNC;

		return (id - 1) < m_Sources.size() ? &m_Sources[id - 1] : nullptr;
	}
} // namespace Frertex::Source