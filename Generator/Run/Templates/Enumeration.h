$$ASSIGN:VALUES,$$
$$FOREACH:ENUM=ENUMS$$
	$$IF:VALUES$$
		$$APPEND:VALUES,\,\n$$
	$$END$$
	$$APPEND:VALUES,$$ENUM.NAME$$ = $$ENUM.VALUE$$$$
$$END$$
$$ALIGNL:VALUES,=$$
enum class $$TYPENAME$$ : std::uint32_t
{
	$$VALUES$$
};