#pragma once

#include "EntrypointType.h"
#include "TypeID.h"
#include "TypeQualifier.h"

#include <filesystem>

namespace Frertex::FIL
{
	struct Binary
	{
	};

	void WriteToFile(const Binary& binary, const std::filesystem::path& filepath);
} // namespace Frertex::FIL