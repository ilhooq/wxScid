/*
 * database.h
 *
 */

#ifndef DATABASE_H_
#define DATABASE_H_

#include <wx/string.h>

struct DbInfos
{
    wxString name;
    wxString path;
    int handle;
    int numGames;
};

struct GamePos
{
    int RAVdepth;
    int RAVnum;
    wxString FEN;
    wxString NAGs;
    wxString comment;
    wxString lastMoveSAN;
};

#endif /* DATABASE_H_ */
