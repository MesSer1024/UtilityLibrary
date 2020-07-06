#pragma once

#include <Core/Platform.h>

namespace ddahlkvist
{

#if defined(BUILD_COMPILE_DLL)
#if defined(BUILD_EXPORT_LIBRARY_MODULE)
#define LIBRARY_PUBLIC EXPORT_DLL
#else
#define LIBRARY_PUBLIC IMPORT_DLL
#endif
#else
#define LIBRARY_PUBLIC
#endif

//#define LIBRARY_INTERNAL LIBRARY_PUBLIC
#if defined(BUILD_INTERNAL_ACCESS_LIBRARY_MODULE) || defined(BUILD_EXPORT_LIBRARY_MODULE)
#define LIBRARY_INTERNAL LIBRARY_PUBLIC
#else
#define LIBRARY_INTERNAL
#endif

LIBRARY_PUBLIC int dummy_bind(); // dummy to get something to compile

}