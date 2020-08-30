#pragma once
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <memory>
#include <vector>

//#define FORCE_LOGS 1

#if defined( DEBUG ) || defined( _DEBUG ) || defined( FORCE_LOGS )
#define DEBUG_INFO 1
#endif

#if defined( _MSC_VER ) // MSVC does not like sprintf
#define custom_print( x, ... ) sprintf_s( x, __VA_ARGS__ )
#else
#define custom_print( x, ... ) sprintf( x, __VA_ARGS__ )
#endif

#if defined( DEBUG_INFO )

#include <iostream>

#define LOG( ... )                                                                            \
    {                                                                                         \
        char str[200];                                                                        \
        custom_print( str, __VA_ARGS__ );                                                     \
        std::cout << "[" << __FILE__ << "][" << __FUNCTION__ << "][Line " << __LINE__ << "] " \
                  << str << std::endl;                                                        \
    }
#else
#define LOG( ... )
#endif

template <typename T>
bool inRange( std::vector<T> const& range, T* elemPtr )
{
    return elemPtr >= &range.front() && elemPtr <= &range.back();
}

inline auto booleanStr( bool val )
{
    static const char* trueVal = "true";
    static const char* falseVal = "false";
    return val ? trueVal : falseVal;
}
