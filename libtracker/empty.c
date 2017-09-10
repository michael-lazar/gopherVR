/* empty.c 
	vi:ts=3 sw=3:
 */

/* $Id: empty.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $
 * $Log: empty.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:21  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:37  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 1.8  1995/02/25  15:43:11  espie
 * Added color.
 *
 * Revision 1.7  1995/02/21  21:13:16  espie
 * Cleaned up source. Moved minor pieces of code around.
 *
 * Revision 1.6  1995/02/21  17:54:32  espie
 * Internal problem: buggy RCS. Fixed logs.
 *
 *
 */

#include "defs.h"
#include "extern.h"

#include "song.h"
     
ID("$Id: empty.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $")

LOCAL void init_empty P((void));

LOCAL void (*INIT)P((void)) = init_empty;

LOCAL struct sample_info dummy;

LOCAL void init_empty()
	{
	int i;

	dummy.name = NULL;
	dummy.length = dummy.rp_offset = dummy.rp_length = 0;
	dummy.fix_length = dummy.fix_rp_length = 0;
	dummy.volume = dummy.color = dummy.finetune = 0;
	dummy.start = dummy.rp_start = NULL;
	dummy.color = 1;
	for (i = 0; i <= MAX_VOLUME; i++)
		dummy.volume_lookup[i] = 0;
	}

struct sample_info *empty_sample()
	{
	INIT_ONCE;

	return &dummy;
	}

