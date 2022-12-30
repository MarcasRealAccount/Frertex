#!$<>\
$$Dofile:"OperandValue.tmpl"$$
$$Dofile:"OperandTypename.tmpl"$$
$$Dofile:"OperandName.tmpl"$$
$$Assign:ArgumentList$$
$$Assign:Typename$$
$$Assign:Name$$
$$Assign:Value$$
$$Foreach:Operand,Instruction.operands$$
	$$Ifn:ArgumentList$$
		$$Append:ArgumentList,\, $$
	$$End$$
	$$OperandTypename:Typename,SPIRVSpec,Operand$$
	$$OperandName:Name,SPIRVSpec,Operand$$
	$$Ifeq:Operand.quantifier,"?"$$
		$$OperandValue:Value,SPIRVSpec,Operand$$
		$$Append:ArgumentList,$<Typename>$ $<Name>$ = $<Value>$$$
	$$Elifeq:Operand.quantifier,"*"$$
		$$Append:ArgumentList,$<Typename>$ $<Name>$ = {}$$
	$$Else$$
		$$Append:ArgumentList,$<Typename>$ $<Name>$$$
	$$End$$
$$End$$
void $$Insert:Instruction.opname$$($$Insert:ArgumentList$$); // $$Insert:Instruction.opcode$$