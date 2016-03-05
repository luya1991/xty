
#ifndef XTY_EXCEPTION_H
#define XTY_EXCEPTION_H

typedef enum
{
	XTY_DULL_EXCEPTION = 0,
	XTY_EMPTY_QUEUE = 1,
	XTY_NULL_POINTER = 2,
	XTY_OUT_OF_BUFFER_RANGE = 3,
	XTY_INVALID_ITERATOR = 4,
	XTY_INVALID_ARRAY_INDEX = 5,
}xtyExceptionCode;

static const char* xtyExceptionMsgArray[] =
{
	"Warning: Dull exception just for test",
	"Warning: Try to dequeue an empty queue",
	"Warning: Try to pass 'NULL' to a pointer which cannot be 'NULL'",
	"Warning: Try to access data that is out of buffer's range",
	"Warning: Try to operate an invalid iterator",
	"Warning: Try to access an element of array with invalid index",
	NULL,
};

class xtyException
{
public:
	explicit xtyException(const xtyExceptionCode& Code)
	: m_ExceptionCode(Code), m_ExceptionMsg(xtyExceptionMsgArray[Code])
	{/* does nothing but need a definition */}
	
	virtual ~xtyException()
	{/* does nothing but need a definition */}

public:
	// allow copying, because 'try-catch' always causes copying behavior
	xtyException(const xtyException& e)
	{
		m_ExceptionCode = e.m_ExceptionCode;
		m_ExceptionMsg = e.m_ExceptionMsg;
	}
	
	xtyException& operator = (const xtyException& e)
	{
		m_ExceptionCode = e.m_ExceptionCode;
		m_ExceptionMsg = e.m_ExceptionMsg;
		
		return *this;
	}
	
public:
	inline void Dump() { std::cout << m_ExceptionMsg << std::endl; }

private:
	xtyExceptionCode m_ExceptionCode;
	std::string m_ExceptionMsg;
};

#endif

