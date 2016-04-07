#ifndef CheckLogicH
#define CheckLogicH
#include "config.h"
#include "check.h"
class CheckLogic :public Check{
public:
	/** This constructor is used when registering the CheckClass */
	CheckLogic() : Check(myName()) {
	}

	/** This constructor is used when running checks. */
	CheckLogic(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger)
		: Check(myName(), tokenizer, settings, errorLogger) {
	}

	void runChecks(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger) {
		CheckLogic CheckLogic(tokenizer, settings, errorLogger);

		// Checks
		CheckLogic.CheckBracketUsage();
	}


	void runSimplifiedChecks(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger) {
		CheckLogic CheckLogic(tokenizer, settings, errorLogger);


	}

	void CheckBracketUsage();

private:

	static std::string myName() {
		return "logic";
	}

	void getErrorMessages(ErrorLogger *errorLogger, const Settings *settings) const {
		CheckLogic c(0, settings, errorLogger);
	}


	std::string classInfo() const {
		return "Check for ambiguous logic usage:\n";
	}

	bool troubleInBracket(const Token** tok);
};

#endif
