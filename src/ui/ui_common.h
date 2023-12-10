#ifndef __VECTRL_UI_COMMON_H__
#define __VECTRL_UI_COMMON_H__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/defs.h"
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum {
    // Menu items
    App_Quit = wxID_EXIT,
    App_About = wxID_ABOUT,
    App_RestoreDefaultTemplate = wxID_RESET,
    App_ApplyTemplate = wxID_APPLY,
    App_StartWatching = 101,
    App_StopWatching,
    // Thread commands
    Thread_Connected = 201,
    Thread_Disconnected,
};

#endif
