#include <stdio.h>

#include "sd_stdio.h"
#include "apricot-hd.h"

#ifdef APRIDEBUG

void debug_print_status(WDSTAT reg)
{
	printf("Status Register:\n");
	printf("| Busy | Ready | Write Fail | Seek Comp | Data Req | Reserved | CIP | Error |\n");
	printf("  %3d    %3d        %3d         %3d        %3d         %3d     %3d     %3d\n",
	       reg.status.bsy,
	       reg.status.rdy,
	       reg.status.wf,
	       reg.status.sc,
	       reg.status.drq,
	       reg.status.res,
	       reg.status.cip,
	       reg.status.err);
}

void debug_print_sdh(WDREG reg)
{
}

void debug_print_error(WDREG reg)
{
}

#endif
