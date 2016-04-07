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
			for (retok = retok->next(); retok != func->tokenDef; retok = retok->next())
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
		for (tok = scope->classStart; tok != scope->classEnd ; tok = tok->next())
		{
			if (tok->str() == "return")
			{
				hasreturn = 1;
				tok = tok->next();
				for (; tok->str() == "("; tok = tok->next());
				//�ж��ǳ������Ǳ���
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
				//�����������Ϊconst
				if (isconst == 1)
				{
					if (var->isConst())
					{
						//��鷵��ֵ���Ͳ�ƥ��
						if (!isMatchType(retok, var))
						{
							reportError(tok, Severity::warning, "returnWarning", "Return type doesn't match.");
						}
					}
					else
					{
						//const����
						reportError(tok, Severity::warning, "returnWarning", "Return type should be const type.");
					}
				}
				else if (!isMatchType(retok, var))
				{
					//��鷵��ֵ���Ͳ�ƥ��
					reportError(tok, Severity::warning, "returnWarning", "Return type doesn't match.");
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

//��鷵�صı�������
bool CheckReturn::isMatchType(const Token * tok, const Variable * var)
{
	//�ҵ�������ǰ����������
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
		//��������ṹ���ͷ���ֵ
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
