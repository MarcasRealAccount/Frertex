File: TranslationUnit

TranslationUnit: Declaration*

Declaration?:
	FunctionDeclaration

FunctionDeclaration: Attributes Typename Identifier Parameters CompoundStatement

Statements: Statement*
Statement?:
	EmptyStatement
	ExpressionStatement
	CompoundStatement
	ReturnStatement
	Declaration
EmptyStatement:      ";"
ReturnStatement:     "return" (Expression | BracedInitList)? ";"
CompoundStatement:   "{" Statements "}"
ExpressionStatement: Expression ";"

Expression?:          AssignmentExpression
AssignmentExpression: Identifier "=" InitializerClause

InitializerClause?:
	AssignmentExpression
	BracedInitList

BracedInitList: "{" InitializerClause "}"

Parameters: "(" (Parameter ("," Parameter)*)? ")"
Parameter:  Attributes Typename Identifier

Attributes: "[[" (Attribute ("," Attribute)*)? "]]"
Attribute:  Identifier Arguments

Arguments: "(" (Argument ("," Argument)*)? ")"
Argument:  Literal

Typename:       TypeQualifiers Identifier
TypeQualifiers: TypeQualifier*
TypeQualifier:
	"in"
	"out"
	"inout"

Identifier:
	NonDigit
	Identifier (NonDigit | Digit)
NonDigit?: "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J" | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | "S" | "T" | "U" | "V" | "W" | "X" | "Y" | "Z" | "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j" | "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" | "s" | "t" | "u" | "v" | "w" | "x" | "y" | "z" | "_"

Literal:
	IntegerLiteral
	FloatLiteral
	BoolLiteral

IntegerLiteral:
	BinaryLiteral IntegerSuffix?
	OctalLiteral IntegerSuffix?
	DecimalLiteral IntegerSuffix?
	HexadecimalLiteral IntegerSuffix?
BinaryLiteral?:
	("0b" |	"0B") BinaryDigit
	BinaryLiteral "'"? BinaryDigit
OctalLiteral?:
	("0o" | "0O") OctalDigit
	OctalLiteral "'"? OctalDigit
DecimalLiteral?: DigitSequence
HexadecimalLiteral?:
	HexadecimalPrefix HexadecimalDigitSequence
BinaryDigit?:      "0" | "1"
OctalDigit?:       BinaryDigit | "2" | "3" | "4" | "5" | "6" | "7"
Digit?:            OctalDigit | "8" | "9"
HexadecimalDigit?: Digit | "A" | "B" | "C" | "D" | "E" | "F" | "a" | "b" | "c" | "d" | "e" | "f"
HexadecimalPrefix?: "0x" | "0X"
HexadecimalDigitSequence?:
	HexadecimalDigit
	HexadecimalDigitSequence "'"? HexadecimalDigit
DigitSequence?:
	Digit
	DigitSequence "'"? Digit

IntegerSuffix?:
	UnsignedSuffix (ShortSuffix | LongSuffix)?
	(ShortSuffix | LongSuffix) UnsignedSuffix?
UnsignedSuffix?: "U" | "u"
ShortSuffix?:    "S" | "s"
LongSuffix?:     "L" | "l"

FloatLiteral:
	DecimalFloatLiteral
	HexadecimalFloatLiteral
DecimalFloatLiteral?:
	FractionalConstant ExponentPart? FloatSuffix?
	DigitSequence ExponentPart FloatSuffix?
HexadecimalFloatLiteral?:
	HexadecimalPrefix HexadecimalFractionalConstant BinaryExponentPart FloatSuffix?
	HexadecimalPrefix HexadecimalDigitSequence BinaryExponentPart FloatSuffix?
FractionalConstant:
	DigitSequence? "." DigitSequence
	DigitSequence "."
HexadecimalFractionalConstant?:
	HexadecimalDigitSequence? "." HexadecimalDigitSequence
	HexadecimalDigitSequence "."
ExponentPart?:       ("e" | "E") Sign? DigitSequence
BinaryExponentPart?: ("p" | "P") Sign? DigitSequence
Sign?:               "+" | "-"
FloatSuffix:         "h" | "H" | "f" | "F" | "d" | "D"

BoolLiteral: "false" | "true"