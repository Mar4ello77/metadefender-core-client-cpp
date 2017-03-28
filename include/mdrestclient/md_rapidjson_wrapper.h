#ifndef _RAPIDJSON_WRAPPER_H
#define _RAPIDJSON_WRAPPER_H

#include "md_exception.h"
#include "md_error_messages.h"

#ifndef RAPIDJSON_ASSERT
#define RAPIDJSON_ASSERT(x) do{if(!(x)) throw Opswat::MDException(Opswat::MD_JSON_PARSING_ERROR);}while(0)
#endif

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#endif
