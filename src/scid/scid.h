#ifndef libScid_H
#define libScid_H
#include <exception>

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
  const char* result;
  unsigned short moves_number;
  const char* white_name;
  const char* black_name;
  const char* white_elo;
  const char* black_elo;
  char date[16];
  const char* event_name;
  const char* round_name;
  const char* site_name;
  unsigned int nag_count;
  unsigned int comment_count;
  unsigned int variation_count;
  bool deleted_flag;
  char flags[16];
  char eco[6];
  const char* end_material;
  bool start_flag;
  char event_date[16];
  unsigned int year;
  unsigned char rating;
  const char* first_moves;
  unsigned int index;
  unsigned int ply;
};

/*
class ScidDriver
{
  ~ScidDriver();

  virtual void onListGetEntry(ScidDatabaseEntry entry);
};
*/


/*
namespace scid {
  extern void print_hello();
};
*/

class Scid
{
public:
  Scid();
  ~Scid();
  int openDatabase(const char* filename);
  void listGames(int baseHandle, const char* ordering, const char* filterId, void(*onListGetEntry)(ScidDatabaseEntry), unsigned int start=0, unsigned int count=0);
};


#endif
