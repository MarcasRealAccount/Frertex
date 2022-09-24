#!$<>\
#include "Frertex/Transpilers/SPIRV/Enumerations.h"
#include "Frertex/Utils/Profiler.h"

namespace Frertex::Transpilers::SPIRV
{
	$$Foreach:REQUIRECAPEXT,REQUIRECAPEXT_IMPLS$$
	$$Assign:TYPENAME,E$<REQUIRECAPEXT.NAME>$$$
	$$Template:Insert,"RequireCapExtImpl.h",NAME=REQUIRECAPEXT.NAME,TYPENAME=TYPENAME,CASES=REQUIRECAPEXT.CASES$$
	$$End$$
} // namespace Frertex::Transpilers::SPIRV