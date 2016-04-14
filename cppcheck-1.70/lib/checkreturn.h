#ifndef checkreturnH
#define checkreturnH
#include "config.h"
#include "check.h"
class CheckReturn :
	public Check
{
public:
	/** This constructor is used when registering the CheckClass */
	CheckReturn() : Check(myName()) {
	}

	/** This constructor is used when running checks. */
	CheckReturn(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger)
		: Check(myName(), tokenizer, settings, errorLogger) {
	}

	void runChecks(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger) {
		CheckReturn checkReturn(tokenizer, settings, errorLogger);

		// Checks
		checkReturn.checkArgus();
		checkReturn.checkRes();
	}


	void runSimplifiedChecks(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger) {
		CheckReturn checkReturn(tokenizer, settings, errorLogger);

		checkReturn.checkType();

	}



	//检查函数返回值类型不匹配问题
	void checkType();

	//检查函数指针参数是否被被修改
	void checkArgus();

	//检查非资源性对象是否被更改
	void checkRes();

private:

	static std::string myName() {
		return "FuncReturn";
	}

	void getErrorMessages(ErrorLogger *errorLogger, const Settings *settings) const {
		CheckReturn c(0, settings, errorLogger);


	}


	std::string classInfo() const {
		return "Check for function return usage:\n";
	}

	bool isMatchType(const Token* tok, const Variable *var);

	bool isMatchNum(const Token* tok, const Token* retok);

	bool isNum(const Token* tk);

};

#endif