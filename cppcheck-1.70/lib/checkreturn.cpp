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
	//�������к�����������ṹ��Ա����
	for (std::size_t i = 0; i < functions; ++i)
	{
		const Scope * scope = symbolDatabase->functionScopes[i];
		const Function * func = scope->function;
		retok = func->retDef;
		hasreturn = 0;
		isconst = 0;
		//�������Ϊ���캯��ֱ������
		if (func->isConstructor())
			continue;
		else if (retok->str() == "const")
		{
			//��¼�ú�������ֵΪconst����
			isconst = 1;
			//��¼���ҵ���Ӧ��typeToken
			for (retok = func->tokenDef->previous(); retok != func->retDef; retok = retok->previous())
			{
				if (Token::Match(retok, "%type%"))
				{
					break;
				}
			}
		}
		//�޷���ֵ������ֱ������
		if (retok->str() == "void")
		{
			continue;
		}
		//struct���ͺ�class����ͬ�࣬��struct����ֱ��ȡ�ýṹ����
		if (retok->str() == "struct")
		{
			retok = retok->next();
		}
		if (retok->str() == "std")
		{
			retok = retok->tokAt(2);
		}
		//�ҵ�return�ؼ���
		for (tok = scope->classStart; tok != scope->classEnd; tok = tok->next())
		{
			if (tok->str() == "return")
			{
				hasreturn = 1;
				tok = tok->next();
				for (; !(tok->isLiteral() || tok->isName()); tok = tok->next());
				//�ж��Ǻ������Ǳ������ǳ��������Ǳ��ʽ
				if (tok->type())
				{
					//������µ������
					if (retok->str() != tok->str())
					{
						reportError(tok, Severity::warning, "returnWarning", "Return type doesn't match.");
					}
				}
				else if (tok->tokType() == 2)
				{
					//�Ǻ���
					retfun = tok->function();
					const Token* retfuntok;
					for (retfuntok = retfun->tokenDef->previous(); retfuntok != retfun->retDef; retfuntok = retfuntok->previous())
					{
						if (Token::Match(retfuntok, "%type%"))
						{
							break;
						}
					}
					//�����������Ϊconst
					if ((isconst == 1 && retfun->retDef->str() == "const") || (isconst==0 && retfun->retDef->str() != "const"))
					{
						if (retok->str() != retfuntok->str())
						{
							reportError(tok, Severity::warning, "returnWarning", "Return type doesn't match.");
						}
					}
					else
					{
						//���constƥ��
						reportError(tok, Severity::warning, "returnWarning", "Return type should be const type.");
					}
				}
				else if (!tok->isLiteral())
				{
					//�Ǳ�����
					var = tok->variable();
					//�����������Ϊconst
					if ((isconst == 1 && var->isConst()) || (isconst==0 && !var->isConst()))
					{
						if (!isMatchType(retok,var))
						{
							reportError(tok, Severity::warning, "returnWarning", "Return type doesn't match.");
						}
					}
					else
					{
						//���const����
						reportError(tok, Severity::warning, "returnWarning", "Return type should be const type.");
					}
				}
				else
				{
					//�ǳ���
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
			//�޷���ֵ����
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
	//�������к�����������ṹ��Ա����
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
					//����ָ�뱻�޸�
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
				//�ҵ�return�ؼ���
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

//��鷵�صı�������
bool CheckReturn::isMatchType(const Token * tok, const Variable * var)
{
	//�ҵ�������ǰ����������
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
		//��������ṹ���ͷ���ֵ
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

//��鳣����Ӧ�ķ���ֵ
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
		//��������ṹ���ͷ���ֵ��������Ȼ��ƥ��
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
