#ifndef _MD_EXCEPTION_H
#define _MD_EXCEPTION_H

#include <stdexcept>

namespace Opswat
{

/// @brief General exception class
class MDException : public std::runtime_error
{
public:

	MDException(const std::string& errorMessage) : std::runtime_error(errorMessage)
	{
	}	
};

/// @brief Exception class for parser errors 
class MDParsingException : public MDException
{
public:

	MDParsingException(const std::string& errorMessage, const std::string& json) : MDException(errorMessage), json_(json)
	{
	}	

	std::string getJson() const
	{
		return json_;
	}

private:

	std::string json_;
};

/// @brief Exception class for errors returned by the rest server
class MDErrorResponseException : public MDException
{
public:

	MDErrorResponseException(const std::string& errorMessage, int statusCode, const std::string& body) : MDException(errorMessage) , statusCode_(statusCode), body_(body)
	{
	}

	int getResponseStatusCode() const
	{
		return statusCode_;
	}	

	std::string getResponseBody() const
	{
		return body_;
	}

private:

	int statusCode_;
	std::string body_;
};

/// @brief Exception class for connection errors
class MDConnectionException : public MDException
{
public:

	MDConnectionException(const std::string& errorMessage) : MDException(errorMessage)
	{
	}
};

};

#endif