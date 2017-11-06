/*
 * ChessRules.h
 */

#ifndef EVENTHANDLERS_CHESSRULES_H_
#define EVENTHANDLERS_CHESSRULES_H_

#include <wx/event.h>
#include "database.h"


class ChessRules: public wxEvtHandler
{
public:
    ChessRules();
    ~ChessRules();
private:
wxDECLARE_EVENT_TABLE();
};

#endif /* EVENTHANDLERS_CHESSRULES_H_ */
