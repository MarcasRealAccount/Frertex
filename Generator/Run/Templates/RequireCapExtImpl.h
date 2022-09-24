#!$<>\
void RequireCapExt$$Insert:NAME$$([[maybe_unused]] $$Insert:TYPENAME$$ value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
{
	PROFILE_FUNC;
	$$If:CASES$$

	switch (value)
	{
	$$Foreach:CASE,CASES$$
	case $$Insert:CASE.NAME$$:
		$$Foreach:CAPABILITY,CASE.CAPABILITIES$$
		$$Assign:CTYPENAME,E$<CAPABILITY.NAME>$$$
		capabilities.insert($$Insert:CTYPENAME$$::$$Insert:CAPABILITY.VALUE$$);
		$$End$$
		$$Foreach:EXTENSION,CASE.EXTENSIONS$$
		extensions.insert($$Insert:EXTENSION$$);
		$$End$$
		break;
	$$End$$
	default: break;
	}
	$$End$$
}