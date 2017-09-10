
/***************************************************/
#define MENU_FILE 0
#define MENU_FILE_QUIT 4

/***************************************************/
#define MENU_EDIT 1
#define MENU_EDIT_UNDO  0
#define MENU_EDIT_CUT   1
#define MENU_EDIT_COPY  2
#define MENU_EDIT_PASTE 3

/**  The different stuff you can have in a menubar.. **/

#include <Xm/RowColumn.h>

#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>

#include <Xm/PushB.h>
#include <Xm/PushBG.h>

#include <Xm/Label.h>
#include <Xm/LabelG.h>

#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>

#include <Xm/Separator.h>
#include <Xm/SeparatoG.h>

Widget Create_Menubar(Widget parent);

typedef struct _menu_item {
    char        *label;         /* the label for the item */
    WidgetClass *class;         /* pushbutton, label, separator... */
    char         mnemonic;      /* mnemonic; NULL if none */
    char        *accelerator;   /* accelerator; NULL if none */
    char        *accel_text;    /* to be converted to compound string */
    void       (*callback)();   /* routine to call; NULL if none */
    XtPointer    callback_data; /* client_data for callback() */
    struct _menu_item *subitems; /* pullright menu items, if not NULL */
} MenuItem;

extern MenuItem *StandardHelpMenu;
