#ifndef _MD_UTILS_H
#define _MD_UTILS_H

#include <memory>

namespace Opswat
{
namespace Utils
{

/// @brief Basic make_unique implementation. It does not support arrays or custom deleters.
/// Source: Effective Modern C++ [Scott Meyers]
template<typename T, typename... Ts>
std::unique_ptr<T> make_unique(Ts&&... params)
{
	return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}
	
}
}

#endif
