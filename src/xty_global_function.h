
#ifndef XTY_GLOBAL_FUNCTION_H
#define XTY_GLOBAL_FUNCTION_H

// max uint16: "65535" & '\0'
#define XTY_UINT16_STR_LEN 8

// max uint32: "4294967295" & '\0'
#define XTY_UINT32_STR_LEN 16

namespace xty
{
	inline bool NumericStringCheck(const std::string& s)
	{
		size_t i;

		for (i = 0; i < s.size(); i++)
		{
			if (s[i] < '0' || s[i] > '9')
				return false;
		}

		return true;
	}
	
	inline bool IPStringCheck(const std::string s)
	{
		size_t i;
		
		unsigned int temp = 0;
		unsigned int dot_count = 0;
		
		for (i = 0; i < s.size(); i++)
		{
			// valid ip string can only contain digits and dots
			if ((s[i] != '.') && ((s[i] < '0') || (s[i] > '9')))
				return false;
			
			if (s[i] == '.')
			{
				dot_count++;
				
				if (dot_count > 3)
					return false;

				// reset 'temp'
				temp = 0;
			}
			
			else
			{
				temp *= 10;
				temp += s[i] - '0';
			
				if (temp > 255)
					return false;
			}
		}
		
		if (dot_count != 3)
			return false;
		
		return true;
	}

	inline uint16_t StringToUint16(const std::string& s)
	{
		// warning: what if 's' makes 'res' overflow?!(e.g. 's' = '99999')
		
		uint16_t res = 0;

		size_t i;

		for (i = 0; i < s.size(); i++)
		{
			res *= 10;
			res += s[i] - '0';
		}

		return res;
	}
	
	inline uint32_t StringToUint32(const std::string& s)
	{
		// warning: what if 's' makes 'res' overflow?!(e.g. 's' = '9999999999')
		
		uint32_t res = 0;

		size_t i;

		for (i = 0; i < s.size(); i++)
		{
			res *= 10;
			res += s[i] - '0';
		}

		return res;
	}
	
	inline std::string Uint16ToString(const uint16_t n)
	{
		// warning: illegal input '-1' will become '65535' as 'uint16_t'
		
		uint16_t uint = n;	
		char uint_str[XTY_UINT16_STR_LEN] = {0};
		
		int i;
		size_t start_pos = 0;
		
		for (i = XTY_UINT16_STR_LEN - 2; i >= 0; i--)
		{
			uint_str[i] = (char)((uint % 10) + 48);
			uint = uint / 10;
			
			if (uint == 0)
			{
				start_pos = i;
				break;
			}
		}
		
		return std::string(&uint_str[start_pos]);
	}
	
	inline std::string Uint32ToString(const uint32_t n)
	{
		// warning: illegal input '-1' will become '4294967295' as 'uint32_t'
		
		uint32_t uint = n;	
		char uint_str[XTY_UINT32_STR_LEN] = {0};
		
		int i;
		size_t start_pos = 0;
		
		for (i = XTY_UINT32_STR_LEN - 2; i >= 0; i--)
		{
			uint_str[i] = (char)((uint % 10) + 48);
			uint = uint / 10;
			
			if (uint == 0)
			{
				start_pos = i;
				break;
			}
		}
		
		return std::string(&uint_str[start_pos]);
	}
	
	inline std::vector<std::string> SplitStringByChar(const std::string& s, const char c)
	{
		std::vector<std::string> res;
		
		size_t i;
		size_t pos1 = 0;
		size_t pos2 = 0;
		
		for (i = 0; i < s.size(); i++)
		{
			if (s[i] == c)
			{
				pos2 = i;
				
				// mark down current segment
				res.push_back(std::string(s, pos1, pos2 - pos1));
				
				pos2++;
				
				// update split position
				pos1 = pos2;
			}
		}
		
		return res;
	}
	
	inline void SetFdNonBlocking(const int fd)
	{
		int flags;

		if ((flags = fcntl(fd, F_GETFL, 0)) < 0)
		{
			std::cout << "fcntl(F_GETFL) error: ";
			std::cout << strerror(errno) << std::endl;
			exit(-1);
		}

		flags |= O_NONBLOCK;

		if (fcntl(fd, F_SETFL, flags) < 0)
		{
			std::cout << "fcntl(F_SETFL) error: ";
			std::cout << strerror(errno) << std::endl;
			exit(-1);
		}
	}

	inline void GetServerAddrConfig(std::string& ip, uint16_t& port)
	{
		xtyConfPara server_ip = xtyConfParse::GetConfPara("server_ip");
		xtyConfPara server_port = xtyConfParse::GetConfPara("server_port");
		
		// although 'inet_pton' will check ip string, I still impled my own version
		if (!xty::IPStringCheck(server_ip.m_ConfParaStrValue))
		{
			std::cout << "Invalid conf para 'server_ip': invalid ip string" << std::endl;
			exit(XTY_ERROR);
		}
		
		ip = server_ip.m_ConfParaStrValue;
		
		if (!xty::NumericStringCheck(server_port.m_ConfParaStrValue))
		{
			std::cout << "Invalid conf para 'server_port': invalid numeric string" << std::endl;
			exit(XTY_ERROR);
		}
		
		port = xty::StringToUint16(server_port.m_ConfParaStrValue);
	}
	
	inline void GetHeartIntervalConfig(uint32_t& interval)
	{
		xtyConfPara heart_interval = xtyConfParse::GetConfPara("heart_interval");
		
		if (!xty::NumericStringCheck(heart_interval.m_ConfParaStrValue))
		{
			std::cout << "Invalid conf para 'heart_interval': invalid numeric string" << std::endl;
			exit(XTY_ERROR);
		}
		
		interval = xty::StringToUint32(heart_interval.m_ConfParaStrValue);
	}
}
	
#endif

