#include "check.h"
#include "checkreturn.h"
#include "symboldatabase.h"
#include <regex>

namespace {
	CheckReturn instance;
}

void CheckReturn::checkType()
{
	if (!_settings->isEnabled("warning"))
		return;

	int hasreturn, isconst;
	const SymbolDatabase *symbolDatabase = _tokenizer->getSymbolDatabase();
	const Token *tok,* retok;
	const std::size_t functions = symbolDatabase->functionScopes.size();
	const Variable* var;
	//遍历所有函数，包括类结构成员函数
	for (std::size_t i = 0; i < functions; ++i) 
	{
		const Scope * scope = symbolDatabase->functionScopes[i];
		const Function * func = scope->function;
		retok = func->retDef;
		hasreturn = 0;
		isconst = 0;
		//如果函数为构造函数直接跳过
		if (func->isConstructor())
			continue;
		else if (retok->str() == "const")
		{
			//记录该函数返回值为const类型
			isconst = 1;
			//记录后找到对应的typeToken
			for (retok = retok->next(); retok != func->tokenDef; retok = retok->next())
			{
				if (Token::Match(retok, "%type%"))
				{
					break;
				}
			}
		}
		//无返回值类型则直接跳过
		if (retok->str() == "void")
		{
			continue;
		}
		//struct类型和class看作同类，将struct跳过直接取得结构名称
		if (retok->str() == "struct")
		{
			retok = retok->next();
		}
		if (retok->str() == "std")
		{
			retok = retok->tokAt(2);
		}
		//找到return关键字
		for (tok = scope->classStart; tok != scope->classEnd ; tok = tok->next())
		{
			if (tok->str() == "return")
			{
				hasreturn = 1;
				tok = tok->next();
				for (; tok->str() == "("; tok = tok->next());
				//判断是常数还是变量
				if(!tok->isLiteral())
					var = tok->variable();
				else
				{
					if (!isMatchNum(tok, retok))
					{
						reportError(tok, Severity::warning, "returnWarning", "Return type doesn't match.");
					}
					continue;
				}
				//如果函数类型为const
				if (isconst == 1)
				{
					if (var->isConst())
					{
						//检查返回值类型不匹配
						if (!isMatchType(retok, var))
						{
							reportError(tok, Severity::warning, "returnWarning", "Return type doesn't match.");
						}
					}
					else
					{
						//const问题
						reportError(tok, Severity::warning, "returnWarning", "Return type should be const type.");
					}
				}
				else if (!isMatchType(retok, var))
				{
					//检查返回值类型不匹配
					reportError(tok, Severity::warning, "returnWarning", "Return type doesn't match.");
				}
			}
		}
		if (hasreturn == 0)
		{
			//无返回值问题
			reportError(retok, Severity::warning, "returnWarning", "The function doesn't have a return value.");
		}
	}
}

//检查返回的变量类型
bool CheckReturn::isMatchType(const Token * tok, const Variable * var)
{
	//找到变量名前的声明类型
	const Token* varType=var->nameToken()->previous();
	if (varType->str() == "*")
	{
		varType = varType->previous();
	}
	if (tok->str() == "int")
	{
		if (!var->isIntegralType())
		{
			return false;
		}
	}
	else if (tok->str() == "float" || tok->str() == "double")
	{
		if (!var->isFloatingType())
		{
			return false;
		}
	}
	else if(tok->tokType()==1)
	{
		//如果是类或结构类型返回值
		if(tok->str()!=var->nameToken()->previous()->str())
		{
			return false;
		}
	}
	else if (tok->str() == "bool")
	{
		if (varType->str()!="bool" )
		{
			return false;
		}
	}
	else if (tok->str() == "char")
	{
		if (varType->str() != "char")
		{
			return false;
		}
	}
	else if (tok->str() == "string")
	{
		if (!var->isStlStringType())
		{
			return false;
		}
	}
	return true;
}

//检查常量对应的返回值
bool CheckReturn::isMatchNum(const Token * tok, const Token * retok)
{
	if (retok->str() == "int")
	{
		if (isNum(tok) || (!tok->isNumber()))
		{
			return false;
		}
	}
	else if (retok->str() == "float" || retok->str() == "double")
	{
		if (!isNum(tok) || (!tok->isNumber()))
		{
			return false;
		}
	}
	else if (retok->tokType() == 1)
	{
		//如果是类或结构类型返回值，常量必然不匹配
		return false;
	}
	else if (retok->str() == "bool")
	{
		if (!tok->isBoolean())
		{
			return false;
		}
	}
	else if (retok->str() == "char")
	{
		if (tok->tokType()!=7)
		{
			return false;
		}
	}
	else if (retok->str() == "string")
	{
		if (tok->tokType()!=6)
		{
			return false;
		}
	}
	return true;
}

bool CheckReturn::isNum(const Token * tk)
{
	std::string _str = tk->str();
	const std::regex pattern1("^-?[0-9]+.{1}?[0-9]+$");
	if (std::regex_match(_str, pattern1))
	{
		return true;
	}
	return false;
}
