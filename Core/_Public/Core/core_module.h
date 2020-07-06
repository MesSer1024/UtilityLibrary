#pragma once

#include <Core/Platform.h>

namespace ddahlkvist
{

#if defined(BUILD_COMPILE_DLL)
#if defined(BUILD_EXPORT_CORE_MODULE)
#define CORE_PUBLIC EXPORT_DLL
#else
#define CORE_PUBLIC IMPORT_DLL
#endif
#else
#define CORE_PUBLIC
#endif

#if defined(BUILD_INTERNAL_ACCESS_CORE_MODULE) || defined(BUILD_EXPORT_CORE_MODULE)
#define CORE_INTERNAL CORE_PUBLIC
#else
#define CORE_INTERNAL
#endif

CORE_PUBLIC int dummy_bind(); // dummy to get something to compile

}