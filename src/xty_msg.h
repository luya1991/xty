
#ifndef XTY_MSG_H
#define XTY_MSG_H

typedef enum
{
	XTY_DULL_MSG = 0,
	XTY_QUIT_MSG = 1,
	XTY_REG_MSG = 2,
	XTY_DEREG_MSG = 3,
	XTY_DATA_MSG = 4,
}xtyMsgType;
	
struct xtyMsg
{
public:
	xtyMsg() : m_MsgLen(0)
	{
#ifdef DEBUG
		// std::cout << "In xtyMsg::xtyMsg()" << std::endl;
#endif
	}
	
	explicit xtyMsg(const xtyMsgType& MsgType) : m_MsgType(MsgType), m_MsgLen(0)
	{
#ifdef DEBUG
		// std::cout << "In xtyMsg::xtyMsg()" << std::endl;
#endif
	}
	
	virtual ~xtyMsg()
	{
#ifdef DEBUG
		// std::cout << "In xtyMsg::~xtyMsg()" << std::endl;
#endif
	}

public:
	// 2015-11-07
	// because 'xtyMsgDeSerializer' delete 'xtyMsg*' every time before
	// deserialization, I have to set copying operations to be 'public'
	xtyMsg(const xtyMsg& msg) : m_MsgType(msg.m_MsgType), m_MsgLen(msg.m_MsgLen)
	{
		// does nothing
	}
	
	xtyMsg& operator = (const xtyMsg& msg)
	{
		m_MsgType = msg.m_MsgType;
		m_MsgLen = msg.m_MsgLen;
		
		return *this;
	}

public:	
	// 2015-11-07
	// dangerous(new-inside-delete-outside)
	// but I think it is also necessary, especially
	// when temp object cannot satisfy the condition
	inline virtual xtyMsg* Clone() = 0;

	// 2016-01-06
	// calculate msg length before serialization
	inline virtual void CalculateSerializedMsgLen() = 0;
	
	// overload operator '<<' for message-struct's output
	inline friend std::ostream& operator << (std::ostream& out, const xtyMsg& msg)
	{
		out << "m_MsgType: " << msg.m_MsgType << std::endl;
		out << "m_MsgLen: " << msg.m_MsgLen << std::endl;
		return out;
	}

public:
	// 2015-11-04
	// add pure virtual serial/deserial interfaces for each msg-struct
	virtual void Serialize(char*& pSerializeIterator, char* const& pSerializeBufEnd) = 0;
	virtual void DeSerialize(char*& pDeSerializeIterator, char* const& pDeSerializeBufEnd) = 0;
	
public:
	// 2015-10-15
	// set all the msg's fields to be 'public' for syntactic sugar
	xtyMsgType m_MsgType;
	size_t m_MsgLen;
};

struct xtyDullMsg : public xtyMsg
{
public:
	xtyDullMsg() : xtyMsg(XTY_DULL_MSG)
	{
#ifdef DEBUG
		std::cout << "In xtyDullMsg::xtyDullMsg()" << std::endl;
#endif
	}
	
	virtual ~xtyDullMsg()
	{
#ifdef DEBUG
		std::cout << "In xtyDullMsg::~xtyDullMsg()" << std::endl;
#endif
	}

public:
	xtyDullMsg(const xtyDullMsg& msg) : xtyMsg(msg)
	{
		// does nothing
	}
	
	xtyDullMsg& operator = (const xtyDullMsg& msg)
	{
		m_MsgType = msg.m_MsgType;
		m_MsgLen = msg.m_MsgLen;
		
		return *this;
	}

public:	
	// 2015-11-07
	// dangerous(new-inside-delete-outside)
	// but I think it is also necessary especially
	// when temp object cannot satisfy the condition
	inline virtual xtyMsg* Clone()
	{
		xtyDullMsg* pCloneMsg = new xtyDullMsg;
		
		pCloneMsg->m_MsgType = m_MsgType;
		pCloneMsg->m_MsgLen = m_MsgLen;
		
		return pCloneMsg;
	}

	// 2016-01-06
	// calculate msg length before serialization
	inline virtual void CalculateSerializedMsgLen()
	{
		m_MsgLen = sizeof(m_MsgType) + sizeof(m_MsgLen);
	}
	
public:	
	// overload operator '<<' for message-struct's output
	inline friend std::ostream& operator << (std::ostream& out, const xtyDullMsg& msg)
	{
		out << "m_MsgType: " << msg.m_MsgType << std::endl;
		out << "m_MsgLen: " << msg.m_MsgLen << std::endl;
		return out;
	}

public:
	// 2015-11-04
	// add virtual serial/deserial interfaces for each msg-struct
	virtual void Serialize(char*& pSerializeIterator, char* const& pSerializeBufEnd);
	virtual void DeSerialize(char*& pDeSerializeIterator, char* const& pDeSerializeBufEnd);
};

struct xtyQuitMsg : public xtyMsg
{
public:
	xtyQuitMsg() : xtyMsg(XTY_QUIT_MSG)
	{
#ifdef DEBUG
		std::cout << "In xtyQuitMsg::xtyQuitMsg()" << std::endl;
#endif
	}
	
	virtual ~xtyQuitMsg()
	{
#ifdef DEBUG
		std::cout << "In xtyQuitMsg::~xtyQuitMsg()" << std::endl;
#endif
	}

public:
	xtyQuitMsg(const xtyQuitMsg& msg) : xtyMsg(msg)
	{
		// does nothing
	}
	
	xtyQuitMsg& operator = (const xtyQuitMsg& msg)
	{
		m_MsgType = msg.m_MsgType;
		m_MsgLen = msg.m_MsgLen;
		
		return *this;
	}

public:	
	// 2015-11-07
	// dangerous(new-inside-delete-outside)
	// but I think it is also necessary especially
	// when temp object cannot satisfy the condition
	inline virtual xtyMsg* Clone()
	{
		xtyQuitMsg* pCloneMsg = new xtyQuitMsg;
		
		pCloneMsg->m_MsgType = m_MsgType;
		pCloneMsg->m_MsgLen = m_MsgLen;
		
		return pCloneMsg;
	}
	
	// 2016-01-06
	// calculate msg length before serialization
	inline virtual void CalculateSerializedMsgLen()
	{
		m_MsgLen = sizeof(m_MsgType) + sizeof(m_MsgLen);
	}

public:	
	// overload operator '<<' for message-struct's output
	inline friend std::ostream& operator << (std::ostream& out, const xtyQuitMsg& msg)
	{
		out << "m_MsgType: " << msg.m_MsgType << std::endl;
		out << "m_MsgLen: " << msg.m_MsgLen << std::endl;
		return out;
	}

public:
	// 2015-11-04
	// add virtual serial/deserial interfaces for each msg-struct
	virtual void Serialize(char*& pSerializeIterator, char* const& pSerializeBufEnd);
	virtual void DeSerialize(char*& pDeSerializeIterator, char* const& pDeSerializeBufEnd);
};

struct xtyRegMsg : public xtyMsg
{
public:
	xtyRegMsg() : xtyMsg(XTY_REG_MSG)
	{
#ifdef DEBUG
		std::cout << "In xtyRegMsg::xtyRegMsg()" << std::endl;
#endif
	}
	
	virtual ~xtyRegMsg()
	{
#ifdef DEBUG
		std::cout << "In xtyRegMsg::~xtyRegMsg()" << std::endl;
#endif
	}

public:
	xtyRegMsg(const xtyRegMsg& msg)
	: xtyMsg(msg), m_ProcessName(msg.m_ProcessName), m_ProcessIP(msg.m_ProcessIP), m_ProcessPort(msg.m_ProcessPort)
	{
		// does nothing
	}
	
	xtyRegMsg& operator = (const xtyRegMsg& msg)
	{
		m_MsgType = msg.m_MsgType;
		m_MsgLen = msg.m_MsgLen;
		m_ProcessName = msg.m_ProcessName;
		m_ProcessIP = msg.m_ProcessIP;
		m_ProcessPort = msg.m_ProcessPort;
		
		return *this;
	}

public:	
	// 2015-11-07
	// dangerous(new-inside-delete-outside)
	// but I think it is also necessary especially
	// when temp object cannot satisfy the condition
	inline virtual xtyMsg* Clone()
	{
		xtyRegMsg* pCloneMsg = new xtyRegMsg;
		
		pCloneMsg->m_MsgType = m_MsgType;
		pCloneMsg->m_MsgLen = m_MsgLen;
		pCloneMsg->m_ProcessName = m_ProcessName;
		pCloneMsg->m_ProcessIP = m_ProcessIP;
		pCloneMsg->m_ProcessPort = m_ProcessPort;
		
		return pCloneMsg;
	}

	// 2016-01-06
	// calculate msg length before serialization
	inline virtual void CalculateSerializedMsgLen()
	{
		m_MsgLen = sizeof(m_MsgType) + sizeof(m_MsgLen) + sizeof(size_t) + m_ProcessName.size()
			+ sizeof(size_t) + m_ProcessIP.size() + sizeof(size_t) + m_ProcessPort.size();
	}
	
public:	
	// overload operator '<<' for message-struct's output
	inline friend std::ostream& operator << (std::ostream& out, const xtyRegMsg& msg)
	{
		out << "m_MsgType: " << msg.m_MsgType << std::endl;
		out << "m_MsgLen: " << msg.m_MsgLen << std::endl;
		out << "m_ProcessName: " << msg.m_ProcessName << std::endl;
		out << "m_ProcessIP: " << msg.m_ProcessIP << std::endl;
		out << "m_ProcessPort: " << msg.m_ProcessPort << std::endl;
		return out;
	}
	
public:
	// 2015-11-04
	// add virtual serial/deserial interfaces for each msg-struct
	virtual void Serialize(char*& pSerializeIterator, char* const& pSerializeBufEnd);
	virtual void DeSerialize(char*& pDeSerializeIterator, char* const& pDeSerializeBufEnd);

public:
	// 2015-10-15
	// set all the msg's fields to be 'public' for syntactic sugar
	std::string m_ProcessName;
	
	// 2016-01-11
	// add ip+port for application-layer reg-dereg
	std::string m_ProcessIP;
	std::string m_ProcessPort;
};

struct xtyDeRegMsg : public xtyMsg
{
public:
	xtyDeRegMsg() : xtyMsg(XTY_DEREG_MSG)
	{
#ifdef DEBUG
		std::cout << "In xtyDeRegMsg::xtyDeRegMsg()" << std::endl;
#endif
	}
	
	virtual ~xtyDeRegMsg()
	{
#ifdef DEBUG
		std::cout << "In xtyDeRegMsg::~xtyDeRegMsg()" << std::endl;
#endif
	}

public:
	xtyDeRegMsg(const xtyDeRegMsg& msg)
	: xtyMsg(msg), m_ProcessName(msg.m_ProcessName), m_ProcessIP(msg.m_ProcessIP), m_ProcessPort(msg.m_ProcessPort)
	{
		// does nothing
	}
	
	xtyDeRegMsg& operator = (const xtyDeRegMsg& msg)
	{
		m_MsgType = msg.m_MsgType;
		m_MsgLen = msg.m_MsgLen;
		m_ProcessName = msg.m_ProcessName;
		m_ProcessIP = msg.m_ProcessIP;
		m_ProcessPort = msg.m_ProcessPort;
		
		return *this;
	}

public:	
	// 2015-11-07
	// dangerous(new-inside-delete-outside)
	// but I think it is also necessary especially
	// when temp object cannot satisfy the condition
	inline virtual xtyMsg* Clone()
	{
		xtyDeRegMsg* pCloneMsg = new xtyDeRegMsg;
		
		pCloneMsg->m_MsgType = m_MsgType;
		pCloneMsg->m_MsgLen = m_MsgLen;
		pCloneMsg->m_ProcessName = m_ProcessName;
		pCloneMsg->m_ProcessIP = m_ProcessIP;
		pCloneMsg->m_ProcessPort = m_ProcessPort;
		
		return pCloneMsg;
	}
	
	// 2016-01-06
	// calculate msg length before serialization
	inline virtual void CalculateSerializedMsgLen()
	{
		m_MsgLen = sizeof(m_MsgType) + sizeof(m_MsgLen) + sizeof(size_t) + m_ProcessName.size()
			+ sizeof(size_t) + m_ProcessIP.size() + sizeof(size_t) + m_ProcessPort.size();
	}

public:	
	// overload operator '<<' for message-struct's output
	inline friend std::ostream& operator << (std::ostream& out, const xtyDeRegMsg& msg)
	{
		out << "m_MsgType: " << msg.m_MsgType << std::endl;
		out << "m_MsgLen: " << msg.m_MsgLen << std::endl;
		out << "m_ProcessName: " << msg.m_ProcessName << std::endl;
		out << "m_ProcessIP: " << msg.m_ProcessIP << std::endl;
		out << "m_ProcessPort: " << msg.m_ProcessPort << std::endl;
		return out;
	}

public:
	// 2015-11-04
	// add virtual serial/deserial interfaces for each msg-struct
	virtual void Serialize(char*& pSerializeIterator, char* const& pSerializeBufEnd);
	virtual void DeSerialize(char*& pDeSerializeIterator, char* const& pDeSerializeBufEnd);

public:
	// 2015-10-15
	// set all the msg's fields to be 'public' for syntactic sugar
	std::string m_ProcessName;
	
	// 2016-01-11
	// add ip+port for application-layer reg-dereg
	std::string m_ProcessIP;
	std::string m_ProcessPort;
};

struct xtyDataMsg : public xtyMsg
{
public:
	xtyDataMsg() : xtyMsg(XTY_DATA_MSG)
	{
#ifdef DEBUG
		std::cout << "In xtyDataMsg::xtyDataMsg()" << std::endl;
#endif
	}
	
	virtual ~xtyDataMsg()
	{
#ifdef DEBUG
		std::cout << "In xtyDataMsg::~xtyDataMsg()" << std::endl;
#endif
	}

public:
	xtyDataMsg(const xtyDataMsg& msg)
	: xtyMsg(msg), m_ToProcessIP(msg.m_ToProcessIP), m_ToProcessName(msg.m_ToProcessName), m_Content(msg.m_Content)
	{
		// does nothing
	}
	
	xtyDataMsg& operator = (const xtyDataMsg& msg)
	{
		m_MsgType = msg.m_MsgType;
		m_MsgLen = msg.m_MsgLen;
		m_ToProcessIP = msg.m_ToProcessIP;
		m_ToProcessName = msg.m_ToProcessName;
		m_Content = msg.m_Content;
		
		return *this;
	}

public:	
	// 2015-11-07
	// dangerous(new-inside-delete-outside)
	// but I think it is also necessary especially
	// when temp object cannot satisfy the condition
	inline virtual xtyMsg* Clone()
	{
		xtyDataMsg* pCloneMsg = new xtyDataMsg;
		
		pCloneMsg->m_MsgType = m_MsgType;
		pCloneMsg->m_MsgLen = m_MsgLen;
		pCloneMsg->m_ToProcessIP = m_ToProcessIP;
		pCloneMsg->m_ToProcessName = m_ToProcessName;
		pCloneMsg->m_Content = m_Content;
		
		return pCloneMsg;
	}

	// 2016-01-06
	// calculate msg length before serialization
	inline virtual void CalculateSerializedMsgLen()
	{
		m_MsgLen = sizeof(m_MsgType) + sizeof(m_MsgLen) + sizeof(size_t) + m_ToProcessIP.size()
			+ sizeof(size_t) + m_ToProcessName.size() + sizeof(size_t) + m_Content.size();
	}
	
public:	
	// overload operator '<<' for message-struct's output
	inline friend std::ostream& operator << (std::ostream& out, const xtyDataMsg& msg)
	{
		out << "m_MsgType: " << msg.m_MsgType << std::endl;
		out << "m_MsgLen: " << msg.m_MsgLen << std::endl;
		out << "m_ToProcessIP: " << msg.m_ToProcessIP << std::endl;
		out << "m_ToProcessName: " << msg.m_ToProcessName << std::endl;
		out << "m_Content: " << msg.m_Content << std::endl;
		return out;
	}

public:
	// 2015-11-04
	// add virtual serial/deserial interfaces for each msg-struct
	virtual void Serialize(char*& pSerializeIterator, char* const& pSerializeBufEnd);
	virtual void DeSerialize(char*& pDeSerializeIterator, char* const& pDeSerializeBufEnd);

public:
	// 2015-10-15
	// set all the msg's fields to be 'public' for syntactic sugar
	std::string m_ToProcessIP;
	std::string m_ToProcessName;
	std::string m_Content;
};

#endif

