/*
 * events.h
 *
 */

#ifndef EVENTS_H_
#define EVENTS_H_

#include <wx/event.h>

wxDECLARE_EVENT(EVT_OPEN_DATABASE_REQUEST, wxCommandEvent);
wxDECLARE_EVENT(EVT_OPEN_DATABASE, wxCommandEvent);
wxDECLARE_EVENT(EVT_LISTGAMES_REQUEST, wxCommandEvent);
wxDECLARE_EVENT(EVT_LOAD_GAME_REQUEST, wxCommandEvent);
wxDECLARE_EVENT(EVT_GAME_LOADED, wxCommandEvent);
wxDECLARE_EVENT(EVT_MAKE_MOVE, wxCommandEvent);

#endif /* EVENTS_H_ */
