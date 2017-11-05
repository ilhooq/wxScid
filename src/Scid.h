/*
 * Scid.h
 *
 */
#ifndef SCID_H_
#define SCID_H_

#include <exception>
#include <string>

#include <wx/event.h>
#include <wx/vector.h>

#include "database.h"

class ScidError : std::exception
{
public:
  ScidError(const std::string& msg, int code) throw()
  :m_msg(msg),
   m_code(code)
  {}

  virtual const char* what() const throw()
  {
    return m_msg.c_str();
  }

  int getCode() throw()
  {
    return m_code;
  }

  virtual ~ScidError() throw()
  {
  }

private:
  int m_code;  // Code number
  std::string m_msg;  // Error description
};

class Scid : public wxEvtHandler
{
public:
  Scid();
  ~Scid();
  void openDatabase(wxCommandEvent& evt);
  void LoadGame(wxCommandEvent& evt);
  void OnListGames(wxCommandEvent& evt);
  void listGames(int baseHandle, const char* ordering, const char* filterId, HashGameEntries* hashEntries, unsigned int start=0, unsigned int count=0);
  /* Get the number of games in the database */
  unsigned int numGames(int baseHandle);
private:
  int currentDbHandle;
  wxVector<GamePos> *gameLoaded;
  wxDECLARE_EVENT_TABLE();
};

#endif /* SCID_H_ */
