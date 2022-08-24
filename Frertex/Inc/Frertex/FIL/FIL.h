#pragma once

#include <filesystem>

namespace Frertex::FIL
{
	struct Binary
	{
	};

	void WriteToFile(const Binary& binary, const std::filesystem::path& filepath);
} // namespace Frertex::FIL