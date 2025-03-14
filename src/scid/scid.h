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
    scid_error(const std::string& msg, int code=0) throw() :
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

    enum move_egdes {
        AT_START,
        AT_END,
        AT_VAR_START,
        AT_VAR_END,
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

    void game_moves(int db_handle, unsigned int gnum, std::vector<std::string> &dest);

    std::string game_pgn(int db_handle, unsigned int gnum=0);

    //
    // Return the list of legal moves in SAN notation
    //
    void pos_moves(int db_handle, std::vector<std::string> &dest);

    bool pos_canMove(int db_handle, unsigned int sq1, unsigned int sq2, unsigned int promo);

    bool move_add(int db_handle, unsigned int sq1, unsigned int sq2, unsigned int promo);

    // Forward to a move in the loaded game
    void move_to(int db_handle, unsigned int move);

    // Check if move is at start or at end of the game or of a variation
    bool move_edge(int db_handle, move_egdes edge);

    int move_back(int db_handle, int count = 1);

    int move_forward(int db_handle, int count = 1);

    // Is the current Move is equal to the given parameters ?
    bool move_isEqual(int db_handle, unsigned int from, unsigned int to, unsigned int promo);

    // Ad a variation for the current move and moves into the variation
    void move_addVariation(int db_handle);

    // Exit the variation for the current move
    void move_exitVariation(int db_handle);

    // Get the position of the current move
    int move_getPosition(int db_handle);

    std::string pos_fen(int db_handle);
}

#endif /* SCID_H */
