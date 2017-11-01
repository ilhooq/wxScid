/*
 * GameTxtCtrl.h
 *
 */

#ifndef WIDGETS_GAMETXTCTRL_H_
#define WIDGETS_GAMETXTCTRL_H_

#include <wx/richtext/richtextctrl.h>

class GameTxtCtrl : public wxRichTextCtrl
{
public:
  GameTxtCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& point = wxDefaultPosition, const wxSize& size = wxDefaultSize);

  ~GameTxtCtrl(){}

private:
  wxDECLARE_EVENT_TABLE();
};


#endif /* WIDGETS_GAMETXTCTRL_H_ */
