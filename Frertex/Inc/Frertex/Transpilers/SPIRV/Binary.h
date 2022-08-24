#pragma once

#include <filesystem>

namespace Frertex::Transpilers::SPIRV
{
	struct Binary
	{
	};

	void WriteToFile(const Binary& binary, const std::filesystem::path& filepath);
} // namespace Frertex::Transpilers::SPIRV