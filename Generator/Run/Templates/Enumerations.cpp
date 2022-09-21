#include "Frertex/Transpilers/SPIRV/Enumerations.h"
#include "Frertex/Utils/Profiler.h"

namespace Frertex::Transpilers::SPIRV
{
	$$FOREACH:REQUIRECAPEXT=REQUIRECAPEXT_IMPLS$$
	$$ASSIGN:TYPENAME,E$$REQUIRECAPEXT.NAME$$$$
	$$TEMPLATE:"RequireCapExtImpl.h",NAME=REQUIRECAPEXT.NAME,TYPENAME=TYPENAME,CASES=REQUIRECAPEXT.CASES$$
	$$END$$
} // namespace Frertex::Transpilers::SPIRV