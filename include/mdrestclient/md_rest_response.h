#ifndef _MD_REST_RESPONSE_H
#define _MD_REST_RESPONSE_H

#include "md_utils.h"
#include <string>
#include <memory>

namespace Opswat
{

/// @brief Rest response
///
/// This class represents the response sent by the rest server.
template<typename T>
class MDResponse
{
public:

	/// @brief Constructor
	///
	/// @param responseJSON Json sent by the rest server
	MDResponse(const std::string& responseJSON) : json_(responseJSON) {}

public:

	/// @brief Get the response Json
	///
	/// By calling this function, the response Json can be acquired without any parsing overhead
	///
	/// @return response Json
	std::string getJSON() const
	{
		return json_;
	}

	/// @brief Parse the response Json and return the generated structure
	///
	/// This function populates and returns the response structure.
	///
	/// @return Response structure
	std::unique_ptr<T> parse() const
	{
		return Utils::make_unique<T>(json_);
	}

private:

	std::string json_;
};

};

#endif
