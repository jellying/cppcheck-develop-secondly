#ifndef CheckPointerNULLH
#define CheckPointerNULLH
#include "config.h"
#include "check.h"
class CheckPointerNULL :public Check{
public:
	/** This constructor is used when registering the CheckClass */
	CheckPointerNULL() : Check(myName()) {
	}

	/** This constructor is used when running checks. */
	CheckPointerNULL(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger)
		: Check(myName(), tokenizer, settings, errorLogger) {
	}

	void runChecks(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger) {
		CheckPointerNULL CheckPointerNULL(tokenizer, settings, errorLogger);

		// Checks
		CheckPointerNULL.CheckFunctionPointerNULLUse();
		CheckPointerNULL.CheckGlobalPointerNULLUse();
	}


	void runSimplifiedChecks(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger) {
		CheckPointerNULL CheckPointerNULL(tokenizer, settings, errorLogger);


	}

	void CheckFunctionPointerNULLUse();
	void CheckGlobalPointerNULLUse();

private:

	static std::string myName() {
		return "pointerNull";
	}

	void getErrorMessages(ErrorLogger *errorLogger, const Settings *settings) const {
		CheckPointerNULL c(0, settings, errorLogger);
	}


	std::string classInfo() const {
		return "Check for pointer usage:\n";
	}

	void checkPointerNULLUse_iterateScopes(const Scope *scope);
};

#endif
