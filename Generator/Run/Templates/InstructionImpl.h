$$Cast:OPCODE,OPCODE_HEX,hexstr,4$$
$$Assign:CONSTANT_LENGTH,0$$
$$Assign:DYNAMIC_LENGTH$$
$$Assign:ARGUMENT_LIST$$
$$Assign:VALIDATION_LIST$$
$$Assign:REQUIRES_LIST$$
$$Assign:PUSH_LIST$$
$$Foreach:ARGUMENT,ARGUMENTS$$
	$$If:ARGUMENT_LIST$$
		$$Append:ARGUMENT_LIST,\, $$
	$$End$$
	$$Append:ARGUMENT_LIST,$<ARGUMENT.TYPE>$ $<ARGUMENT.NAME>$$$
	$$Ifeq:ARGUMENT.OPTIONAL,?$$
		$$Ifeq:ARGUMENT.CATEGORY,id$$
			$$If:PUSH_LIST$$
				$$Append:PUSH_LIST,\n$$
			$$End$$
			$$If:DYNAMIC_LENGTH$$
				$$Append:DYNAMIC_LENGTH, + $$
			$$End$$
			$$Append:PUSH_LIST,if ($<ARGUMENT.NAME>$)\n\tm_Buffer.emplace_back($<ARGUMENT.NAME>$);$$
			$$Append:DYNAMIC_LENGTH,($<ARGUMENT.NAME>$ ? 1 : 0)$$
		$$Elifeq:ARGUMENT.CATEGORY,int$$
			$$If:PUSH_LIST$$
				$$Append:PUSH_LIST,\n$$
			$$End$$
			$$If:DYNAMIC_LENGTH$$
				$$Append:DYNAMIC_LENGTH, + $$
			$$End$$
			$$Append:PUSH_LIST,if ($<ARGUMENT.NAME>$)\n\tm_Buffer.emplace_back(*$<ARGUMENT.NAME>$);$$
			$$Append:DYNAMIC_LENGTH,($<ARGUMENT.NAME>$ ? 1 : 0)$$
		$$Elifeq:ARGUMENT.CATEGORY,string$$
			$$If:PUSH_LIST$$
				$$Append:PUSH_LIST,\n$$
			$$End$$
			$$If:DYNAMIC_LENGTH$$
				$$Append:DYNAMIC_LENGTH, + $$
			$$End$$
			$$Append:PUSH_LIST,if (!$<ARGUMENT.NAME>$.empty())\n\tpushLiteralString($<ARGUMENT.NAME>$);$$
			$$Append:DYNAMIC_LENGTH,literalStringLength($<ARGUMENT.NAME>$)$$
		$$Elifeq:ARGUMENT.CATEGORY,pair$$
			$$If:PUSH_LIST$$
				$$Append:PUSH_LIST,\n$$
			$$End$$
			$$If:DYNAMIC_LENGTH$$
				$$Append:DYNAMIC_LENGTH, + $$
			$$End$$
			$$Append:PUSH_LIST,if ($<ARGUMENT.NAME>$)\n{\n\tm_Buffer.emplace_back($<ARGUMENT.NAME>$->first);\n\tm_Buffer.emplace_back($<ARGUMENT.NAME>$->second);\n}$$
			$$Append:DYNAMIC_LENGTH,($<ARGUMENT.NAME>$ ? 2 : 0)$$
		$$Elifeq:ARGUMENT.CATEGORY,enum$$
			$$If:PUSH_LIST$$
				$$Append:PUSH_LIST,\n$$
			$$End$$
			$$If:DYNAMIC_LENGTH$$
				$$Append:DYNAMIC_LENGTH, + $$
			$$End$$
			$$Append:PUSH_LIST,if ($<ARGUMENT.NAME>$ != $<ARGUMENT.TYPE>$::None)\n\tm_Buffer.emplace_back(static_cast<std::uint32_t>($<ARGUMENT.NAME>$));$$
			$$Append:DYNAMIC_LENGTH,($<ARGUMENT.NAME>$ ? 1 : 0)$$
		$$End$$
	$$Elifeq:ARGUMENT.OPTIONAL,*$$
		$$Ifeq:ARGUMENT.CATEGORY,id$$
			$$If:PUSH_LIST$$
				$$Append:PUSH_LIST,\n$$
			$$End$$
			$$If:DYNAMIC_LENGTH$$
				$$Append:DYNAMIC_LENGTH, + $$
			$$End$$
			$$Append:PUSH_LIST,m_Buffer.insert(m_Buffer.end()\, $<ARGUMENT.NAME>$.begin()\, $<ARGUMENT.NAME>$.end());$$
			$$Append:DYNAMIC_LENGTH,$<ARGUMENT.NAME>$.size()$$
		$$Elifeq:ARGUMENT.CATEGORY,int$$
			$$If:PUSH_LIST$$
				$$Append:PUSH_LIST,\n$$
			$$End$$
			$$If:DYNAMIC_LENGTH$$
				$$Append:DYNAMIC_LENGTH, + $$
			$$End$$
			$$Append:PUSH_LIST,m_Buffer.insert(m_Buffer.end()\, $<ARGUMENT.NAME>$.begin()\, $<ARGUMENT.NAME>$.end());$$
			$$Append:DYNAMIC_LENGTH,$<ARGUMENT.NAME>$.size()$$
		$$Elifeq:ARGUMENT.CATEGORY,string$$
			$$If:PUSH_LIST$$
				$$Append:PUSH_LIST,\n$$
			$$End$$
			$$If:DYNAMIC_LENGTH$$
				$$Append:DYNAMIC_LENGTH, + $$
			$$End$$
			$$Append:PUSH_LIST,for (auto& v : $<ARGUMENT.NAME>$)\n\tpushLiteralString(v);$$
			$$Append:DYNAMIC_LENGTH,[$<ARGUMENT.NAME>$]() -> std::size_t { std::size_t size = 0; for (auto& v : $<ARGUMENT.NAME>$) size += v.size(); return size; }()$$
		$$Elifeq:ARGUMENT.CATEGORY,pair$$
			$$If:PUSH_LIST$$
				$$Append:PUSH_LIST,\n$$
			$$End$$
			$$If:DYNAMIC_LENGTH$$
				$$Append:DYNAMIC_LENGTH, + $$
			$$End$$
			$$Append:PUSH_LIST,for (auto& v : $<ARGUMENT.NAME>$)\n{\n\tm_Buffer.emplace_back(v.first);\n\tm_Buffer.emplace_back(v.second);\n}$$
			$$Append:DYNAMIC_LENGTH,$<ARGUMENT.NAME>$.size() * 2$$
		$$Elifeq:ARGUMENT.CATEGORY,enum$$
			$$If:PUSH_LIST$$
				$$Append:PUSH_LIST,\n$$
			$$End$$
			$$If:DYNAMIC_LENGTH$$
				$$Append:DYNAMIC_LENGTH, + $$
			$$End$$
			$$Append:PUSH_LIST,for (auto& v : $<ARGUMENT.NAME>$)\n\tm_Buffer.emplace_back(static_cast<std::uint32_t>(v));$$
			$$Append:DYNAMIC_LENGTH,$<ARGUMENT.NAME>$.size()$$
		$$End$$
	$$Else$$
		$$Ifeq:ARGUMENT.CATEGORY,id$$
			$$If:PUSH_LIST$$
				$$Append:PUSH_LIST,\n$$
			$$End$$
			$$If:VALIDATION_LIST$$
				$$Append:VALIDATION_LIST, || $$
			$$End$$
			$$Append:PUSH_LIST,m_Buffer.push_back($<ARGUMENT.NAME>$);$$
			$$Append:VALIDATION_LIST,$<ARGUMENT.NAME>$ == 0$$
			$$Add:CONSTANT_LENGTH,1$$
		$$Elifeq:ARGUMENT.CATEGORY,int$$
			$$If:PUSH_LIST$$
				$$Append:PUSH_LIST,\n$$
			$$End$$
			$$Append:PUSH_LIST,m_Buffer.push_back($<ARGUMENT.NAME>$);$$
			$$Add:CONSTANT_LENGTH,1$$
		$$Elifeq:ARGUMENT.CATEGORY,string$$
			$$If:PUSH_LIST$$
				$$Append:PUSH_LIST,\n$$
			$$End$$
			$$Append:PUSH_LIST,pushLiteralString($<ARGUMENT.NAME>$);$$
		$$Elifeq:ARGUMENT.CATEGORY,pair$$
			$$If:PUSH_LIST$$
				$$Append:PUSH_LIST,\n$$
			$$End$$
			$$Append:PUSH_LIST,m_Buffer.push_back($<ARGUMENT.NAME>$.first);\nm_Buffer.push_back($<ARGUMENT.NAME>$.second);$$
			$$Ifeq:ARGUMENT.FIRST.CATEGORY,id$$
				$$If:VALIDATION_LIST$$
					$$Append:VALIDATION_LIST, || $$
				$$End$$
				$$Append:VALIDATION_LIST,$<ARGUMENT.NAME>$.first == 0$$
			$$End$$
			$$Ifeq:ARGUMENT.SECONDARY.CATEGORY,id$$
				$$If:VALIDATION_LIST$$
					$$Append:VALIDATION_LIST, || $$
				$$End$$
				$$Append:VALIDATION_LIST,$<ARGUMENT.NAME>$.second == 0$$
			$$End$$
			$$Add:CONSTANT_LENGTH,2$$
		$$Elifeq:ARGUMENT.CATEGORY,enum$$
			$$If:PUSH_LIST$$
				$$Append:PUSH_LIST,\n$$
			$$End$$
			$$If:REQUIRES_LIST$$
				$$Append:REQUIRES_LIST,\n$$
			$$End$$
			$$Append:PUSH_LIST,m_Buffer.push_back(static_cast<std::uint32_t>($<ARGUMENT.NAME>$));$$
			$$Append:REQUIRES_LIST,RequireCapExt$<ARGUMENT.KIND>$($<ARGUMENT.NAME>$\, m_Capabilities\, m_Extensions);$$
			$$Add:CONSTANT_LENGTH,1$$
		$$End$$
	$$End$$
$$End$$
$$Cast:CONSTANT_LENGTH,CONSTANT_LENGTH_HEX,hexstr,4$$
$$Assign:FULL_OPCODE,0x$<CONSTANT_LENGTH_HEX>$'$<OPCODE_HEX>$$$
$$If:DYNAMIC_LENGTH$$
	$$Assign:DYNAMIC_LENGTH, + static_cast<std::uint32_t>(($<DYNAMIC_LENGTH>$) << 16)$$
$$End$$
$$Assign:COMPLETE_LENGTH,$<FULL_OPCODE>$$<DYNAMIC_LENGTH>$$$
void CodeBuffer::$$Insert:NAME$$($$Insert:ARGUMENT_LIST$$) // $$Insert:OPCODE$$
{
	PROFILE_FUNC;
	$$If:VALIDATION_LIST$$

	if ($$Insert:VALIDATION_LIST$$)
		return;
	$$End$$
	$$If:REQUIRES_LIST$$

	$$Insert:REQUIRES_LIST$$
	$$End$$
	$$If:CAPABILITIES$$

	$$Foreach:CAPABILITY,CAPABILITIES$$
	requireCapability(ECapability::$$Insert:CAPABILITY$$);
	$$End$$
	$$End$$
	$$If:EXTENSIONS$$

	$$Foreach:EXTENSION,EXTENSIONS$$
	requireExtension("$$Insert:EXTENSION$$");
	$$End$$
	$$End$$

	m_Buffer.emplace_back($$Insert:COMPLETE_LENGTH$$);
	$$Insert:PUSH_LIST$$
}