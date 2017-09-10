/* common.c */

/* common repository of code/macros for all architectures audio */
/* $Id: common.c,v 1.1.1.1 2002/01/18 16:34:22 lindner Exp $ */
/* $Log: common.c,v $
/* Revision 1.1.1.1  2002/01/18 16:34:22  lindner
/* Slightly modded sources for gophervr...
/*
 * Revision 1.1.1.1  1995/06/19  22:48:40  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 1.6  1995/02/27  14:25:37  espie
 * Rolf Grossmann patch.
 *
 * Revision 1.5  1995/02/26  23:07:14  espie
 * Changed sync to tsync.
 *
 * Revision 1.4  1995/02/23  23:33:01  espie
 * Linear resampling changed.
 *
 * Revision 1.3  1995/02/23  22:41:45  espie
 * Added # of bits.
 *
 * Revision 1.2  1995/02/23  17:03:14  espie
 * Continuing changes for a standard file.
 *
 * Revision 1.1  1995/02/23  16:42:10  espie
 * Initial revision
 *
 *
 */

#define abs(x) ((x) < 0 ? -(x) : (x))

/* f' = best_frequency(f, table, def):
 * return nearest frequency in sorted table,
 * unless f == 0 -> return def
 */
LOCAL int best_frequency(f, table, def)
int f;
int table[];
int def;
	{
	int best = table[0];
	int i;

	if (f == 0)
		return def;
	for (i = 0; i < table[i]; i++)
		if (abs(table[i] - f) < abs(best - f))
			best = table[i];
	return best;
	}	


/************************************************************************/
/*      For routine 'cvt' only                                          */
/************************************************************************/
/*      Copyright 1989 by Rich Gopstein and Harris Corporation          */
/************************************************************************/

LOCAL unsigned int cvt(ch)
int ch;
    {
    int mask;

    if (ch < 0)
        {
        ch = -ch;
        mask = 0x7f;
        }
    else
        mask = 0xff;

    if (ch < 32)
        {
        ch = 0xF0 | 15 - (ch / 2);
        }
    else if (ch < 96)
        {
        ch = 0xE0 | 15 - (ch - 32) / 4;
        }
    else if (ch < 224)
        {
        ch = 0xD0 | 15 - (ch - 96) / 8;
        }
    else if (ch < 480)
        {
        ch = 0xC0 | 15 - (ch - 224) / 16;
        }
    else if (ch < 992)
        {
        ch = 0xB0 | 15 - (ch - 480) / 32;
        }
    else if (ch < 2016)
        {
        ch = 0xA0 | 15 - (ch - 992) / 64;
        }
    else if (ch < 4064)
        {
        ch = 0x90 | 15 - (ch - 2016) / 128;
        }
    else if (ch < 8160)
        {
        ch = 0x80 | 15 - (ch - 4064) /  256;
        }
    else
        {
        ch = 0x80;
        }
    return (mask & ch);
    }


#ifdef DEFAULT_SET_MIX
LOCAL int stereo;

#ifdef NEW_OUTPUT_SAMPLES_AWARE

LOCAL int pps[32], pms[32];

void set_mix(percent)
int percent;
	{
	int i;

	for (i = 8; i < 32; i++)
		{
		pps[i] = 1 << (31 - i);
		if (i < 29)
			pms[i] = pps[i] - (percent << (29 - i) )/25;
		else
			pms[i] = pps[i] - (percent >> (i - 29) )/25;
		}
	}

#else /* NEW_OUTPUT_SAMPLES_AWARE */
/* old code: optimized away */
/* LOCAL int primary, secondary;	*/
LOCAL int pps, pms;	/* 1/2 primary+secondary, 1/2 primary-secondary */

void set_mix(percent)
int percent;
   {
	percent *= 256;
	percent /= 100;
/*
	secondary = percent;
	primary = 512 - percent;
 */
	pps = 256;
	pms = 256 - percent;
   }
#endif /* NEW_OUTPUT_SAMPLES_AWARE */

#endif /* DEFAULT_SET_MIX */

#ifdef UNSIGNED_BUFFERS
#define UNSIGNED8
#define UNSIGNED16
#endif

#ifdef DEFAULT_BUFFERS

#ifdef UNSIGNED16
LOCAL unsigned short *buffer16;
#define VALUE16(x)	((x)+32768)
#else
LOCAL short *buffer16;
#define VALUE16(x)	(x)
#endif

#ifdef UNSIGNED8
LOCAL unsigned char *buffer;
#define VALUE8(x)		((x)+128)
#else
LOCAL char *buffer;
#define VALUE8(x)		(x)
#endif
LOCAL int idx;
LOCAL int dsize;			/* current data size */
LOCAL int samples_max;	/* number of samples in buffer */

LOCAL int tsync = FALSE;
#endif	/* DEFAULT_BUFFERS */

#ifdef SEPARATE_BUFFERS
LOCAL char *buffer, *buffer_l, *buffer_r;
LOCAL int idx;
#endif



#ifdef NEW_OUTPUT_SAMPLES_AWARE

LOCAL void add_samples16_stereo(left, right, n)
int left, right, n;
	{
	if (pms[n] == pps[n])	/* no mixing */
		{
		    if (n<16)
		    {
		buffer16[idx++] = VALUE16(left << (16-n) );
		buffer16[idx++] = VALUE16(right << (16-n) );
		    }
		    else
		    {
		buffer16[idx++] = VALUE16(left >> (n-16) );
		buffer16[idx++] = VALUE16(right >> (n-16) );
		    }
		}
	else
		{
		int s1, s2;

		s1 = (left+right)*pps[n];
		s2 = (left-right)*pms[n];

		buffer16[idx++] = VALUE16( (s1 + s2) >> 16);
		buffer16[idx++] = VALUE16( (s1 - s2) >> 16);
		}
	}

LOCAL void add_samples16_mono(left, right, n)
int left, right, n;
	{
	    if (n<15)		/* is this possible? */
		buffer16[idx++] = VALUE16( (left + right) << (15-n) );
	    else
		buffer16[idx++] = VALUE16( (left + right) >> (n-15) );
	}

LOCAL void add_samples16(left, right, n)
int left, right, n;
	{
	if (stereo)
		add_samples16_stereo(left, right, n);
	else
		add_samples16_mono(left, right, n);
	}

LOCAL void add_samples8_stereo(left, right, n)
int left, right, n;
	{
	if (pms[n] == pps[n])	/* no mixing */
		{
		    /* if n<8 -> same problem as above,
		       but that won't happen, right? */
		buffer[idx++] = VALUE8(left >> (n-8) );
		buffer[idx++] = VALUE8(right >> (n-8) );
		}
	else
		{
		int s1, s2;

		s1 = (left+right)*pps[n];
		s2 = (left-right)*pms[n];

		buffer[idx++] = VALUE8( (s1 + s2) >> 24);
		buffer[idx++] = VALUE8( (s1 - s2) >> 24);
		}
	}

LOCAL void add_samples8_mono(left, right, n)
int left, right, n;
	{
	    buffer[idx++] = VALUE8( (left+right) >> (n-7) );
	}

LOCAL void add_samples8(left, right, n)
int left, right, n;
	{
	if (stereo)
		add_samples8_stereo(left, right, n);
	else
		add_samples8_mono(left, right, n);
	}

#else

/* don't ask me if this code is correct then (I guess it is) ...
   anyone still using it? */
LOCAL void add_samples16_stereo(left, right)
int left, right;
	{
	if (pms == pps)	/* no mixing */
		{
		buffer16[idx++] = VALUE16(left/256);
		buffer16[idx++] = VALUE16(right/256);
		}
	else
		{
		int s1, s2;

		s1 = (left+right)*pps;
		s2 = (left-right)*pms;

		buffer16[idx++] = VALUE16( (s1 + s2)/65536 );
		buffer16[idx++] = VALUE16( (s1 - s2)/65536 );
		}
	}

LOCAL void add_samples16_mono(left, right)
int left, right;
	{
	buffer16[idx++] = VALUE16( (left + right)/256);
	}

LOCAL void add_samples16(left, right)
int left, right;
	{
	if (stereo)
		add_samples16_stereo(left, right);
	else
		add_samples16_mono(left, right);
	}

LOCAL void add_samples8_stereo(left, right)
int left, right;
	{
	if (pms == pps)	/* no mixing */
		{
		buffer[idx++] = VALUE8(left/65536);
		buffer[idx++] = VALUE8(right/65536);
		}
	else
		{
		int s1, s2;

		s1 = (left+right)*pps;
		s2 = (left-right)*pms;

		buffer[idx++] = VALUE8( (s1 + s2) >> 24);
		buffer[idx++] = VALUE8( (s1 + s2) >> 24);
		}
	}

LOCAL void add_samples8_mono(left, right)
int left, right;
	{
	buffer[idx++] = VALUE8( (left+right) >> 16);
	}

LOCAL void add_samples8(left, right)
int left, right;
	{
	if (stereo)
		add_samples8_stereo(left, right);
	else
		add_samples8_mono(left, right);
	}

#endif

#ifndef NEW_OUTPUT_SAMPLES_AWARE

XT void output_samples P((int left, int right, int n));

void output_samples(left, right, n)
int left, right, n;
	{
	old_output_samples(left >> (n-23), right >> (n-23));
	}
#define output_samples	old_output_samples

#endif
