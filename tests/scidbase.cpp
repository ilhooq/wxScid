#include <iostream>
#include <string>
#include "../src/scid/scid.h"

using namespace std;

class TestListEventHandler : public ScidListEventHandler
{
public:
  void onListGetEntry(ScidDatabaseEntry& entry);
};

void TestListEventHandler::onListGetEntry(ScidDatabaseEntry& entry)
{
  cout << "White player : "    << (string) entry.white_name
       << " - Black player : " << (string) entry.black_name
       << " - Date : "         << (string) entry.date
       << " - index : "        << to_string(entry.index)
       << " - ply : "          << to_string(entry.ply)
       << "\n" ;
}


int main()
{
  try {
    Scid* scid = new Scid();
    int dbHandle = scid->openDatabase("res_database.si4");
    // For the sort ordering criteria (see sortcache.h)
    TestListEventHandler eventHandler;
    scid->listGames(dbHandle, "d+", "dbfilter", &eventHandler, 0, 20);
  }
  catch(ScidError &error) {
     cout << "Error : " << error.what() << " - code : " << to_string(error.getCode()) << "\n";
  }

    // cout << "Hello, World!";
    return 0;
}
