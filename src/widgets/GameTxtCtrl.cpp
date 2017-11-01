/*
 * GameTxtCtrl.cpp
 *
 */
#include <wx/wx.h>
#include <wx/richtext/richtextctrl.h>
#include "GameTxtCtrl.h"

BEGIN_EVENT_TABLE(GameTxtCtrl, wxRichTextCtrl)

END_EVENT_TABLE()

GameTxtCtrl::GameTxtCtrl(wxWindow* parent, wxWindowID id, const wxPoint &pos, const wxSize &size)
: wxRichTextCtrl( parent, id, "", pos, size,  wxNO_BORDER+wxWANTS_CHARS+wxRE_MULTILINE+wxRE_READONLY)
{

}
