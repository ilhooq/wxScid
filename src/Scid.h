/*
 * Scid.h
 *
 */
#ifndef SCID_H_
#define SCID_H_

#include <wx/event.h>

#include <exception>
#include <string>

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

struct ScidDatabaseEntry
{
  std::string result;
  unsigned short moves_number;
  std::string white_name;
  std::string black_name;
  std::string white_elo;
  std::string black_elo;
  char date[16];
  std::string event_name;
  std::string round_name;
  std::string site_name;
  unsigned int nag_count;
  unsigned int comment_count;
  unsigned int variation_count;
  bool deleted_flag;
  char flags[16];
  char eco[6];
  std::string end_material;
  bool start_flag;
  char event_date[16];
  unsigned int year;
  unsigned char rating;
  std::string first_moves;
  unsigned int index;
  unsigned int ply;
};


class ScidListEventHandler
{
public:
  virtual ~ScidListEventHandler() {};
  virtual void onListGetEntry(ScidDatabaseEntry entry) = 0;
};

class Scid : public wxEvtHandler
{
public:
  Scid();
  ~Scid();
  void openDatabase(wxCommandEvent& evt);
  void OnListGames(wxCommandEvent& evt);
  void listGames(int baseHandle, const char* ordering, const char* filterId, ScidListEventHandler* eventHandler, unsigned int start=0, unsigned int count=0);
  /* Get the number of games in the database */
  unsigned int numGames(int baseHandle);
private:
  int currentDbHandle;
  wxDECLARE_EVENT_TABLE();
};



#endif /* SCID_H_ */
