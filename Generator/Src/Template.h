#pragma once

#include <cstddef>
#include <cstdint>

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

struct TemplateCall
{
public:
	std::string              m_Function;
	std::vector<std::string> m_Arguments;
	std::size_t              m_SourceIndex;
};

struct Template
{
public:
	std::string               m_Text;
	std::vector<TemplateCall> m_Calls;
};

enum class ETemplateMacroType : std::uint32_t
{
	Int,
	UInt,
	Text,
	Array,
	Struct,
	Ref
};

struct TemplateMacro
{
public:
	static ETemplateMacroType GetBestType(std::string_view str);

public:
	void pushFront(TemplateMacro& value);
	void pushBack(TemplateMacro& value);
	void popFront();
	void popBack();

	TemplateMacro* assignIntMacro(std::size_t index, std::int64_t value);
	TemplateMacro* assignUIntMacro(std::size_t index, std::uint64_t value);
	TemplateMacro* assignTextMacro(std::size_t index, std::string_view value);
	TemplateMacro* assignArrayMacro(std::size_t index);
	TemplateMacro* assignStructMacro(std::size_t index);
	TemplateMacro* assignRefMacro(std::size_t index, TemplateMacro& value);
	TemplateMacro* assignIntMacro(std::string_view name, std::int64_t value);
	TemplateMacro* assignUIntMacro(std::string_view name, std::uint64_t value);
	TemplateMacro* assignTextMacro(std::string_view name, std::string_view value);
	TemplateMacro* assignArrayMacro(std::string_view name);
	TemplateMacro* assignStructMacro(std::string_view name);
	TemplateMacro* assignRefMacro(std::string_view name, TemplateMacro& value);

	TemplateMacro*       get(std::size_t index);
	const TemplateMacro* get(std::size_t index) const;
	TemplateMacro*       get(std::string_view name);
	const TemplateMacro* get(std::string_view name) const;

	void remove(std::size_t index);
	void remove(std::string_view name);

public:
	ETemplateMacroType m_Type;
	std::variant<std::int64_t,
	             std::uint64_t,
	             std::string,
	             std::vector<TemplateMacro>,
	             std::unordered_map<std::string, TemplateMacro>,
	             TemplateMacro*>
	    m_Value;
};

struct TemplateCallStackEntry
{
public:
	const TemplateCall* m_StartCall;
	const TemplateCall* m_EndCall;

	std::size_t m_StartCallIndex;
	std::size_t m_EndCallIndex;

	void* m_UserData;
};

class TemplateEngine;

struct TemplateEnvironment
{
public:
	friend TemplateEngine;

public:
	void resolveReferences(std::string& str);

	void eraseRange();
	void insertText(std::string_view text);

	std::string getSourceRange();

	TemplateCallStackEntry* currentStack();

	void pushStack(std::size_t start, std::size_t end, void* userData);
	void popStack();

	void jump(std::size_t call);

	std::size_t findNextCall(std::string_view callName);

	TemplateMacro* assignIntMacro(std::string_view name, std::int64_t value);
	TemplateMacro* assignUIntMacro(std::string_view name, std::uint64_t value);
	TemplateMacro* assignTextMacro(std::string_view name, std::string_view value);
	TemplateMacro* assignArrayMacro(std::string_view name);
	TemplateMacro* assignStructMacro(std::string_view name);
	TemplateMacro* assignRefMacro(std::string_view name, TemplateMacro& value);

	TemplateMacro*       getMacro(std::string_view name);
	const TemplateMacro* getMacro(std::string_view name) const;

	void removeMacro(std::string_view name);

	auto  getEngine() const { return m_Engine; }
	auto  getTemplate() const { return m_Template; }
	auto& getSource() const { return m_Source; }
	auto  getOffset() const { return m_Offset; }
	auto& getMacros() const { return m_Macros; }
	auto& getCallStack() const { return m_CallStack; }
	auto  getCurrentCallIndex() const { return m_CurrentCall; }
	auto  getCurrentLastCallIndex() const { return m_CurrentLastCall; }

private:
	TemplateEngine* m_Engine   = nullptr;
	const Template* m_Template = nullptr;

	std::string m_Source;
	std::size_t m_Offset = 0;

	std::unordered_map<std::string, TemplateMacro> m_Macros;

	std::vector<TemplateCallStackEntry> m_CallStack;

	std::size_t m_CurrentCall     = 0;
	std::size_t m_CurrentLastCall = ~0ULL;
	bool        m_IncrementCall   = false;
};

struct TemplateFunction
{
public:
	using Callback = void (*)(TemplateEnvironment& environment, const TemplateCall& call);

public:
	TemplateFunction(Callback callback, Callback endCallback = nullptr)
	    : m_Callback(callback), m_EndCallback(endCallback) {}

	bool hasEndCall() const { return m_EndCallback; }

public:
	Callback m_Callback;
	Callback m_EndCallback;
};

class TemplateEngine
{
public:
	friend TemplateEnvironment;

public:
	TemplateEngine();

	void setChars(char commandChar, char openReference = '<', char closeReference = '>', char escapeCharacter = '\\')
	{
		m_CommandChar     = commandChar;
		m_OpenReference   = openReference;
		m_CloseReference  = closeReference;
		m_EscapeCharacter = escapeCharacter;
	}

	std::string executeTemplate(std::string_view name, TemplateEnvironment& environment);

	void addFunction(std::string_view name, TemplateFunction function);
	void removeFunction(std::string_view name);

	auto& getTemplates() const { return m_Templates; }

private:
	TemplateFunction* getFunction(std::string_view name);

	void invoke(std::string_view name, TemplateEnvironment& environment, const TemplateCall& call);
	void invokeEnd(std::string_view name, TemplateEnvironment& environment, const TemplateCall& call);

public:
	void compileTemplate(std::string_view name);

private:
	static void Insert(TemplateEnvironment& environment, const TemplateCall& call);
	static void Assign(TemplateEnvironment& environment, const TemplateCall& call);
	static void Unassign(TemplateEnvironment& environment, const TemplateCall& call);
	static void Append(TemplateEnvironment& environment, const TemplateCall& call);
	static void PushFront(TemplateEnvironment& environment, const TemplateCall& call);
	static void PushBack(TemplateEnvironment& environment, const TemplateCall& call);
	static void PopFront(TemplateEnvironment& environment, const TemplateCall& call);
	static void PopBack(TemplateEnvironment& environment, const TemplateCall& call);
	static void Cast(TemplateEnvironment& environment, const TemplateCall& call);
	static void For(TemplateEnvironment& environment, const TemplateCall& call);
	static void Foreach(TemplateEnvironment& environment, const TemplateCall& call);
	static void If(TemplateEnvironment& environment, const TemplateCall& call);

private:
	char m_CommandChar     = '$';
	char m_OpenReference   = '<';
	char m_CloseReference  = '>';
	char m_EscapeCharacter = '\\';

	std::unordered_map<std::string, Template>         m_Templates;
	std::unordered_map<std::string, TemplateFunction> m_Functions;
};