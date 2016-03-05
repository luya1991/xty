
#include "xty.h"

template<typename T>
inline void __Serialize(char*& pSerializeIterator, char* const& pSerializeBufEnd, T& Member)
{
	if (pSerializeIterator + sizeof(T) > pSerializeBufEnd)
		throw xtyException(XTY_OUT_OF_BUFFER_RANGE);
	
	memcpy(pSerializeIterator, &Member, sizeof(T));
	pSerializeIterator += sizeof(T);
}

template<typename T>
inline void __DeSerialize(char*& pDeSerializeIterator, char* const& pDeSerializeBufEnd, T& Member)
{
	if (pDeSerializeIterator + sizeof(T) > pDeSerializeBufEnd)
		throw xtyException(XTY_OUT_OF_BUFFER_RANGE);
	
	memcpy(&Member, pDeSerializeIterator, sizeof(T));
	pDeSerializeIterator += sizeof(T);
}

template<>
inline void __Serialize(char*& pSerializeIterator, char* const& pSerializeBufEnd, std::string& StringMember)
{
	if (pSerializeIterator + sizeof(size_t) + StringMember.size() > pSerializeBufEnd)
		throw xtyException(XTY_OUT_OF_BUFFER_RANGE);

	// need to insert 'length' data for 'std::string' serialize
	size_t length = StringMember.size();
	*((size_t*)pSerializeIterator) = length;
	pSerializeIterator += sizeof(size_t);
	
	memcpy(pSerializeIterator, StringMember.data(), StringMember.size());
	pSerializeIterator += StringMember.size();
}

template<>
inline void __DeSerialize(char*& pDeSerializeIterator, char* const& pDeSerializeBufEnd, std::string& StringMember)
{
	if (pDeSerializeIterator + sizeof(size_t) > pDeSerializeBufEnd)
		throw xtyException(XTY_OUT_OF_BUFFER_RANGE);
	
	// need to extract 'length' data for 'std::string' deserialize
	size_t length = *((size_t*)pDeSerializeIterator);
	pDeSerializeIterator += sizeof(size_t);
	
	if (pDeSerializeIterator + length > pDeSerializeBufEnd)
		throw xtyException(XTY_OUT_OF_BUFFER_RANGE);
	
	StringMember.assign(pDeSerializeIterator, length);
	pDeSerializeIterator += length;
}

void xtyDullMsg::Serialize(char*& pSerializeIterator, char* const& pSerializeBufEnd)
{
	CalculateSerializedMsgLen();
	__Serialize(pSerializeIterator, pSerializeBufEnd, m_MsgType);
	__Serialize(pSerializeIterator, pSerializeBufEnd, m_MsgLen);
}

void xtyDullMsg::DeSerialize(char*& pDeSerializeIterator, char* const& pDeSerializeBufEnd)
{
	__DeSerialize(pDeSerializeIterator, pDeSerializeBufEnd, m_MsgType);
	__DeSerialize(pDeSerializeIterator, pDeSerializeBufEnd, m_MsgLen);
}

void xtyQuitMsg::Serialize(char*& pSerializeIterator, char* const& pSerializeBufEnd)
{
	CalculateSerializedMsgLen();
	__Serialize(pSerializeIterator, pSerializeBufEnd, m_MsgType);
	__Serialize(pSerializeIterator, pSerializeBufEnd, m_MsgLen);
}

void xtyQuitMsg::DeSerialize(char*& pDeSerializeIterator, char* const& pDeSerializeBufEnd)
{
	__DeSerialize(pDeSerializeIterator, pDeSerializeBufEnd, m_MsgType);
	__DeSerialize(pDeSerializeIterator, pDeSerializeBufEnd, m_MsgLen);
}

void xtyRegMsg::Serialize(char*& pSerializeIterator, char* const& pSerializeBufEnd)
{
	CalculateSerializedMsgLen();
	__Serialize(pSerializeIterator, pSerializeBufEnd, m_MsgType);
	__Serialize(pSerializeIterator, pSerializeBufEnd, m_MsgLen);
	__Serialize(pSerializeIterator, pSerializeBufEnd, m_ProcessName);
	__Serialize(pSerializeIterator, pSerializeBufEnd, m_ProcessIP);
	__Serialize(pSerializeIterator, pSerializeBufEnd, m_ProcessPort);
}

void xtyRegMsg::DeSerialize(char*& pDeSerializeIterator, char* const& pDeSerializeBufEnd)
{
	__DeSerialize(pDeSerializeIterator, pDeSerializeBufEnd, m_MsgType);
	__DeSerialize(pDeSerializeIterator, pDeSerializeBufEnd, m_MsgLen);
	__DeSerialize(pDeSerializeIterator, pDeSerializeBufEnd, m_ProcessName);
	__DeSerialize(pDeSerializeIterator, pDeSerializeBufEnd, m_ProcessIP);
	__DeSerialize(pDeSerializeIterator, pDeSerializeBufEnd, m_ProcessPort);
}

void xtyDeRegMsg::Serialize(char*& pSerializeIterator, char* const& pSerializeBufEnd)
{
	CalculateSerializedMsgLen();
	__Serialize(pSerializeIterator, pSerializeBufEnd, m_MsgType);
	__Serialize(pSerializeIterator, pSerializeBufEnd, m_MsgLen);
	__Serialize(pSerializeIterator, pSerializeBufEnd, m_ProcessName);
	__Serialize(pSerializeIterator, pSerializeBufEnd, m_ProcessIP);
	__Serialize(pSerializeIterator, pSerializeBufEnd, m_ProcessPort);
}

void xtyDeRegMsg::DeSerialize(char*& pDeSerializeIterator, char* const& pDeSerializeBufEnd)
{
	__DeSerialize(pDeSerializeIterator, pDeSerializeBufEnd, m_MsgType);
	__DeSerialize(pDeSerializeIterator, pDeSerializeBufEnd, m_MsgLen);
	__DeSerialize(pDeSerializeIterator, pDeSerializeBufEnd, m_ProcessName);
	__DeSerialize(pDeSerializeIterator, pDeSerializeBufEnd, m_ProcessIP);
	__DeSerialize(pDeSerializeIterator, pDeSerializeBufEnd, m_ProcessPort);
}

void xtyDataMsg::Serialize(char*& pSerializeIterator, char* const& pSerializeBufEnd)
{
	CalculateSerializedMsgLen();
	__Serialize(pSerializeIterator, pSerializeBufEnd, m_MsgType);
	__Serialize(pSerializeIterator, pSerializeBufEnd, m_MsgLen);
	__Serialize(pSerializeIterator, pSerializeBufEnd, m_ToProcessIP);
	__Serialize(pSerializeIterator, pSerializeBufEnd, m_ToProcessName);
	__Serialize(pSerializeIterator, pSerializeBufEnd, m_Content);
}

void xtyDataMsg::DeSerialize(char*& pDeSerializeIterator, char* const& pDeSerializeBufEnd)
{
	__DeSerialize(pDeSerializeIterator, pDeSerializeBufEnd, m_MsgType);
	__DeSerialize(pDeSerializeIterator, pDeSerializeBufEnd, m_MsgLen);
	__DeSerialize(pDeSerializeIterator, pDeSerializeBufEnd, m_ToProcessIP);
	__DeSerialize(pDeSerializeIterator, pDeSerializeBufEnd, m_ToProcessName);
	__DeSerialize(pDeSerializeIterator, pDeSerializeBufEnd, m_Content);
}

