#!$<>\
$$Assign:VALUES$$
$$Foreach:ENUM,ENUMS$$
	$$If:VALUES$$
		$$Append:VALUES,\,\n$$
	$$End$$
	$$Append:VALUES,$<ENUM.NAME>$ = $<ENUM.VALUE>$$$
$$End$$
$$AlignL:VALUES,=$$
enum class $$Insert:TYPENAME$$ : std::uint32_t
{
	$$Insert:VALUES$$
};