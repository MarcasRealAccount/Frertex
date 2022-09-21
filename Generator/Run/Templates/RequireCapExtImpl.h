void RequireCapExt$$NAME$$([[maybe_unused]] $$TYPENAME$$ value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
{
	PROFILE_FUNC;
	$$IF:CASES$$

	switch (value)
	{
	$$FOREACH:CASE=CASES$$
	case $$CASE.NAME$$:
		$$FOREACH:CAPABILITY=CASE.CAPABILITIES$$
		$$ASSIGN:CTYPENAME,E$$
		$$APPEND:CTYPENAME,$$CAPABILITY.NAME$$$$
		capabilities.insert($$CTYPENAME$$::$$CAPABILITY.VALUE$$);
		$$END$$
		$$FOREACH:EXTENSION=CASE.EXTENSIONS$$
		extensions.insert($$EXTENSION$$);
		$$END$$
		break;
	$$END$$
	default: break;
	}
	$$END$$
}