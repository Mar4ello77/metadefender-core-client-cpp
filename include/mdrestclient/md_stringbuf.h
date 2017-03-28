#ifndef _MD_STRINGBUF_H
#define _MD_STRINGBUF_H

#include <sstream>

namespace Opswat
{

class StringBuf : public std::stringbuf
{
public:

	std::string& getBuffer()
	{
		return buffer_;
	}

	const std::string& getBuffer() const
	{
		return buffer_;
	}

protected:

	virtual int_type overflow(int_type c)
	{
		if(c != std::stringbuf::traits_type::eof())
		{
			try
			{
				buffer_.push_back(static_cast<char>(c));
			}
			catch(...)
			{
				return std::stringbuf::traits_type::eof();
			}
		}
		return c;
	}

	virtual std::streamsize xsputn(const char* ptr, std::streamsize num)
	{
		try
		{
			buffer_.append(ptr, static_cast<size_t>(num));
		}
		catch(...)
		{
			return 0;
		}
		return num;
	}

protected:

	std::string buffer_;
};

}

#endif