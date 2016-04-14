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
	const Token *tok, *retok;
	const std::size_t functions = symbolDatabase->functionScopes.size();
	const Variable* var;
	const Function* retfun;
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
			for (retok = func->tokenDef->previous(); retok != func->retDef; retok = retok->previous())
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
		for (tok = scope->classStart; tok != scope->classEnd; tok = tok->next())
		{
			if (tok->str() == "return")
			{
				hasreturn = 1;
				tok = tok->next();
				for (; !(tok->isLiteral() || tok->isName()); tok = tok->next());
				//判断是函数，是变量还是常量，还是表达式
				if (tok->type())
				{
					//如果是新的类对象
					if (retok->str() != tok->str())
					{
						reportError(tok, Severity::warning, "returnWarning", "Return type doesn't match.");
					}
				}
				else if (tok->tokType() == 2)
				{
					//是函数
					retfun = tok->function();
					const Token* retfuntok;
					for (retfuntok = retfun->tokenDef->previous(); retfuntok != retfun->retDef; retfuntok = retfuntok->previous())
					{
						if (Token::Match(retfuntok, "%type%"))
						{
							break;
						}
					}
					//如果函数类型为const
					if ((isconst == 1 && retfun->retDef->str() == "const") || (isconst==0 && retfun->retDef->str() != "const"))
					{
						if (retok->str() != retfuntok->str())
						{
							reportError(tok, Severity::warning, "returnWarning", "Return type doesn't match.");
						}
					}
					else
					{
						//检查const匹配
						reportError(tok, Severity::warning, "returnWarning", "Return type should be const type.");
					}
				}
				else if (!tok->isLiteral())
				{
					//是变量名
					var = tok->variable();
					//如果函数类型为const
					if ((isconst == 1 && var->isConst()) || (isconst==0 && !var->isConst()))
					{
						if (!isMatchType(retok,var))
						{
							reportError(tok, Severity::warning, "returnWarning", "Return type doesn't match.");
						}
					}
					else
					{
						//检查const问题
						reportError(tok, Severity::warning, "returnWarning", "Return type should be const type.");
					}
				}
				else
				{
					//是常量
					if (!isMatchNum(tok, retok))
					{
						reportError(tok, Severity::warning, "returnWarning", "Return type doesn't match.");
					}
					continue;
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

void CheckReturn::checkArgus()
{
	if (!_settings->isEnabled("warning"))
		return;

	int hasreturn, isconst;
	const SymbolDatabase *symbolDatabase = _tokenizer->getSymbolDatabase();
	const Token *tok, *argtok;
	const std::size_t functions = symbolDatabase->functionScopes.size();
	const Variable* var;
	//遍历所有函数，包括类结构成员函数
	for (std::size_t i = 0; i < functions; ++i)
	{
		const Scope * scope = symbolDatabase->functionScopes[i];
		const Function * func = scope->function;
		for (std::list<Variable>::const_iterator i = func->argumentList.begin(); i != func->argumentList.end(); i++)
		{
			if (!i->isPointer())
			{
				continue;
			}
			for (tok = scope->classStart; tok != scope->classEnd; tok = tok->next())
			{
				if (tok->str() == i->name() && tok->next()->str()=="=")
				{
					//参数指针被修改
					reportError(tok, Severity::warning, "arguWarning", "The function paramater pointer is altered.");
				}
			}
		}
	}
}

void CheckReturn::checkRes()
{
	if (!_settings->isEnabled("warning"))
		return;
	const SymbolDatabase *symbolDatabase = _tokenizer->getSymbolDatabase();
	const Token *tok;
	const std::size_t classes = symbolDatabase->classAndStructScopes.size();
	const Variable* var;
	for (std::size_t i = 0; i < classes; ++i)
	{
		const Scope * scope = symbolDatabase->classAndStructScopes[i];
		for (std::list<Variable>::const_iterator i = scope->varlist.begin(); i != scope->varlist.end(); i++)
		{
			if (i->isStatic() || !(i->isPrivate()))
			{
				continue;
			}
			for (std::list<Function>::const_iterator fun = scope->functionList.begin(); fun != scope->functionList.end(); fun++)
			{
				if (fun->isConstructor() || fun->retDef->str() == "const" )
				{
					continue;
				}
				//找到return关键字
				for (tok = fun->functionScope->classStart; tok != fun->functionScope->classEnd; tok = tok->next())
				{
					if (tok->str() == "return")
					{
						for (tok = tok->next(); tok->str() != ";"; tok = tok->next())
						{
							if (tok->str() == i->name() && (tok->previous()->str()=="&" || i->isPointer()))
							{
								reportError(i->nameToken(), Severity::warning, "unresChangeWarning", "The private variable "+i->name() + " may be altered outside.");
							}
						}
					}
				}
			}
		}
	}
}

//检查返回的变量类型
bool CheckReturn::isMatchType(const Token * tok, const Variable * var)
{
	//找到变量名前的声明类型
	const Token* varType = var->nameToken()->previous();
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
	else if (tok->tokType() == 1)
	{
		//如果是类或结构类型返回值
		if (tok->str() != var->nameToken()->previous()->str())
		{
			return false;
		}
	}
	else if (tok->str() == "bool")
	{
		if (varType->str() != "bool")
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
		if (!tok->isBoolean() || !(tok->str()=="1" || tok->str() == "0"))
		{
			return false;
		}
	}
	else if (retok->str() == "char")
	{
		if (tok->tokType() != 7)
		{
			return false;
		}
	}
	else if (retok->str() == "string")
	{
		if (tok->tokType() != 6)
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
