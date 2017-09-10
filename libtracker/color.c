/* color.c 
	vi:ts=3 sw=3:
*/

/* $Id: color.c,v 1.1.1.1 2002/01/18 16:34:20 lindner Exp $
 * $Log: color.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:20  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:37  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 1.3  1995/03/08  13:48:44  espie
 * *** empty log message ***
 *
 * Revision 1.2  1995/03/06  23:36:05  espie
 * Proper color patch.
 *
 * Revision 1.1  1995/03/02  13:52:25  espie
 * Initial revision
 *
 */

char *write_color(base, color)
char *base;
int color;
	{
	*base++ = '\033';
	*base++ = '[';
#ifdef SCO_ANSI_COLOR
	if (color == 0)
		{
		*base++ = 'x';
		return base;
		}
#endif
	*base++ = '0' + color / 8;
	*base++=';';
	*base++='3';
	if (color == 0)		/* color == 0 means reset */
		*base++ = '9';
	else
		*base++ = '0' + color % 8;
	*base++ = 'm';
	return base;
	}
