
#include "xty.h"

xtySpinLock xtyError::m_Lock;
xtyError* xtyError::m_pError = NULL;

xtyError::xtyError()
{
#ifdef DEBUG
	std::cout << "In xtyError::xtyError()" << std::endl;
#endif
	int err;
	char* err_msg;
	size_t err_len;
	
	m_pErrnoMsgArray = (char**) malloc(sizeof(char*) * XTY_MAX_ERRNO);
	
	if (m_pErrnoMsgArray == NULL)
	{
		err = errno;
		std::cout << "In xtyError::xtyError(): " << err << " " << strerror(err) << std::endl;
		exit(XTY_ERROR); 
	}
	
	int i;

	for (i = 0; i < XTY_MAX_ERRNO; i++)
	{
		err_msg = strerror(i);
		err_len = strlen(err_msg);
		
		// alloc one more byte for '\0'
		m_pErrnoMsgArray[i] = (char*) malloc(err_len + 1);
		
		if (m_pErrnoMsgArray[i] == NULL)
		{
			err = errno;
			std::cout << "In xtyError::xtyError(): " << err << " " << strerror(err) << std::endl;
			exit(XTY_ERROR); 
		}

		// copy the system error msg to 'm_pErrnoMsgArray'
		memcpy(m_pErrnoMsgArray[i], err_msg, err_len);
		
		// set the last byte to be '\0'
		m_pErrnoMsgArray[i][err_len] = '\0';
	}
}

xtyError::~xtyError()
{
#ifdef DEBUG
	std::cout << "In xtyError::~xtyError()" << std::endl;
#endif
	int i;

	for (i = 0; i < XTY_MAX_ERRNO; i++)
	{
		free(m_pErrnoMsgArray[i]);
		m_pErrnoMsgArray[i] = NULL;
	}
	
	free(m_pErrnoMsgArray);
	m_pErrnoMsgArray = NULL;
}

void xtyError::Create()
{
	if (m_pError == NULL)
	{
		m_Lock.Lock();

		// double-check for high efficiency
		if (m_pError == NULL)
			m_pError = new xtyError;

		m_Lock.UnLock();
	}
}

void xtyError::Destroy()
{
	if (m_pError != NULL)
	{
		m_Lock.Lock();

		// double-check for high efficiency
		if (m_pError != NULL)
		{
			delete m_pError;
			m_pError = NULL;
		}

		m_Lock.UnLock();
	}
}

void xtyError::ErrorExit(const char* ErrorMsg)
{
	if (ErrorMsg != 0)
		xtyLog::WriteLog("Error: %s", ErrorMsg);

	exit(XTY_ERROR);
}

void xtyError::ErrorExit(const int ErrorCode)
{
	if (ErrorCode != 0)
		xtyLog::WriteLog("Error: %s", GetErrnoMsg(ErrorCode));

	exit(XTY_ERROR);
}

void xtyError::ErrorExit(const char* ErrorMsg, const int ErrorCode)
{
	if (ErrorMsg != 0 && ErrorCode != 0)
		xtyLog::WriteLog("Error: %s, %s", ErrorMsg, GetErrnoMsg(ErrorCode));
		
	else if (ErrorMsg != 0)
		xtyLog::WriteLog("Error: %s", ErrorMsg);
		
	else if (ErrorCode != 0)
		xtyLog::WriteLog("Error: %s", GetErrnoMsg(ErrorCode));

	exit(XTY_ERROR);
}

