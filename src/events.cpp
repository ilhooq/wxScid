/*
 * events.cpp
 *
 */

#include "events.h"

// Events definitions
wxDEFINE_EVENT(EVT_OPEN_DATABASE_REQUEST, wxCommandEvent);
wxDEFINE_EVENT(EVT_OPEN_DATABASE, wxCommandEvent);
wxDEFINE_EVENT(EVT_LISTGAMES_REQUEST, wxCommandEvent);
wxDEFINE_EVENT(EVT_LOAD_GAME_REQUEST, wxCommandEvent);
wxDEFINE_EVENT(EVT_GAME_LOADED, wxCommandEvent);
wxDEFINE_EVENT(EVT_MAKE_MOVE, wxCommandEvent);
