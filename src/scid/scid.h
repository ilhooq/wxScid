/*
 * scid.h
 *
 */

#ifndef SCID_H
#define SCID_H

#include <string>
#include <map>

class scid_error: std::exception
{
public:
    scid_error(const std::string& msg, int code) throw() :
    m_msg(msg),
    m_code(code)
    {
    }

    virtual const char* what() const throw()
    {
        return m_msg.c_str();
    }

    int getCode() throw ()
    {
        return m_code;
    }

    virtual ~scid_error() throw()
    {
    }

private:
    int m_code;  // Code number
    std::string m_msg;  // Error description
};

namespace scid {

    enum piece_type {
        INVALID_PIECE = 0,
        KING,
        QUEEN,
        ROOK,
        BISHOP,
        KNIGHT,
        PAWN
    };

    struct game_entry
    {
        std::string result;
        short movesNumber;
        std::string whiteName;
        std::string blackName;
        std::string whiteElo;
        std::string blackElo;
        std::string date;
        std::string eventName;
        std::string roundName;
        std::string siteName;
        int nagCount;
        int commentCount;
        int variationCount;
        bool deletedFlag;
        std::string flags;
        std::string eco;
        std::string endMaterial;
        bool startFlag;
        std::string eventDate;
        int year;
        char rating;
        std::string firstMoves;
        unsigned int index;
        int ply;
    };

    struct game_posInfos
    {
        int RAVdepth;
        int RAVnum;
        std::string FEN;
        std::string NAGs;
        std::string comment;
        std::string lastMoveSAN;
    };

    void init();

    void close();

    int base_getClipBaseHandle();

    int base_open(std::string file);

    std::string base_filename(int db_handle);

    unsigned int base_numgames(int db_handle);

    void base_gameslist(int baseHandle, const char* ordering, const char* filterId, unsigned int start, unsigned int count, std::map<unsigned int, game_entry>* map);

    // Get the loaded game
    void base_getGame(int db_handle, std::vector<game_posInfos> &dest);

    void base_getGame(int db_handle, unsigned int entry_index, std::vector<game_posInfos> &dest);

    void game_load(int db_handle, unsigned int gnum);

    //
    // Return the list of legal moves in SAN notation
    //
    void pos_moves(int db_handle, std::vector<std::string> &dest);

    bool move_add(int db_handle, unsigned int sq1, unsigned int sq2, unsigned int promo);
}

#endif /* SCID_H */
