#include "menus.h"
#include "motiftools.h"
#include "guidefs.h"
#include "helpdiag.h"
#include "dialogs.h"
#include "vogltools.h"
#include "gophwin.h"

void Edit_Copy_CB();



/* Pulldown menus are built from cascade buttons, so this function
 * also includes pullright menus.  Create the menu, the cascade button
 * that owns the menu, and then the submenu items.
 */
Widget
BuildMenu(Widget   parent,
		  int      menu_type,
		  char     *menu_title, 
		  char     menu_mnemonic, 
		  Boolean  tear_off,
		  MenuItem *items)
{
    Widget menu, cascade, widget;
    int i;
    XmString str;

    if (menu_type == XmMENU_PULLDOWN || menu_type == XmMENU_OPTION)
        menu = XmCreatePulldownMenu (parent, "_pulldown", NULL, 0);
    else if (menu_type == XmMENU_POPUP)
        menu = XmCreatePopupMenu (parent, "_popup", NULL, 0);
    else {
        XtWarning ("Invalid menu type passed to BuildMenu()");
        return NULL;
    }
    if (tear_off)
        XtVaSetValues (menu, XmNtearOffModel, XmTEAR_OFF_ENABLED, NULL);

    if (strcmp(menu_title, "Help") == 0) {
	 XtVaSetValues(parent, XmNmenuHelpWidget, widget, NULL);
    }


    /* Pulldown menus require a cascade button to be made */
    if (menu_type == XmMENU_PULLDOWN) {
        str = XmStringCreateSimple (menu_title);
        cascade = XtVaCreateManagedWidget (menu_title,
            xmCascadeButtonGadgetClass, parent,
            XmNsubMenuId,   menu,
            XmNlabelString, str,
            XmNmnemonic,    menu_mnemonic,
            NULL);
        XmStringFree (str);
    } 
    else if (menu_type == XmMENU_OPTION) {
        /* Option menus are a special case, but not hard to handle */
        Arg args[5];
        int n = 0;
        str = XmStringCreateSimple (menu_title);
        XtSetArg (args[n], XmNsubMenuId, menu); n++;
        XtSetArg (args[n], XmNlabelString, str); n++;
        /* This really isn't a cascade, but this is the widget handle
         * we're going to return at the end of the function.
         */
        cascade = XmCreateOptionMenu (parent, menu_title, args, n);
        XmStringFree (str);
    }

    /* Now add the menu items */
    for (i = 0; items[i].label != NULL; i++) {
        /* If subitems exist, create the pull-right menu by calling this
         * function recursively.  Since the function returns a cascade
         * button, the widget returned is used..
         */
        if (items[i].subitems)
            if (menu_type == XmMENU_OPTION) {
                XtWarning ("You can't have submenus from option menu items.");
                continue;
            } 
            else
                widget = BuildMenu (menu, XmMENU_PULLDOWN, items[i].label, 
                    items[i].mnemonic, tear_off, items[i].subitems);
        else
            widget = XtVaCreateManagedWidget (items[i].label,
                *items[i].class, menu,
                NULL);

        /* Whether the item is a real item or a cascade button with a
         * menu, it can still have a mnemonic.
         */
        if (items[i].mnemonic)
            XtVaSetValues (widget, XmNmnemonic, items[i].mnemonic, NULL);

        /* any item can have an accelerator, except cascade menus. But,
         * we don't worry about that; we know better in our declarations.
         */
        if (items[i].accelerator) {
            str = XmStringCreateSimple (items[i].accel_text);
            XtVaSetValues (widget,
                XmNaccelerator, items[i].accelerator,
                XmNacceleratorText, str,
                NULL);
            XmStringFree (str);
        }

        if (items[i].callback)
            XtAddCallback (widget,
                (items[i].class == &xmToggleButtonWidgetClass ||
                 items[i].class == &xmToggleButtonGadgetClass) ?
                    XmNvalueChangedCallback : /* ToggleButton class */
                    XmNactivateCallback,      /* PushButton class */
                items[i].callback, items[i].callback_data);
   }



    /* for popup menus, just return the menu; pulldown menus, return
     * the cascade button; option menus, return the thing returned
     * from XmCreateOptionMenu().  This isn't a menu, or a cascade button!
     */
    return menu_type == XmMENU_POPUP ? menu : cascade;
}

extern void MenuOpenURL();
extern void MenuReload();
extern void MenuShowGopherMenu();

static MenuItem FileMenuItems[] = {
/* These don't work yet. -- Cameron */
/*
{"New Document", &xmPushButtonGadgetClass, '\0', NULL, NULL, 
      MenuNewDocument, NULL, NULL },
{"New Item List", &xmPushButtonGadgetClass, '\0', NULL, NULL, 
      NULL, NULL, NULL},
{"", &xmSeparatorGadgetClass, '\0', NULL, NULL,
      NULL, NULL, NULL},
*/
{"Show Gopher Menu...", &xmPushButtonGadgetClass, 'M', "Alt<Key>M", "Alt-M",
	/* V_GenMenuListWin, */ MenuShowGopherMenu, NULL, NULL },
{"Reload Gopher Menu", &xmPushButtonGadgetClass, 'M', "Alt<Key>R", "Alt-R",
	MenuReload, NULL, NULL},
{"Open Location...", &xmPushButtonGadgetClass, 'O', "Alt<Key>O", "Alt-O", 
      MenuOpenURL, NULL, NULL},
{"Print...", &xmPushButtonGadgetClass, 'P', "Alt<Key>P", "Alt-P", 
      MenuNotImplemented, NULL, NULL},
{"", &xmSeparatorGadgetClass, '\0', NULL, NULL,
      NULL, NULL, NULL},
{"Preferences...", &xmPushButtonGadgetClass, '\0', NULL, NULL, 
     MenuNotImplemented, NULL, NULL },
{"", &xmSeparatorGadgetClass, '\0', NULL, NULL,
      NULL, NULL, NULL},
{"Quit", &xmPushButtonGadgetClass, 'Q', "Alt<Key>Q", "Alt-Q", 
      MenuQuit_CB, (XtPointer)4, NULL},
NULL
};



static MenuItem EditMenuItems[] = {
{"Undo", &xmPushButtonGadgetClass, '\0', NULL, NULL, 
      NULL, NULL, NULL },
{"", &xmSeparatorGadgetClass, '\0', NULL, NULL,
      NULL, NULL, NULL},
{"Cut", &xmPushButtonGadgetClass, 'X', "Alt<Key>X", "Alt-X", 
      NULL, NULL, NULL},
{"Copy", &xmPushButtonGadgetClass, 'C', "Alt<Key>C", "Alt-C",
      Edit_Copy_CB, "xx", NULL},
{"Paste", &xmPushButtonGadgetClass, 'V', "Alt<Key>V", "Alt-V", 
      NULL, NULL, NULL},
{"Clear", &xmPushButtonGadgetClass, '\0', NULL, NULL, 
      NULL, NULL, NULL},
NULL
};

static MenuItem BookmarksMenuItems[] = {
{"Bookmarks...", &xmPushButtonGadgetClass, 'B', "Alt<Key>B", "Alt-B",
      MenuNotImplemented, NULL, NULL},
{"Add This Menu to Bookmarks", &xmPushButtonGadgetClass, 'D', 
	"Alt<Key>D", "Alt-D",
      MenuNotImplemented, NULL, NULL},
NULL
};


static MenuItem HelpMenuItems[] = {
{"About Gopher VR...", &xmPushButtonGadgetClass, 'A', NULL, NULL, 
      MenuAboutBox, NULL, NULL },
{"", &xmSeparatorGadgetClass, '\0', NULL, NULL,
      NULL, NULL, NULL},
{"Help Topics", &xmPushButtonGadgetClass, '\0', NULL, NULL,
		V_HelpWin, d_USING, NULL},
NULL
};

extern void MenuOverview();
extern void MenuJumpForward();
extern void MenuJumpUp();
extern void MenuSpinOut();
extern void MenuSicko();
extern void MenuInitialView();
extern void MenuMoveUp();
extern void MenuMoveDown();

static MenuItem NavigateItems[] = {
{"Initial Viewpoint",  &xmPushButtonGadgetClass, 'I', "Alt<Key>I", "Alt-I", 
      MenuInitialView, NULL, NULL},
{"Overview", &xmPushButtonGadgetClass, '\0', NULL, "Space",
      MenuOverview, NULL, NULL},
{"Jump Forward", &xmPushButtonGadgetClass, '\0', NULL, NULL,
      MenuJumpForward, NULL, NULL},
{"Jump Up", &xmPushButtonGadgetClass, '\0', NULL, NULL, 
      MenuJumpUp, NULL, NULL},
{"", &xmSeparatorGadgetClass, '\0', NULL, NULL,
      NULL, NULL, NULL},
{"Up", &xmPushButtonGadgetClass, '\0', NULL, "[",
      MenuMoveUp, NULL, NULL},
{"Down", &xmPushButtonGadgetClass, '\0', NULL, "]",
      MenuMoveDown, NULL, NULL},
NULL
};

/*
{"", &xmSeparatorGadgetClass, '\0', NULL, NULL,
      NULL, NULL, NULL},
{"Spin Out", &xmPushButtonGadgetClass, '\0', NULL, NULL, 
      MenuSpinOut, NULL, NULL},
{"Motion Sickness", &xmPushButtonGadgetClass, '\0', NULL, NULL, 
      MenuSicko, NULL, NULL},

*/

MenuItem *StandardHelpMenu = HelpMenuItems;

/**********************************************************************/

Widget
Create_Menubar(Widget parent)
{
     Widget    menubar, menu, widget;

     menubar = XmCreateMenuBar (parent, "menubar", NULL, 0);

     BuildMenu(menubar, XmMENU_PULLDOWN, "File", 'F', True, FileMenuItems);
     BuildMenu(menubar, XmMENU_PULLDOWN, "Edit", 'E', True, EditMenuItems);
     BuildMenu(menubar, XmMENU_PULLDOWN, "Bookmarks", 'B',
		True, BookmarksMenuItems);
     BuildMenu(menubar, XmMENU_PULLDOWN, "Navigate", 'N', True, NavigateItems);
     BuildMenu(menubar, XmMENU_PULLDOWN, "Help", '\0', True, HelpMenuItems);

     if (widget = XtNameToWidget(menubar, "Help"))
	  XtVaSetValues(menubar, XmNmenuHelpWidget, widget, NULL);


     /* Menubar is done -- manage it */
     XtManageChild (menubar);

     return(menubar);

}
