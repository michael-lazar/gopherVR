#include <X11/Intrinsic.h>	/* For Widget etal */
/* A simple status widget */
void StatusNew(Widget parent, char *msg);
void StatusUpdate(char *msg);

/* A busy thing-a-ma-bob */
void BusyThingNew(Widget parent);
void BusyUpdate(void);


/* Common Menu callbacks */
void MenuQuit_CB(Widget w, XtPointer menuitem, XtPointer call_data);
void MenuAboutBox(Widget w, XtPointer menuitem, XtPointer call_data);
void MenuNewDocument(Widget w, XtPointer menuitem, XtPointer call_data);
void MenuNotImplemented(Widget w, XtPointer menuitem, XtPointer call_data);
void filepicker(char *suggestion, void *gs);

#define PixFromBitmap(bits, w, h)  XCreatePixmapFromBitmapData(XtDisplay(top),\
                           RootWindowOfScreen(XtScreen(top)), bits,\
			   w, h, fg, bg,\
			   DefaultDepthOfScreen(XtScreen(top)));
