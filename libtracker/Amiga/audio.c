/* amiga/audio.c 
	vi:ts=3 sw=3:
 */
/* $Id: audio.c,v 1.1.1.1 2002/01/18 16:34:24 lindner Exp $
 * $Log: audio.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:24  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:39  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 1.11  1995/02/14  16:51:22  espie
 * *** empty log message ***
 *
 * Revision 1.10  1995/02/13  22:05:42  espie
 * Implemented new allocation policy.
 * Updated calls to refer to new release.
 *
 * Revision 1.9  1995/01/13  13:31:35  espie
 * *** empty log message ***
 *
 *
 * Revision 1.6  1994/01/08  04:00:52  Espie
 * added obtain_sample, renamed release_audio_channel,
 * new_channel_tag_list.
 * Major change: moved timing computations to the audio server.
 * Pause is now feasible !
 * Better automatic time adjustement.
 * Ability to play long samples.
 * Modified task pseudoinit in case
 * we use an external server.
 * Removed some typecasts.
 * Added SYNC_DO hook facility.
 * FLUSH_BUFFER on end_all.
 * Messages now initialized correctly
 * Added external sound server for debug.
 */

#include <proto/exec.h>
#include <proto/timer.h>

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/ports.h>

#include "defs.h"
#include "extern.h"
#include "song.h"
#include "tags.h"
#include "amiga/amiga.h"

ID("$Id: audio.c,v 1.1.1.1 2002/01/18 16:34:24 lindner Exp $")

#define AMIGA_MAX_SAMPLE_LENGTH  131072
#define AMIGA_MIN_SAMPLE_LENGTH  2

LOCAL void init_audio(void);
LOCAL void (*INIT)(void) = init_audio;

XT unsigned int inhibit_output;

LOCAL struct Library *TimerBase = 0;
LOCAL struct timerequest *tr = 0;

/* remember allocated samples for cleaning up in panic case */
LOCAL struct MinList tracked_list;


LOCAL void free_things()
   {
   struct MinNode *current, *next;

   SCANLIST(current, next, &tracked_list, struct MinNode *)
      FreeVec(current);
   if (TimerBase)
      CloseDevice(tr);
   if (tr)
      FreeVec(tr);
   }
      

/* kludge alert: we allocate arbitrary channel numbers around here.
 * This is probably temporary, and definitely NOT the way to go !
 */

LOCAL struct audio_channel
   {
   int amiga_number;
   struct sample_info *samp;
   int volume;
   int pitch;
   } chan[4];

LOCAL int allocated = 0;
LOCAL int left_allocated = 0;
LOCAL int right_allocated = 0;

LOCAL int free_channel[2][3] =
   {
      {0,3,-1},
      {1,2,-1}
   };

struct audio_channel *new_channel_tag_list(struct tag *prop)
   {
   struct audio_channel *new;

   INIT_ONCE;

   new = &chan[allocated];
   while (prop = get_tag(prop))
      {
      switch(prop->type)
         {
      case AUDIO_SIDE:
         switch(prop->data.scalar)
            {
         case LEFT_SIDE:
            new->amiga_number = free_channel[LEFT_SIDE][left_allocated];
            if (new->amiga_number != -1)
               left_allocated++;
            else
               return NULL;
            break;
         case RIGHT_SIDE:
            new->amiga_number = free_channel[RIGHT_SIDE][right_allocated];
            if (new->amiga_number != -1)
               right_allocated++;
            else
               return NULL;
            break;
         default:
            return NULL;
            }
         break;
      default:
         }
      prop++;
      }
         
   allocated++;
   new->samp = empty_sample();
   new->volume = 0;
   new->pitch = 0;
   new->samp = 0;
   return new;
   }

void release_audio_channels()
   {
   left_allocated = right_allocated = allocated = 0;
   }


/* Special sample management on the amiga */
void *alloc_sample(int len)
   {
   char *s;

   INIT_ONCE
   
   s = AllocVec(len + sizeof(struct MinNode), MEMF_CHIP | MEMF_CLEAR);
   if (!s)
      return 0;
   AddTail(&tracked_list, (struct Node *)s);
   return s + sizeof(struct MinNode);
   }


/* note we have to synchronize audio output before we free the sample */
#define sync_audio close_audio

void free_sample(char *s)
   {
   sync_audio();
   s -= sizeof(struct MinNode);
   Remove((struct Node *)s);
   FreeVec(s);
   }

int obtain_sample(char *start, int l, struct exfile *f)
   {
   return read_file(start, 1, l, f);
   }


LOCAL void init_timer()
   {
   int error;

   tr = AllocVec(sizeof(struct timerequest), MEMF_PUBLIC | MEMF_CLEAR);
   if (!tr)
      end_all("No timerequest");
   error = OpenDevice(TIMERNAME, UNIT_MICROHZ, tr, 0);
   if (error)
      end_all("Could not open timer");
   TimerBase = (struct Library *)tr->tr_node.io_Device;
   }


LOCAL void init_audio()
   {
   NewList(&tracked_list);
   at_end(free_things);
   init_timer();
   }

/* indispensible for not losing memory ! */

void __regargs __chkabort()
   {
   }
   
/* audio */

/* empty operation on the amiga */
void init_tables(oversample, frequency)
int oversample, frequency;
   {
   }

void resample(oversample, number)
int oversample;
int number;
   {
   struct ext_message *msg;   

   INIT_ONCE;
   msg = obtain_message();
   msg->data.time.low = (unsigned long)number;
   msg->data.time.high = 0;
   send(msg, TYPE_WAIT);
   }


/* So you can play long samples with short repeat parts */
LOCAL void play_long_sample(struct audio_channel *au, UBYTE *start, int length)
   {
   struct ext_message *msg, *msg2;
   int chunk;
   while (length >= AMIGA_MIN_SAMPLE_LENGTH)
      {
      chunk = MIN(AMIGA_MAX_SAMPLE_LENGTH, length);
      msg = obtain_message();
      msg2 = obtain_message();
      msg->data.sample.start = start;
      msg->data.sample.length = chunk;
      msg2->data.info.channel_mask = 1<<au->amiga_number;
      msg2->data.info.cycle = 1;
      msg2->data.info.pitch = au->pitch;
      msg2->data.info.volume = au->volume;
      Forbid();
      send(msg, TYPE_SETUP);
      send(msg2, TYPE_CHANGE);
      Permit();
      length -= chunk;
      start += chunk;
      }
   }

void play_note(au, samp, pitch)
struct audio_channel *au;
struct sample_info *samp;
int pitch;
   {
   struct ext_message *msg, *msg2;

   au->pitch = pitch;
   msg = obtain_message();
   msg->data.info.channel_mask = 1<<au->amiga_number;
   send(msg, TYPE_FLUSH_CHANNEL);
   
   if (samp)
      {
      au->samp = samp;
      if (samp->start)
         {
         play_long_sample(au, samp->start, samp->length);
         if (samp->rp_start)
            {
            msg = obtain_message();
            msg2 = obtain_message();
            msg->data.sample.start = samp->rp_start;
            msg->data.sample.length = samp->rp_length;
            msg2->data.info.channel_mask = 1<<au->amiga_number;
            msg2->data.info.cycle = 0;
            msg2->data.info.pitch = 0;
            msg2->data.info.volume = 0;
            Forbid();
            send(msg, TYPE_SETUP);
            send(msg2, TYPE_CHANGE);
            Permit();
            }  
         }
      }
   }

void set_play_pitch(au, pitch)
struct audio_channel *au;
int pitch;
   {
   if (pitch != au->pitch)
      {
      struct ext_message *msg;

      au->pitch = pitch;
      msg = obtain_message();
      msg->data.info.channel_mask = 1<<au->amiga_number;
      msg->data.info.pitch = au->pitch;
      msg->data.info.volume = au->volume;
      send(msg, TYPE_CHANGE);
      }
   }

void set_play_volume(au, volume)
struct audio_channel *au;
int volume;
   {
   if (volume != au->volume)
      {
      struct ext_message *msg;

      au->volume = volume;
      if (au->samp != empty_sample())
         {
         msg = obtain_message();
         msg->data.info.channel_mask = 1<<au->amiga_number;
         msg->data.info.pitch = au->pitch;
         msg->data.info.volume = au->volume;
         send(msg, TYPE_CHANGE);
         }
      }
   }

void set_play_position(au, pos)
struct audio_channel *au;
int pos;
   {
   struct ext_message *msg, *msg2;
   
   
   msg = obtain_message();
   msg->data.info.channel_mask = 1<<au->amiga_number;
   send(msg, TYPE_FLUSH_CHANNEL);

   if (au->samp->start)
      {
      if (pos < au->samp->length)
         {
         play_long_sample(au, au->samp->start + pos, au->samp->length - pos);
         if (au->samp->rp_start)
            {
            msg = obtain_message();
            msg2 = obtain_message();
            msg->data.sample.start = au->samp->rp_start;
            msg->data.sample.length = au->samp->rp_length;
            msg2->data.info.channel_mask = 1<<au->amiga_number;
            msg2->data.info.cycle = 0;
            msg2->data.info.pitch = 0;
            msg2->data.info.volume = 0;
            Forbid();
            send(msg, TYPE_SETUP);
            send(msg2, TYPE_CHANGE);
            Permit();
            }  
         }
      }
   }



void set_mix(percent)
int percent;
   {
   }

int open_audio(f, s)
int f, s;
   {
   ULONG freq;
   LOCAL struct EClockVal dummy;

   INIT_ONCE;

      /* samples/sec used as a timing unit: 1sec =1 000 000 µs */
   freq = ReadEClock(&dummy);
   return (int)freq;
   }
   
void set_synchro(s)
int s;
   {
   }

int update_frequency()
   {
   return 0;
   }

void flush_buffer(void)
   {
   }

void discard_buffer(void)
   {
   struct ext_message *msg;
   
   inhibit_output++;
   msg = obtain_message();
   send(msg, TYPE_FLUSH_BUFFER);
   msg = obtain_message();
   send(msg, TYPE_SYNC);
   while (msg != await_type(TYPE_SYNC))
      ;
   inhibit_output--;
   }

