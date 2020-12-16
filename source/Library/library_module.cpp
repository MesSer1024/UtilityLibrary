// copyright Daniel Dahlkvist (c) 2020 [github.com/messer1024]
#include <Library/library_module.h>
#include <Core/core_module.h>

namespace ddahlkvist
{

int library_dummy()
{
	int fromCore = core_dummy();
	return fromCore;
}

}