#ifndef checkconstH
#define checkconstH
#include "config.h"
#include "check.h"


class CPPCHECKLIB CheckConst :public Check
{
public:
	/** This constructor is used when registering the CheckClass */
	CheckConst() : Check(myName()) {
	}

	/** This constructor is used when running checks. */
	CheckConst(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger)
		: Check(myName(), tokenizer, settings, errorLogger) {
	}

	void runChecks(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger) {
		CheckConst checkConst(tokenizer, settings, errorLogger);

		// Checks
		checkConst.checkConstUse();
		checkConst.checkInit();
	}
	

	void runSimplifiedChecks(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger) {
		CheckConst checkConst(tokenizer, settings, errorLogger);

	}

	//���������ʽ�еĳ���������define��const����ʹ��
	void checkConstUse();

	//����ʱ��ʼ��
	void checkInit();
private:

	static std::string myName() {
		return "Constant";
	}

	void getErrorMessages(ErrorLogger *errorLogger, const Settings *settings) const {
		CheckConst c(0, settings, errorLogger);
	

	}


	std::string classInfo() const {
		return "Check for constant usage:\n";
	}

	bool isNum(const Token * tk);

	bool isInited(const Token * tk);
};

#endif