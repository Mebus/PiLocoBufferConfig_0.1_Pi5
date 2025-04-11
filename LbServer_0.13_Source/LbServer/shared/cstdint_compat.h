/**
 * @file    cstdint_compat.h
 * @version $Id: cstdint_compat.h 996 2017-12-09 18:18:22Z pischky $
 * @author  Martin Pischky <martin@pischky.de>
 *
 * Compatibility header file for compiler not supporting "#include <cstdint>".
 * See http://stackoverflow.com/a/14615587/1282742
 * and https://msinttypes.googlecode.com/svn/trunk/stdint.h
 * More information: https://code.google.com/archive/p/msinttypes/
 */
 
#ifndef CSTDINT_COMPAT_H_INCLUDED
#define CSTDINT_COMPAT_H_INCLUDED

#ifdef _MSC_VER
    #if _MSC_VER >= 1600        // Visual Studio 2010 or later
        #include <cstdint>
    #elif _MSC_VER >= 1300      // Visual Studio.NET (2002) or later
        typedef signed __int8       int8_t;
        typedef signed __int16      int16_t;
        typedef signed __int32      int32_t;
        typedef unsigned __int8     uint8_t;
        typedef unsigned __int16    uint16_t;
        typedef unsigned __int32    uint32_t;
        typedef signed __int64      int64_t;
        typedef unsigned __int64    uint64_t;
    #else                       // Visual C++ 6.0 or earlier
        typedef signed char         int8_t;
        typedef signed short        int16_t;
        typedef signed int          int32_t;
        typedef unsigned char       uint8_t;
        typedef unsigned short      uint16_t;
        typedef unsigned int        uint32_t;
        typedef signed __int64      int64_t;
        typedef unsigned __int64    uint64_t;
    #endif
#elif __GNUC__ >= 3
    #include <cstdint>
#else
    #error unknown compiler (you should consider updateing cstdint_compat.h)
#endif

//TODO: intmax_t, uintmax_t, int_least8_t, ..., INTMAX_MIN, ..., INT8_MIN, ...

#endif //CSTDINT_COMPAT_H_INCLUDED