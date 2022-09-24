#!$<>\
#pragma once

#include <cstdint>

#include <set>
#include <string>

namespace Frertex::Transpilers::SPIRV
{
	$$Foreach:ENUMERATION,ENUMERATIONS$$
	$$Assign:TYPENAME,E$<REQUIRECAPEXT.NAME>$$$
	$$Template:Insert,"Enumeration.h",TYPENAME=TYPENAME,ENUMS=ENUMERATION.ENUMS$$
	$$END$$

	$$Foreach:REQUIRECAPEXT,REQUIRECAPEXTS$$
	$$Assign:TYPENAME,E$<REQUIRECAPEXT.NAME>$$$
	$$Template:Insert,"RequireCapExt.h",NAME=REQUIRECAPEXT.NAME,TYPENAME=TYPENAME$$
	$$End$$
} // namespace Frertex::Transpilers::SPIRV