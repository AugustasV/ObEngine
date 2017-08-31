/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TGUI_CONFIG_HPP
#define TGUI_CONFIG_HPP

#include <SFML/Config.hpp>

// TGUI will link in the same way as SFML, unless TGUI_DYNAMIC or TGUI_STATIC is defined
#if !defined(TGUI_DYNAMIC) && !defined(TGUI_STATIC)
    #ifdef SFML_STATIC
        #define TGUI_STATIC
    #endif
#endif

#ifndef TGUI_STATIC

    #ifdef SFML_SYSTEM_WINDOWS

        // Windows compilers need specific (and different) keywords for export and import
        #ifdef tgui_EXPORTS
            #define TGUI_API
        #else
            #define TGUI_API
        #endif

    #else // Linux, FreeBSD, Mac OS X

        // GCC 4 has special keywords for showing/hidding symbols
        #if __GNUC__ >= 4
            #define TGUI_API __attribute__ ((__visibility__ ("default")))
        #else
            #define TGUI_API
        #endif

    #endif

#else

    // Static build doesn't need import/export macros
    #define TGUI_API
    #define TGUI_API

#endif


// Version of the library
#define TGUI_VERSION_MAJOR 0
#define TGUI_VERSION_MINOR 8
#define TGUI_VERSION_PATCH 0

// All supported compilers except GCC 4.9 support constexpr
#if __GNUC__
    #if __cpp_constexpr >= 201304
        #define TGUI_CONSTEXPR constexpr
    #else
        #define TGUI_CONSTEXPR
    #endif
#else
    #define TGUI_CONSTEXPR constexpr
#endif

#ifndef TGUI_NO_DEPRECATED_WARNINGS
    #define TGUI_DEPRECATED(msg) [[deprecated(msg)]]
#endif

#endif // TGUI_CONFIG_HPP
