#pragma once

#include <cassert>

namespace ddahlkvist
{
#define MSVC_COMPILER 1

#if defined(MSVC_COMPILER)
#define IMPORT_DLL __declspec(dllimport)
#define EXPORT_DLL __declspec(dllexport)
#else
#define IMPORT_DLL __attribute__ ((visibility("default")))
#define EXPORT_DLL __attribute__ ((visibility("default")))
#endif

#if defined(DD_DEBUG) || defined(DD_RELEASE)
#define DD_ASSERT(x) assert(x);
#elif defined (DD_FINAL)
#define DD_ASSERT(x) do {} while (false);
#endif

}