/* options.h
   xgopher application resources and program options */

     /*---------------------------------------------------------------*/
     /* Moog           version 0.0     27 October 1992                */
     /* Xgopher        version 1.1     20 April 1991                  */
     /*                version 1.0     04 March 1991                  */
     /*                                                               */
     /* X window system client for the University of Minnesota        */
     /*                                Internet Gopher System.        */
     /*                                                               */
     /* Martin Hamilton,  Loughborough University of Technology       */
     /*                   Department of Computer Studies              */
     /*                                                               */
     /* Allan Tuchman,    University of Illinois at Urbana-Champaign  */
     /*                   Computing Services Office                   */
     /*                                                               */
     /* Jonathan Goldman, WAIS project                                */
     /*                   Thinking Machines Corporation               */
     /*                                                               */
     /* Copyright 1992 by                                             */
     /*           the Board of Trustees of the University of Illinois */
     /* Permission is granted to freely copy and redistribute this    */
     /* software with the copyright notice intact.                    */
     /*---------------------------------------------------------------*/

#ifndef G_OPTIONS_H
#define G_OPTIONS_H

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#ifdef XGOPHER_X11R4
typedef char	*XPointer;
#endif

typedef struct {
	Boolean		defaultsInstalled;
	String		rootServer;
	int		rootPort;
	String		mainTitle;
	Boolean		allowSave;
	Boolean		allowPrint;
	String		printCommand;
	String		helpFile;
	int		directoryTime;
	Boolean		hasSound;
	String		soundCommand;
	Boolean		allowTelnet;
	String		telnetCommand;
	int		itemStart;
	int		itemIncrement;
	int		dirStart;
	int		dirIncrement;
	Boolean		doubleClick;
	Boolean		markRoot;
	String		tempDirectory;
	String		logFile;
	String		prefixFile;
	String		prefixDir;
	String		prefixTelnet;
	String		prefixCSO;
	String		prefixIndex;
	String		prefixSound;
	String		prefixUnknown;
	} gopherAppResources, *gopherAppResourcesP;


/* conf.h contains the default values for many of the resources */

#include "conf.h"

#define GOffset(x) XtOffset(gopherAppResourcesP, x)

static XtResource resources[] = {
    {"defaultsInstalled", "DefaultsInstalled", XtRBoolean, sizeof(Boolean),
                GOffset(defaultsInstalled), XtRImmediate, (XPointer) False},
    {"rootServer", "RootServer", XtRString, sizeof(String),
		GOffset(rootServer), XtRString, ROOT_SERVER},
    {"rootPort", "RootPort", XtRInt, sizeof(int),
		GOffset(rootPort), XtRImmediate, (XPointer) ROOT_PORT},
    {"mainTitle", "MainTitle", XtRString, sizeof(String),
		GOffset(mainTitle), XtRString, MAIN_TITLE},
    {"allowSave", "AllowSave", XtRBoolean, sizeof(Boolean),
		GOffset(allowSave), XtRImmediate, (XPointer) ALLOW_SAVE},
    {"allowPrint", "AllowPrint", XtRBoolean, sizeof(Boolean),
		GOffset(allowPrint), XtRImmediate, (XPointer) ALLOW_PRINT},
    {"printCommand", "PrintCommand", XtRString, sizeof(String),
		GOffset(printCommand), XtRString, PRINT_COMMAND},
    {"helpFile", "HelpFile", XtRString, sizeof(String),
		GOffset(helpFile), XtRString, HELP_FILE},
    {"directoryTime", "DirectoryTime", XtRInt, sizeof(int),
		GOffset(directoryTime), XtRImmediate, (XPointer)DIRECTORY_TIME},
    {"hasSound", "HasSound", XtRBoolean, sizeof(Boolean),
		GOffset(hasSound), XtRImmediate, (XPointer) HAS_SOUND},
    {"soundCommand", "SoundCommand", XtRString, sizeof(String),
		GOffset(soundCommand), XtRString, SOUND_COMMAND},
    {"allowTelnet", "AllowTelnet", XtRBoolean, sizeof(Boolean),
		GOffset(allowTelnet), XtRImmediate, (XPointer) ALLOW_TELNET},
    {"telnetCommand", "TelnetCommand", XtRString, sizeof(String),
		GOffset(telnetCommand), XtRString, TELNET_COMMAND},
    {"itemStart", "ItemStart", XtRInt, sizeof(int),
		GOffset(itemStart), XtRImmediate, (XPointer) ITEMS_TO_START},
    {"itemIncrement", "ItemIncrement", XtRInt, sizeof(int),
		GOffset(itemIncrement), XtRImmediate, (XPointer) ITEMS_TO_ADD},
    {"dirStart", "DirStart", XtRInt, sizeof(int),
		GOffset(dirStart), XtRImmediate, (XPointer) DIRS_TO_START},
    {"dirIncrement", "DirIncrement", XtRInt, sizeof(int),
		GOffset(dirIncrement), XtRImmediate, (XPointer) DIRS_TO_ADD},
    {"doubleClick", "DoubleClick", XtRBoolean, sizeof(Boolean),
		GOffset(doubleClick), XtRImmediate, (XPointer) DOUBLE_CLICK},
    {"markRoot", "MarkRoot", XtRBoolean, sizeof(Boolean),
		GOffset(markRoot), XtRImmediate, (XPointer) MARK_ROOT},
    {"tempDirectory", "TempDirectory", XtRString, sizeof(String),
		GOffset(tempDirectory), XtRString, TEMP_DIRECTORY},
    {"logFile", "LogFile", XtRString, sizeof(String),
		GOffset(logFile), XtRImmediate, NULL},
    {"prefixFile", "prefixFile", XtRString, sizeof(String),
		GOffset(prefixFile), XtRString, PREFIX_FILE},
    {"prefixDir", "prefixDir", XtRString, sizeof(String),
		GOffset(prefixDir), XtRString, PREFIX_DIR},
    {"prefixTelnet", "prefixTelnet", XtRString, sizeof(String),
		GOffset(prefixTelnet), XtRString, PREFIX_TELNET},
    {"prefixCSO", "prefixCSO", XtRString, sizeof(String),
		GOffset(prefixCSO), XtRString, PREFIX_CSO},
    {"prefixIndex", "prefixIndex", XtRString, sizeof(String),
		GOffset(prefixIndex), XtRString, PREFIX_INDEX},
    {"prefixSound", "prefixSound", XtRString, sizeof(String),
		GOffset(prefixSound), XtRString, PREFIX_SOUND},
    {"prefixUnknown", "prefixUnknown", XtRString, sizeof(String),
		GOffset(prefixUnknown), XtRString, PREFIX_UNKNOWN},
    };

#undef GOffset

#endif   /* G_OPTIONS_H */
