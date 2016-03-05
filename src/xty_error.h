
#ifndef XTY_ERROR_H
#define XTY_ERROR_H

#define XTY_MAX_ERRNO 135

class xtyError
{
private:
	// does not allow con/decon
	xtyError();
	~xtyError();

private:
	// does not allow copying
	xtyError(const xtyError&);
	xtyError& operator = (const xtyError&);

public:
	inline static xtyError* GetInstance() { return m_pError; }
	
	inline static char* GetErrnoMsg(const int ErrorCode)
	{	
		xtyError* pError = GetInstance();
	
		if(pError == NULL)
		{
#ifdef DEBUG
			std::cout << "In xtyError::GetErrnoMsg(): ";
			std::cout << "Need to call xtyError::Create() first" << std::endl;
#endif
			exit(XTY_ERROR);
		}
		
		return pError->m_pErrnoMsgArray[ErrorCode];
	}

public:
#ifdef DEBUG
	friend class xtyErrorTest;
#endif

public:
	// added on 2015-11-11, for 'xtyFundamentalFacility'
	friend class xtyFundamentalFacility;

private:
	// singleton create function, called at the start of a program
	static void Create();
	
	// singleton destroy function, called at the end of a program
	static void Destroy();

public:
	// some fatal error happened, need to exit the process
	static void ErrorExit(const char* ErrorMsg);
	static void ErrorExit(const int ErrorCode);
	static void ErrorExit(const char* ErrorMsg, const int ErrorCode);
	
private:
	// system error msg array
	char** m_pErrnoMsgArray;
	
	// singleton members
	static xtySpinLock m_Lock;
	static xtyError* m_pError;
};

#endif

