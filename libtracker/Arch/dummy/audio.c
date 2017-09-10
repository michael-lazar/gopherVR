/*

Dummy for unsupported systems, such as Mac OS X, etc.
Eventually I'll write an SDL version.
Cameron Kaiser

 */

#include "defs.h"
#include "extern.h"
/* #include "Arch/common.c" */

int open_audio(int f, int s)
{
	return 0;
}

void set_synchro(int s)
{
	;
}

int update_frequency()
{
	return 0;
}

void output_samples(int left, int right)
{
	;
}

void flush_buffer()
{
	;
}

void discard_buffer()
{
	;
}

void close_audio()
{
	;
}

void set_mix (int percent)
{
	;
}
