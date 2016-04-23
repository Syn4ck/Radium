#include <Core/CoreMacros.hpp>
#ifndef RADIUMENGINE_RA_CORE_HPP_
#define RADIUMENGINE_RA_CORE_HPP_

/// Defines the correct macro to export dll symbols.
#if defined   RA_CORE_EXPORTS    
    #define RA_CORE_API DLL_EXPORT 
#else
    #define RA_CORE_API DLL_IMPORT 
#endif

#endif //RADIUMENGINE_RA_CORE_HPP_
