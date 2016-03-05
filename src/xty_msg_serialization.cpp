
#include "xty.h"

xtySerializeEntity::xtySerializeEntity()
: m_pSerializeBuf(0), m_pSerializeIterator(0), m_pSerializeBufEnd(0), m_IsOccupied(0)
{
	m_pSerializeBuf = new char[XTY_SERIALIZE_BUF_LEN];
	memset(m_pSerializeBuf, 0, XTY_SERIALIZE_BUF_LEN);
}

xtySerializeEntity::~xtySerializeEntity()
{
	delete [] m_pSerializeBuf;
	m_pSerializeBuf = NULL;
}

char* xtySerializeEntity::Serialize(xtyMsg* pSerializeMsg, size_t* pSerializeDataLen)
{
	// prep work
	Prepare();
	
	// call msg class's virtual method for serialization
	pSerializeMsg->Serialize(m_pSerializeIterator, m_pSerializeBufEnd);
	
	// calculate the length of serialize data and write to the caller
	*pSerializeDataLen = (size_t) (m_pSerializeIterator - m_pSerializeBuf);
	
	return m_pSerializeBuf;
}

xtyDeSerializeEntity::xtyDeSerializeEntity()
: m_pDeSerializedMsg(0), m_pDeSerializeIterator(0), m_pDeSerializeBufEnd(0), m_IsOccupied(0)
{

}

xtyDeSerializeEntity::~xtyDeSerializeEntity()
{
	if (m_pDeSerializedMsg != NULL)
	{
		delete m_pDeSerializedMsg;
		m_pDeSerializedMsg = NULL;
	}
}

xtyMsg* xtyDeSerializeEntity::DeSerialize(char* pDeSerializeBuf, size_t DeSerializeBufLen)
{
	// prep work
	Prepare(pDeSerializeBuf, DeSerializeBufLen);
	
	// call msg class's virtual method for deserialization
	m_pDeSerializedMsg->DeSerialize(m_pDeSerializeIterator, m_pDeSerializeBufEnd);

	return m_pDeSerializedMsg;
}

xtyMsgSerializer::xtyMsgSerializer(const size_t TotalEntity) : m_TotalEntityCount(TotalEntity)
{
	m_pEntityArray = new xtySerializeEntity*[m_TotalEntityCount];
	memset(m_pEntityArray, 0, sizeof(xtySerializeEntity*) * m_TotalEntityCount);
	
	for (size_t i = 0; i < m_TotalEntityCount; i++)
		m_pEntityArray[i] = new xtySerializeEntity;
}

xtyMsgSerializer::~xtyMsgSerializer()
{
	for (size_t i = 0; i < m_TotalEntityCount; i++)
	{
		if (m_pEntityArray[i] != NULL)
		{
			delete m_pEntityArray[i];
			m_pEntityArray[i] = NULL;
		}
	}
	
	delete [] m_pEntityArray;
	m_pEntityArray = NULL;
}

xtySerializeEntity* xtyMsgSerializer::AcquireEntity()
{
	xtySerializeEntity* pAvailableEntity = NULL;
	
	m_EntityArrayLock.Lock();
	
	while ((pAvailableEntity = SearchAvailableEntity()) == NULL)
		m_EntityArrayCond.Wait(&m_EntityArrayLock);
	
	pAvailableEntity->m_IsOccupied = 1;
	
	m_EntityArrayLock.UnLock();
	
	return pAvailableEntity;
}

void xtyMsgSerializer::ReleaseEntity(const char* const pSerializeData)
{
	m_EntityArrayLock.Lock();
	
	for (size_t i = 0; i < m_TotalEntityCount; i++)
	{
		// find the entity that holds the given pointer of serialize buf
		if ((void*) pSerializeData == (void*) m_pEntityArray[i]->m_pSerializeBuf)
		{
			m_pEntityArray[i]->m_IsOccupied = 0;
			break;
		}
	}
	
	m_EntityArrayLock.UnLock();
	
	m_EntityArrayCond.WakeUp();
}

char* xtyMsgSerializer::Serialize(xtyMsg* pSerializeMsg, size_t* pSerializeDataLen)
{
	char* pSerializeData = NULL;
	
	xtySerializeEntity* pAvailableEntity = AcquireEntity();
	
	pSerializeData = pAvailableEntity->Serialize(pSerializeMsg, pSerializeDataLen);
	
	return pSerializeData;
}

xtyMsgDeSerializer::xtyMsgDeSerializer(const size_t TotalEntity) : m_TotalEntityCount(TotalEntity)
{
	m_pEntityArray = new xtyDeSerializeEntity*[m_TotalEntityCount];
	memset(m_pEntityArray, 0, sizeof(xtyDeSerializeEntity*) * m_TotalEntityCount);
	
	for (size_t i = 0; i < m_TotalEntityCount; i++)
		m_pEntityArray[i] = new xtyDeSerializeEntity;
}

xtyMsgDeSerializer::~xtyMsgDeSerializer()
{
	for (size_t i = 0; i < m_TotalEntityCount; i++)
	{
		if (m_pEntityArray[i] != NULL)
		{
			delete m_pEntityArray[i];
			m_pEntityArray[i] = NULL;
		}
	}
	
	delete [] m_pEntityArray;
	m_pEntityArray = NULL;
}

xtyDeSerializeEntity* xtyMsgDeSerializer::AcquireEntity()
{
	xtyDeSerializeEntity* pAvailableEntity = NULL;
	
	m_EntityArrayLock.Lock();
	
	while ((pAvailableEntity = SearchAvailableEntity()) == NULL)
		m_EntityArrayCond.Wait(&m_EntityArrayLock);
	
	pAvailableEntity->m_IsOccupied = 1;
	
	m_EntityArrayLock.UnLock();
	
	return pAvailableEntity;
}

void xtyMsgDeSerializer::ReleaseEntity(const xtyMsg * const pMsg)
{
	m_EntityArrayLock.Lock();

	for (size_t i = 0; i < m_TotalEntityCount; i++)
	{
		// find the entity that holds the given pointer of msg
		if ((void*) pMsg == (void*) m_pEntityArray[i]->m_pDeSerializedMsg)
		{
			m_pEntityArray[i]->m_IsOccupied = 0;
			break;
		}
	}

	m_EntityArrayLock.UnLock();

	m_EntityArrayCond.WakeUp();
}

xtyMsg* xtyMsgDeSerializer::DeSerialize(char* pDeSerializeBuf, size_t DeSerializeBufLen)
{
	xtyMsg* pDeSerializedMsg = NULL;
	
	xtyDeSerializeEntity* pAvailableEntity = AcquireEntity();
	
	pDeSerializedMsg = pAvailableEntity->DeSerialize(pDeSerializeBuf, DeSerializeBufLen);
	
	return pDeSerializedMsg;
}

