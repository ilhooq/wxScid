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
wxDECLARE_EVENT(EVT_OPEN_DATABASE_ENTRY_REQUEST, wxCommandEvent);

#endif /* EVENTS_H_ */
