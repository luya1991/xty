
#ifndef XTY_MSG_SERIALIZATION_H
#define XTY_MSG_SERIALIZATION_H

#define XTY_SERIALIZE_BUF_LEN 			4096

#define	XTY_SESSION_SERIALIZE_ENTITY_NUM 	1
#define XTY_SESSION_DESERIALIZE_ENTITY_NUM	1

#define XTY_SERVER_SERIALIZE_ENTITY_NUM		1
#define XTY_SERVER_DESERIALIZE_ENTITY_NUM 	1024

inline static xtyMsg* CreateDullMsg()
{
	xtyMsg* pMsg = new xtyDullMsg;
	return pMsg;
}

inline static xtyMsg* CreateQuitMsg()
{
	xtyMsg* pMsg = new xtyQuitMsg;
	return pMsg;
}

inline static xtyMsg* CreateRegMsg()
{
	xtyMsg* pMsg = new xtyRegMsg;
	return pMsg;
}

inline static xtyMsg* CreateDeRegMsg()
{
	xtyMsg* pMsg = new xtyDeRegMsg;
	return pMsg;
}

inline static xtyMsg* CreateDataMsg()
{
	xtyMsg* pMsg = new xtyDataMsg;
	return pMsg;
}

// 2016-02-23, create msg fun type define
typedef xtyMsg* (*xtyCreateMsgFun)(void);

// 2016-02-23, global array to create different type of msg
static const xtyCreateMsgFun xtyCreateMsgFunArray[] = 
{
	CreateDullMsg,
	CreateQuitMsg,
	CreateRegMsg,
	CreateDeRegMsg,
	CreateDataMsg,
	NULL,
};

class xtySerializeEntity
{
private:
	xtySerializeEntity();
	~xtySerializeEntity();

private:
	// added on 2016-02-24
	inline void Prepare()
	{
		// clean up old data
		memset(m_pSerializeBuf, 0, XTY_SERIALIZE_BUF_LEN);

		// set up serialize pointers
		m_pSerializeIterator = m_pSerializeBuf;
		m_pSerializeBufEnd = m_pSerializeBuf + XTY_SERIALIZE_BUF_LEN;
	}
	
private:
	char* Serialize(xtyMsg* pSerializeMsg, size_t* pSerializeDataLen);

private:
	friend class xtyMsgSerializer;
	
private:
	// product of serialization
	char* m_pSerializeBuf;
	
	// serialization aux pointers
	char* m_pSerializeIterator;
	char* m_pSerializeBufEnd;
	
	// added on 2016-02-24
	bool m_IsOccupied;
};

class xtyDeSerializeEntity
{
private:
	xtyDeSerializeEntity();
	~xtyDeSerializeEntity();

private:
	// added on 2016-02-24
	inline void Prepare(char* pDeSerializeBuf, size_t DeSerializeBufLen)
	{
		// clean up old data
		if (m_pDeSerializedMsg)
		{
			delete m_pDeSerializedMsg;
			m_pDeSerializedMsg = NULL;
		}
		
		// set up deserialize pointers
		m_pDeSerializeIterator = pDeSerializeBuf;
		m_pDeSerializeBufEnd = pDeSerializeBuf + DeSerializeBufLen;
		
		// get msg type from the head of deserialize buffer
		xtyMsgType Type = *((xtyMsgType*) pDeSerializeBuf);
		
		/* 
		 * 2016-02-23
		 * 'switch-case' here is not OOP, but to impl real OOP
		 * I need to write a lot of trivial classes. So, I made
		 * a trade-off to write an array of msg creation funs,
		 * and then use 'Type' as index to access certain slots.
		 *
		 * switch (Type)
		 * {
		 *	case XTY_DULL_MSG:
		 *		m_pDeSerializedMsg = new xtyDullMsg;
		 *		break;
		 *
		 *	case XTY_QUIT_MSG:
		 *		m_pDeSerializedMsg = new xtyQuitMsg;
		 *		break;
		 *
		 *	case XTY_REG_MSG:
		 *		m_pDeSerializedMsg = new xtyRegMsg;
		 *		break;
		 *
		 *	case XTY_DEREG_MSG:
		 *		m_pDeSerializedMsg = new xtyDeRegMsg;
		 *		break;
		 *		
            	 *	case XTY_DATA_MSG:
                 *		m_pDeSerializedMsg = new xtyDataMsg;
		 *		break;
		 * }
		 */
		
		// call msg creation fun on slot 'Type' of the fun array
		m_pDeSerializedMsg = xtyCreateMsgFunArray[Type]();
	}

private:
	xtyMsg* DeSerialize(char* pDeSerializeBuf, size_t DeSerializeBufLen);
	
private:
	friend class xtyMsgDeSerializer;
	
private:
	// product of deserialization
	xtyMsg* m_pDeSerializedMsg;
	
	// deserialization aux pointers
	char* m_pDeSerializeIterator;
	char* m_pDeSerializeBufEnd;
	
	// added on 2016-02-24
	bool m_IsOccupied;
};

class xtyMsgSerializer
{
public:
	explicit xtyMsgSerializer(const size_t TotalEntity);
	~xtyMsgSerializer();

private:
	xtyMsgSerializer(const xtyMsgSerializer&);
	xtyMsgSerializer& operator = (const xtyMsgSerializer&);

private:	
	// 2016-02-26 newly added
	inline xtySerializeEntity* SearchAvailableEntity()
	{
		for (size_t i = 0; i < m_TotalEntityCount; i++)
		{
			if (m_pEntityArray[i]->m_IsOccupied == 0)
				return m_pEntityArray[i];
		}
		
		return NULL;
	}
	
public:
	// serialize core method
	char* Serialize(xtyMsg* pSerializeMsg, size_t* pSerializeDataLen);
	
	// 2016-02-24 newly added, acquire an entity and set state to occupied
	xtySerializeEntity* AcquireEntity();
	
	// 2016-02-27 newly added, release an entity given buf's pointer
	void ReleaseEntity(const char* const pSerializeData);
	
public:
#ifdef DEBUG
	friend class xtyMsgSerializationTest;
#endif
	
private:
	// array of serialize entities
	xtySerializeEntity** m_pEntityArray;
	
	// total number of entities
	const size_t m_TotalEntityCount;
	
	// added on 2016-02-23
	xtyMutex m_EntityArrayLock;
	
	// added on 2016-02-24
	xtyCond m_EntityArrayCond;
};

class xtyMsgDeSerializer
{
public:
	explicit xtyMsgDeSerializer(const size_t TotalEntity);
	~xtyMsgDeSerializer();

private:
	xtyMsgDeSerializer(const xtyMsgDeSerializer&);
	xtyMsgDeSerializer& operator = (const xtyMsgDeSerializer&);

private:	
	// 2016-02-26 newly added
	inline xtyDeSerializeEntity* SearchAvailableEntity()
	{
		for (size_t i = 0; i < m_TotalEntityCount; i++)
		{
			if (m_pEntityArray[i]->m_IsOccupied == 0)
				return m_pEntityArray[i];
		}
		
		return NULL;
	}
	
public:
	// deserialize core method
	xtyMsg* DeSerialize(char* pDeSerializeBuf, size_t DeSerializeBufLen);
	
	// 2016-02-24 newly added, acquire an entity and set state to occupied
	xtyDeSerializeEntity* AcquireEntity();

	// 2016-02-27 newly added, release an entity given msg's pointer
	void ReleaseEntity(const xtyMsg* const pMsg);

public:
#ifdef DEBUG
	friend class xtyMsgSerializationTest;
#endif
	
private:
	// array of deserialize entities
	xtyDeSerializeEntity** m_pEntityArray;
	
	// total number of entities
	const size_t m_TotalEntityCount;
	
	// added on 2016-02-23
	xtyMutex m_EntityArrayLock;
	
	// added on 2016-02-24
	xtyCond m_EntityArrayCond;
};

#endif

