#include "checkconst.h"
#include "symboldatabase.h"
#include <sstream>
#include <regex>

// Register this check class (by creating a static instance of it)
namespace {
	CheckConst instance;
}

void CheckConst::checkConstUse()
{
	if (!_settings->isEnabled("style"))
		return;

	// Parse all executing scopes..
	const SymbolDatabase *symbolDatabase = _tokenizer->getSymbolDatabase();
	//检查所有常数的出现和使用
	const Token *tok;
	for (const Token *tok = _tokenizer->tokens(); tok; tok = tok->next()) 
	{
		if (!tok->isExpandedMacro() && isNum(tok))
		{
			reportError(tok, Severity::style, "constSuggest", "Suggest that use define or const variable instead of " + tok->str());
		}
	}
}

void CheckConst::checkInit()
{
	if (!_settings->isEnabled("style"))
		return;

	// Parse all executing scopes..
	const SymbolDatabase *symbolDatabase = _tokenizer->getSymbolDatabase();
	const std::size_t functions = symbolDatabase->functionScopes.size();
	const Token *tok;
	//先检查全局变量
	const Scope * scope = symbolDatabase->findScopeByName("");
	for (std::list<Variable>::const_iterator i = scope->varlist.begin(); i != scope->varlist.end(); ++i)
	{
		
		tok = i->nameToken();
		tok = tok->tokAt(-1);
		if (!i->isExtern() && !isInited(tok))
		{
			reportError(tok, Severity::style, "initSuggest", "Declare variable " + tok->tokAt(1)->str() + " without initialize.");
		}
	}
	//再检查局部变量
	for (std::size_t i = 0; i < functions; ++i) {
		const Scope * scope = symbolDatabase->functionScopes[i];
		for (std::list<Variable>::const_iterator i = scope->varlist.begin(); i != scope->varlist.end(); ++i)
		{
			tok = i->nameToken();
			tok = tok->tokAt(-1);
			if (!isInited(tok))
			{
				reportError(tok, Severity::style, "initSuggest", "Declare variable " + tok->tokAt(1)->str() + " without initialize.");
			}
		}
	}
}

//验证是超过4位的数字或字符串
bool CheckConst::isNum(const Token* tk)
{
	std::string _str = tk->str();
	const std::regex pattern1("^-?[0-9]+.?[0-9]+$");
	if (std::regex_match(_str, pattern1))
	{
		std::string last = tk->tokAt(-1)->str();
		std::string next = tk->tokAt(1)->str();
		if (_str.length() >= 4 && (strcmp(last.c_str(), "=") != 0 || strcmp(next.c_str(), ";") != 0))
			return true;
	}
	return false;
}

//检查是否声明后立刻初始化
bool CheckConst::isInited(const Token * tk)
{
	if (Token::Match(tk, "%type% %name% [;]") && tk->tokAt(1)->str() != tk->tokAt(3)->str())
		return false;
	else
		return true;
}
