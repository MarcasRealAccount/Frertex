#include "Frertex/Utils/Utils.h"
#include "Frertex/Utils/Profiler.h"

namespace Frertex::Utils
{
	std::string EscapeString(Utils::CopyMovable<std::string>&& str)
	{
		PROFILE_FUNC;

		std::string output = str.get();

		for (std::size_t i = 0; i < output.size(); ++i)
		{
			switch (output[i])
			{
			case '\t':
				output[i] = 't';
				output.insert(output.begin() + i, '\\');
				++i;
				break;
			case '\n':
				output[i] = 'n';
				output.insert(output.begin() + i, '\\');
				++i;
				break;
			case '\v':
				output[i] = 'v';
				output.insert(output.begin() + i, '\\');
				++i;
				break;
			case '\f':
				output[i] = 'f';
				output.insert(output.begin() + i, '\\');
				++i;
				break;
			case '\r':
				output[i] = 'r';
				output.insert(output.begin() + i, '\\');
				++i;
				break;
			case '"':
				output[i] = '"';
				output.insert(output.begin() + i, '\\');
				++i;
				break;
			case '\\':
				output[i] = '\\';
				output.insert(output.begin() + i, '\\');
				++i;
				break;
			}
		}

		return output;
	}
} // namespace Frertex::Utils