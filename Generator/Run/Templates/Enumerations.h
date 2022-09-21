#pragma once

#include <cstdint>

#include <set>
#include <string>

namespace Frertex::Transpilers::SPIRV
{
	$$FOREACH:ENUMERATION=ENUMERATIONS$$
	$$ASSIGN:TYPENAME,E$$REQUIRECAPEXT.NAME$$$$
	$$TEMPLATE:"Enumeration.h",TYPENAME=TYPENAME,ENUMS=ENUMERATION.ENUMS$$
	$$END$$

	$$FOREACH:REQUIRECAPEXT=REQUIRECAPEXTS$$
	$$ASSIGN:TYPENAME,E$$REQUIRECAPEXT.NAME$$$$
	$$TEMPLATE:"RequireCapExt.h",NAME=REQUIRECAPEXT.NAME,TYPENAME=TYPENAME$$
	$$END$$
} // namespace Frertex::Transpilers::SPIRV