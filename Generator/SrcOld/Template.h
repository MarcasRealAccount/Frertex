#pragma once

#include <cstddef>
#include <cstdint>

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

struct TemplateFunction;

struct TemplateCall
{
public:
	std::string              m_Function;
	TemplateFunction*        m_FunctionPtr;
	std::vector<std::string> m_Arguments;
	std::size_t              m_SourceIndex;
	bool                     m_HasNewline;
	std::size_t              m_Spaces;
	std::size_t              m_ScopeIndex = ~0ULL;
};

struct TemplateScope
{
public:
	std::size_t m_OpenCall;
	std::size_t m_CloseCall;

	// Inter scope stuff

	std::size_t m_PreviousScope;
	std::size_t m_NextScope;
	std::size_t m_FirstScope;
	std::size_t m_LastScope;
};

struct Template
{
public:
	std::string                m_Text;
	std::vector<TemplateCall>  m_Calls;
	std::vector<TemplateScope> m_Scopes     = {};
	std::size_t                m_LUTVersion = ~0ULL;
};

enum class ETemplateMacroType : std::uint32_t
{
	Null,
	Bool,
	Int,
	UInt,
	Float,
	Text,
	Array,
	Struct,
	Ref
};

struct TemplateMacro;
using TemplateMacroArrayType  = std::vector<TemplateMacro>;
using TemplateMacroStructType = std::unordered_map<std::string, TemplateMacro>;

template <class T>
concept TemplateMacroTypeConcept = std::is_integral_v<T> ||
								   std::is_floating_point_v<T> ||
								   std::is_same_v<T, std::string_view> ||
								   std::is_same_v<T, std::string> ||
								   std::is_same_v<T, TemplateMacroArrayType> ||
								   std::is_same_v<T, TemplateMacroStructType> ||
								   std::is_same_v<T, TemplateMacro>;

struct TemplateMacro
{
public:
	static ETemplateMacroType GetBestType(std::string_view str);

public:
	void pushFront(TemplateMacro& value);
	void pushBack(TemplateMacro& value);
	void popFront();
	void popBack();

	TemplateMacro* assignNullMacro(std::size_t index);
	TemplateMacro* assignBoolMacro(std::size_t index, bool value);
	TemplateMacro* assignIntMacro(std::size_t index, std::int64_t value);
	TemplateMacro* assignUIntMacro(std::size_t index, std::uint64_t value);
	TemplateMacro* assignFloatMacro(std::size_t index, double value);
	TemplateMacro* assignTextMacro(std::size_t index, std::string_view value);
	TemplateMacro* assignArrayMacro(std::size_t index);
	TemplateMacro* assignStructMacro(std::size_t index);
	TemplateMacro* assignRefMacro(std::size_t index, TemplateMacro& value);
	TemplateMacro* assignNullMacro(std::string_view name);
	TemplateMacro* assignBoolMacro(std::string_view name, bool value);
	TemplateMacro* assignIntMacro(std::string_view name, std::int64_t value);
	TemplateMacro* assignUIntMacro(std::string_view name, std::uint64_t value);
	TemplateMacro* assignFloatMacro(std::string_view name, double value);
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
	ETemplateMacroType m_Type = ETemplateMacroType::Null;
	std::variant<bool,
				 std::int64_t,
				 std::uint64_t,
				 double,
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

	const TemplateScope* m_Scope;
	std::size_t          m_ScopeIndex;

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
	void insertText(std::string_view text, bool includeSpaces = false);

	std::string getSourceRange();

	TemplateCallStackEntry* currentStack();
	const TemplateScope*    currentScope();
	const TemplateScope*    getScope(std::size_t scope);
	void                    exitScope();
	void                    exitScopes();
	void                    nextScope();
	void                    lastScope();

	void setStackUserData(void* userData);

	void jumpToStartOfScope();
	void jump(std::size_t call);

	TemplateMacro* assignNullMacro(std::string_view name);
	TemplateMacro* assignBoolMacro(std::string_view name, bool value);
	TemplateMacro* assignIntMacro(std::string_view name, std::int64_t value);
	TemplateMacro* assignUIntMacro(std::string_view name, std::uint64_t value);
	TemplateMacro* assignFloatMacro(std::string_view name, double value);
	TemplateMacro* assignTextMacro(std::string_view name, std::string_view value);
	TemplateMacro* assignArrayMacro(std::string_view name);
	TemplateMacro* assignStructMacro(std::string_view name);
	TemplateMacro* assignRefMacro(std::string_view name, TemplateMacro& value);

	TemplateMacro*       getMacro(std::string_view name);
	const TemplateMacro* getMacro(std::string_view name) const;

	void removeMacro(std::string_view name);

	auto getEngine() const { return m_Engine; }

	auto getTemplate() const { return m_Template; }

	auto& getCWD() const { return m_CWD; }

	auto& getSource() const { return m_Source; }

	auto getOffset() const { return m_Offset; }

	auto& getMacros() const { return m_Macros; }

	auto& getCallStack() const { return m_CallStack; }

	auto getCurrentCallIndex() const { return m_CurrentCall; }

	auto getCurrentLastCallIndex() const { return m_CurrentLastCall; }

private:
	void pushScope(std::size_t scopeIndex);
	void popScope();

private:
	TemplateEngine* m_Engine   = nullptr;
	const Template* m_Template = nullptr;

	std::string m_CWD;
	std::string m_Source;
	std::size_t m_Offset = 0;

	std::unordered_map<std::string, TemplateMacro>        m_Macros;
	std::unordered_map<std::string, const TemplateScope*> m_Functions;

	std::vector<TemplateCallStackEntry> m_CallStack;

	std::size_t m_CurrentScopeIndex = ~0ULL;
	std::size_t m_CurrentCall       = 0;
	std::size_t m_CurrentLastCall   = ~0ULL;
	bool        m_IncrementCall     = false;
	bool        m_PopScope          = true;
};

struct TemplateFunction
{
public:
	using Callback = void (*)(TemplateEnvironment& environment, const TemplateCall& call);

public:
	TemplateFunction(Callback callback, const std::string& lastCall = "", const std::vector<std::string>& interCalls = {})
		: m_Callback(callback),
		  m_InterCalls(interCalls),
		  m_LastCall(lastCall) {}

	TemplateFunction(Callback callback, std::string&& lastCall, std::vector<std::string>&& interCalls = {})
		: m_Callback(callback),
		  m_InterCalls(std::move(interCalls)),
		  m_LastCall(std::move(lastCall)) {}

	bool hasInterCalls() const { return !m_InterCalls.empty(); }

	bool hasLastCall() const { return !m_LastCall.empty(); }

public:
	Callback                 m_Callback;
	std::vector<std::string> m_InterCalls;
	std::string              m_LastCall;
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

public:
	void compileTemplate(std::string_view name);
	void updateScopesAndFunctionLUT(std::string_view name);

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

	static void Function(TemplateEnvironment& environment, const TemplateCall& call);
	static void Return(TemplateEnvironment& environment, const TemplateCall& call);

	static void For(TemplateEnvironment& environment, const TemplateCall& call);
	static void Foreach(TemplateEnvironment& environment, const TemplateCall& call);
	static void Break(TemplateEnvironment& environment, const TemplateCall& call);
	static void Continue(TemplateEnvironment& environment, const TemplateCall& call);

	static void If(TemplateEnvironment& environment, const TemplateCall& call);

	static void Add(TemplateEnvironment& environment, const TemplateCall& call);

	static void LoadObject(TemplateEnvironment& environment, const TemplateCall& call);
	static void CallTemplate(TemplateEnvironment& environment, const TemplateCall& call);
	static void Dofile(TemplateEnvironment& environment, const TemplateCall& call);

private:
	char m_CommandChar     = '$';
	char m_OpenReference   = '<';
	char m_CloseReference  = '>';
	char m_EscapeCharacter = '\\';

	std::unordered_map<std::string, Template>         m_Templates;
	std::unordered_map<std::string, TemplateFunction> m_Functions;
	std::size_t                                       m_FunctionVersion       = 0;
	bool                                              m_UpdateFunctionVersion = true;
};