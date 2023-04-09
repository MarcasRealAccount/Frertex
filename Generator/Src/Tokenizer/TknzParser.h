#pragma once

#include <string>
#include <variant>
#include <vector>

enum class ETknzPredicateType
{
	Class,
	Char,
	ExtraState
};

struct TknzPredicate
{
	ETknzPredicateType Type;
	bool               Inverse;

	std::variant<std::uint64_t, std::string> Statement;
};

enum class ETknzResultType
{
	BaseState,
	ExtraState,
	Transition
};

struct TknzResult
{
	ETknzResultType Type;

	std::variant<std::uint64_t, std::string> Statement;
};

struct TknzRule
{
	std::vector<TknzPredicate> Predicates;
	std::vector<TknzResult>    Results;
};

struct TknzClass
{
	std::string           Name;
	bool                  Excluded;
	std::vector<TknzRule> Rules;
};

struct Tknz
{
public:
	std::vector<TknzClass> TokenClasses;

	std::string   StartTokenClass;
	std::string   Charset;
	std::uint64_t ExtraStates;
	std::uint64_t SizePerState;
};

static constexpr std::uint8_t TknzResultStateStep  = 0x01;
static constexpr std::uint8_t TknzResultStateEnd   = 0x02;
static constexpr std::uint8_t TknzResultStateError = 0x04;

Tknz              TknzParse(std::string_view source);
std::string_view* TknzGetCharClasses(Tknz& tknz);

TknzRule* TknzMatchRule(Tknz& tknz, TknzClass& clazz, std::uint64_t character, std::uint64_t extraState);

std::uint64_t TknzRuleBaseState(Tknz& tknz, TknzClass& clazz, TknzRule& rule);
std::uint64_t TknzRuleExtraState(Tknz& tknz, TknzClass& clazz, TknzRule& rule);
std::uint64_t TknzRuleTransition(Tknz& tknz, TknzClass& clazz, TknzRule& rule);