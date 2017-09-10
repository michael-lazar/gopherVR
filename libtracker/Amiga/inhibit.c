/* amiga/inhibit.c
	vi:ts=3 sw=3:
 */

/* $Id: inhibit.c,v 1.1.1.1 2002/01/18 16:34:23 lindner Exp $
 * $Log: inhibit.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:23  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:39  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 1.1  1995/01/13  13:31:35  espie
 * Initial revision
 *
 */


/* look at client.c/audio.c for the use of inhibit_output with respect
 * to discard_buffer.
 * This variable is used at multiple places, I didn't know where to put it,
 * since it doesn't belong to any package...
 */
unsigned int inhibit_output = 0;

