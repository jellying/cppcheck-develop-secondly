#include "check.h"
#include "CheckPointerNULL.h"
#include "symboldatabase.h"

namespace {
	CheckPointerNULL instance;
}

void CheckPointerNULL::CheckFunctionPointerNULLUse(){
	if (!_settings->isEnabled("style"))
		return;

	// Parse all executing scopes..
	const SymbolDatabase *symbolDatabase = _tokenizer->getSymbolDatabase();
	 
	// only check functions
	const std::size_t functions = symbolDatabase->functionScopes.size();
	for (std::size_t i = 0; i < functions; ++i) {
		const Scope * scope = symbolDatabase->functionScopes[i];
		checkPointerNULLUse_iterateScopes(scope);
	}
}

void CheckPointerNULL::CheckGlobalPointerNULLUse(){
	if (!_settings->isEnabled("style"))
		return;

	// Parse all executing scopes..
	const SymbolDatabase *symbolDatabase = _tokenizer->getSymbolDatabase();

	const Scope *globalScope = symbolDatabase->findScopeByName("");
	checkPointerNULLUse_iterateScopes(globalScope);
}

void CheckPointerNULL::checkPointerNULLUse_iterateScopes(const Scope *scope){
	// Find declarations if the scope is executable..
	if (scope->isExecutable()) {
		// Find declarations
		for (std::list<Variable>::const_iterator i = scope->varlist.begin(); i != scope->varlist.end(); ++i) {
			if (i->isPointer()){
				Token *var = i->typeEndToken()->next();
				if (var->tokAt(2)->str() != var->str() || var->tokAt(3)->str() != "=")
					reportError(var, Severity::style, "pointerSuggest", "Declare '" + var->str() + "' without initialize.");
				for (const Token *nextVar = var->tokAt(4); nextVar != scope->classEnd; nextVar = nextVar->next()){
					if (nextVar->str() == var->str()){
						if ((!(Token::Match(nextVar->tokAt(-2), "if ( %var% != 0 )"))) && (!(Token::Match(nextVar->tokAt(-4), "if ( 0 != %var% )")))){
							reportError(nextVar, Severity::style, "pointerSuggest", "Haven't check if pointer is NULL when first use '" + nextVar->str() + "'.");
						}
						break;
					}
				}
				for (const Token *release = var->tokAt(4); release != scope->classEnd; release = release->next()){
					if ((Token::Match(release, "free ( %var% )") && release->tokAt(2)->str() == var->str()) ||
						(Token::Match(release, "delete %var%") && release->tokAt(1)->str() == var->str())){
						for (release = release->tokAt(4); release != scope->classEnd; release = release->next()){
							if (release->str() == var->str() && !(Token::Match(release, "%var% = 0"))){
								reportError(release, Severity::style, "pointerSuggest", "The pointer " + release->str() + " has been released when use.");
							}
						}
						break;
					}
				}
			}
		}
	}
}
