
#ifndef XTY_CONF_PARSE_H
#define XTY_CONF_PARSE_H

#define XTY_CONF_PARSE_BUF_LEN 4096

#define XTY_CONF_FILE_PATH \
"/home/genius/workspace/proj/xty/conf/xty.conf"

typedef enum
{
	XTY_CONF_TYPE_STRING = 0,
	XTY_CONF_TYPE_UINT16 = 1,
	XTY_CONF_TYPE_UINT32 = 2,
}xtyConfParaType;

typedef struct xtyConfPara_t
{
	xtyConfParaType m_ConfParaType;
	std::string m_ConfParaName;
	std::string m_ConfParaStrValue;
}xtyConfPara;
	
class xtyConfParse
{
private:
	xtyConfParse();
	~xtyConfParse();

private:
	xtyConfParse(const xtyConfParse&);
	xtyConfParse& operator = (const xtyConfParse&);

private:
	static void Create();
	static void Destroy();

private:
	void InitConfParaTypeTable();
	bool ParseConfFile();
	bool ParseOneLine(const std::vector<std::string>& LineContent);
	
public:	
	inline static xtyConfParse* GetInstance() { return m_pConfParse; }
	
	inline static xtyConfPara GetConfPara(const std::string& ConfParaName)
	{
		xtyConfParse* pConfParse = xtyConfParse::GetInstance();
		
		if(pConfParse == NULL)
		{
#ifdef DEBUG
			std::cout << "In xtyConfParse::GetConfPara(): ";
			std::cout << "Need to call xtyConfParse::Create() first" << std::endl;
#endif
			exit(XTY_ERROR);
		}
		
		size_t i;
		
		for (i = 0; i < pConfParse->m_ConfParaList.size(); i++)
		{
			if (pConfParse->m_ConfParaList[i].m_ConfParaName == ConfParaName)
				return pConfParse->m_ConfParaList[i];
		}
		
#ifdef DEBUG
		std::cout << "In xtyConfParse::GetConfPara(): ";
		std::cout << "Unknown configuration parameter name" << std::endl;
#endif
		exit(XTY_ERROR);
	}

public:
#ifdef DEBUG
	friend class xtyConfParseTest;
#endif

public:
	// added on 2015-11-11, for 'xtyFundamentalFacility'
	friend class xtyFundamentalFacility;

private:
	// conf file's fd
	int m_FileDescriptor;
	
	// store the result of conf file parsing
	// 2016-02-20, use std-component to avoid 'pAlloc'
	std::vector<xtyConfPara> m_ConfParaList;
	
	// <para-name, para-type> mapping table
	// 2016-02-20, use std-component to avoid 'pAlloc'
	std::map<std::string, xtyConfParaType> m_ConfParaTypeTable;
	
	static xtySpinLock m_Lock;
	static xtyConfParse* m_pConfParse;
};

#endif

