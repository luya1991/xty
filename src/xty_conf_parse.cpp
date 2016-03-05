
#include "xty.h"

xtySpinLock xtyConfParse::m_Lock;
xtyConfParse* xtyConfParse::m_pConfParse = NULL;

xtyConfParse::xtyConfParse()
{
	m_FileDescriptor = open(XTY_CONF_FILE_PATH, O_RDONLY);
	
	if (m_FileDescriptor == -1)
		xtyError::ErrorExit(errno);
	
	// init <para-name, para-type> mapping table
	InitConfParaTypeTable();
	
	// start to parse configuration file
	ParseConfFile();
}

xtyConfParse::~xtyConfParse()
{
	(void) close(m_FileDescriptor);
}

void xtyConfParse::Create()
{	
	if (m_pConfParse == NULL)
	{
		m_Lock.Lock();
		
		// double-check for high efficiency
		if (m_pConfParse == NULL)
			m_pConfParse = new xtyConfParse;
		
		m_Lock.UnLock();
	}
}

void xtyConfParse::Destroy()
{
	if (m_pConfParse != NULL)
	{
		m_Lock.Lock();
		
		// double-check for high efficiency
		if (m_pConfParse != NULL)
		{
			delete m_pConfParse;
			m_pConfParse = NULL;
		}
		
		m_Lock.UnLock();
	}
}

void xtyConfParse::InitConfParaTypeTable()
{
	// This function is called in the constructor,
	// since the constructor will only be called once,
	// there is no need to add a 'lock-unlock' area.
	
	m_ConfParaTypeTable["server_name"] = XTY_CONF_TYPE_STRING;
	m_ConfParaTypeTable["server_ip"] = XTY_CONF_TYPE_STRING;
	m_ConfParaTypeTable["server_port"] = XTY_CONF_TYPE_UINT16;
	m_ConfParaTypeTable["heart_interval"] = XTY_CONF_TYPE_UINT32;
}

bool xtyConfParse::ParseOneLine(const std::vector<std::string>& LineContent)
{
#ifdef DEBUG
	std::cout << "In xtyConfParse::ParseOneLine()" << std::endl;
#endif
	if (LineContent.size() != 2)
	{
		std::cout << "Warning: invalid cond para num in one line" << std::endl;
		return false;
	}
	
	std::map<std::string, xtyConfParaType>::const_iterator iter;
	
	if ((iter = m_ConfParaTypeTable.find(LineContent[0])) == m_ConfParaTypeTable.end())
	{
		std::cout << "Warning: unsupported conf para name" << std::endl;
		return false;
	}
	
	xtyConfPara temp_conf_para;
	
	temp_conf_para.m_ConfParaType = iter->second;
	temp_conf_para.m_ConfParaName = LineContent[0];
	temp_conf_para.m_ConfParaStrValue = LineContent[1];
	
	m_ConfParaList.push_back(temp_conf_para);

	return true;
}

bool xtyConfParse::ParseConfFile()
{
	// This function is called in the constructor,
	// since the constructor will only be called once,
	// there is no need to add a 'lock-unlock' area.
	
	char buf[XTY_CONF_PARSE_BUF_LEN] = {0};
	
	ssize_t readbytes = read(m_FileDescriptor, buf, XTY_CONF_PARSE_BUF_LEN);
	
	if (readbytes == -1)
		xtyError::ErrorExit(errno);

	bool b_annotation_area = 0;
	bool b_conf_para_area = 0;
	
	std::string temp_res;
	std::vector<std::string> line_temp_res;
	
	char* p;
	
	for (p = buf; *p; p++)
	{
		if (*p == '#')
		{
			b_annotation_area = 1;
			continue;
		}
		
		if (*p == '\n')
		{
			if (b_annotation_area)
				b_annotation_area = 0;
			
			if (b_conf_para_area)
			{
				b_conf_para_area = 0;
				
				if (!temp_res.empty())
				{
					line_temp_res.push_back(temp_res);
					temp_res.clear();
				}
			}
			
			if (!line_temp_res.empty())
			{
				if (!ParseOneLine(line_temp_res))
					return false;
				
				line_temp_res.clear();
			}
			
			continue;
		}
		
		if (b_annotation_area)
			continue;
		
		if (*p == ' ' || *p == '\t')
		{
			if (b_conf_para_area)
			{
				b_conf_para_area = 0;
				
				if (!temp_res.empty())
				{
					line_temp_res.push_back(temp_res);
					temp_res.clear();
				}
			}
			
			continue;
		}
		
		if (!((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z')
		|| (*p >= '0' && *p <= '9') || (*p == '_' || *p == '.')))
		{
			std::cout << "In text area, unexpected char: " << *p << std::endl;
			return false;
		}
		
		else
		{
			b_conf_para_area = 1;
			temp_res += *p;
			continue;
		}
	}
	
#ifdef DEBUG
	size_t j;
	for (j = 0; j < m_ConfParaList.size(); j++)
	{
		std::cout << "para name: " << m_ConfParaList[j].m_ConfParaName << std::endl;
		std::cout << "para str value: " << m_ConfParaList[j].m_ConfParaStrValue << std::endl;
	}
#endif

	return true;
}

