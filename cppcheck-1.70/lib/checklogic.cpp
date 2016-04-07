#include "check.h"
#include "checklogic.h"

namespace {
	CheckLogic instance;
}

void CheckLogic::CheckBracketUsage(){
	if (!_settings->isEnabled("style"))
		return;
	int level = 0;
	for (const Token *tok = _tokenizer->tokens(); tok; tok = tok->next()) {
		if (Token::Match(tok, "("))
			level++;
		if (Token::Match(tok, ")"))
			level--;
		if (Token::Match(tok, "%or%|%oror%|&|&&|!|^|==|!=")){
			while (level != 0){
				if (Token::Match(tok->previous(), "(")){
					level--;
				}
				tok = tok->previous();
			}
			if (tok->str() == "(")
				tok = tok->next();
			if (troubleInBracket(&tok)){
				reportError(tok, Severity::style, "logisticSuggest", "Logistic order is ambiguous without bracket.");
			}
		}
	}
}

bool CheckLogic::troubleInBracket(const Token** tok){
	int flag = 0;                                    //logic ops in this bracket
	while (*tok) {
		if (Token::Match(*tok, ")|;")){
			return false;
		}
		else{
			if (Token::Match(*tok, "%or%|%oror%|&|&&|!|^|==|!=")){
				if (++flag > 1){
					return true;
				}
			}
			if (Token::Match(*tok, "(")){
				*tok = (*tok)->next();
				if (troubleInBracket(tok))
					return true;
			}
			*tok = (*tok)->next();
		}
	}
	return false;
}
