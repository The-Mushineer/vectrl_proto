#ifndef __VECTRL_CONTROLLER_DESCRIPTION_H__
#define __VECTRL_CONTROLLER_DESCRIPTION_H__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/string.h"
#endif

struct ButtonInfo {
    int number{0};
    wxString label;
};

struct EncoderInfo {
    int number{0};
    int buttonNumber{0};
    wxString label;
};

struct ControllerDescription {
    wxString name;
    std::vector<ButtonInfo> buttons;
    std::vector<EncoderInfo> encoders;
};

extern const ControllerDescription VECTRL_PROTO_DESCRIPTION;

#endif