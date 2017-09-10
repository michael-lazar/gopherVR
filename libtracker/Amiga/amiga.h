/* amiga/amiga.h 
	vi:ts=3 sw=3:
 */

/* $Id: amiga.h,v 1.1.1.1 2002/01/18 16:34:24 lindner Exp $
 * $Log: amiga.h,v $
 * Revision 1.1.1.1  2002/01/18 16:34:24  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:39  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 1.16  1995/01/13  13:31:35  espie
 * *** empty log message ***
 *
 *
 * Revision 1.15  1994/01/09  04:49:18  Espie
 * File requester !
 * Added pause gadget.
 * Uncentralized event handling using event management functions.
 * Scroller back calls.
 * Fully working asynchronous interface.
 * Problem with output ? Still to fix !
 * new TYPE_SYNC_DO.
 * Protos for obtain_message, send.
 *
 */

#define forever for(;;)

/* list scanning.
 * next is needed: with it, we can actually unlink the node while scanning
 * the current list.  Type is provided to avoid type-casting errors in
 * SCANLIST expansion.
 */
#define SCANLIST(node, next, list, type) \
   for((node) = (type)((struct MinList *)(list))->mlh_Head; \
      (next) = (type)((struct MinNode *)(node))->mln_Succ; \
      (node) = (next))


#define PUBLIC_PORT_NAME "Debug this tracker"

/* number of messages to allocate */
#define BUFFER_SIZE 500
/* (total size: 500 * 32 = 16000 bytes, corresponding to roughly
 * 1.5 second */

struct ext_message
   {
   struct Message msg;
#define TYPE_DIE 0
#define TYPE_WAIT 1
#define TYPE_FLUSH_CHANNEL 2
#define TYPE_SETUP 3
#define TYPE_CHANGE 4
#define TYPE_FLUSH_BUFFER 5
#define TYPE_COMM 6
#define TYPE_SYNC 7
#define TYPE_SYNC_DO 8
#define TYPE_INVALID 9
#define TYPE_PAUSE 10
#define TYPE_UNPAUSE 11

   int type;
   union
      {
      struct
         {
         void (*func)(VALUE p);
         VALUE p;
         } hook;
      struct 
         {
         ULONG high;
         ULONG low;
         } time;
      struct
         {
         void *start;
         ULONG length;
         } sample;
      struct
         {
         USHORT channel_mask;
         USHORT cycle;
         USHORT pitch;
         USHORT volume;
         } info;
      struct
         {
         struct MsgPort *port;
         struct Task *task;
         } comm;
      } data;
   };


/* the audio server entry point */      
XT void subtask P((struct ext_message *msg));

/* Standard functions of the client */

/* mes = obtain_message():
 *    obtain a message we can send to the server
 *    (this call may block, but usually not for long)
 */
XT struct ext_message *obtain_message P((void));

/* mes = await_type(type):
 *    wait for the return of the next message of a given type
 *    and returns it. Synchronization with the server.
 */
XT struct ext_message *await_type P((int type));

/* send(msg, type):
 *    send msg to the server, filling type along the way
 */
XT void send P((struct ext_message *msg, int type));


XT void add_scroller P((char *s));

XT struct Screen *obtain_pubscreen(void);

XT void set_break(void);
XT void check_events(void);
XT void await_events(void);
XT void install_signal_handler(int signal, void (*f)(GENERIC data), GENERIC data);
XT void remove_signal_handler(int signal);
XT void install_req_handler(ULONG mask, void (*req_f)(ULONG received));
XT void remove_req_handler(void);

struct amiganame
   {
   struct MinNode n;
   int i;
   char s[0];  
   };

/* prototypes for the file requester */

XT void launch_requester P((void));
XT void requested_file P((struct amiganame *name));

/* V39 function */
XT void set_busy_pointer P((BOOL maybe));
