#!$<>\
$$Assign:ARGUMENT_LIST$$
$$Foreach:ARGUMENT,ARGUMENTS$$
	$$Ifn:ARGUMENT_LIST$$
		$$Append:ARGUMENT_LIST,\, $$
	$$End$$
	$$If:ARGUMENT.VALUE$$
		$$Append:ARGUMENT_LIST,$<ARGUMENT.TYPE>$ $<ARGUMENT.NAME>$ = $<ARGUMENT.VALUE>$$$
	$$Else$$
		$$Append:ARGUMENT_LIST,$<ARGUMENT.TYPE>$ $<ARGUMENT.NAME>$$$
	$$End$$
$$End$$
void $$Insert:NAME$$($$Insert:ARGUMENT_LIST$$); // $$Insert:OPCODE$$