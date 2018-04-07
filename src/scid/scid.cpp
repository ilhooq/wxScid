/*
 * scid.cpp
 *
 */

#include <vector>
#include <map>

#include "scid.h"
#include "scidbase.h"
#include "dbasepool.h"

// static scidBaseT* db;

namespace scid {

    void init()
    {
        DBasePool::init();
    }

    void close()
    {
        DBasePool::closeAll();
    }

    int base_getClipBaseHandle()
    {
        return DBasePool::getClipBase();
    }

    int base_open(std::string file)
    {
        ICodecDatabase::Codec codec = ICodecDatabase::Codec::MEMORY;
        fileModeT fMode = FMODE_Both;

        std::string extension = file.substr(file.find_last_of(".") + 1);

        if (extension == "si4") {
            codec = ICodecDatabase::Codec::SCID4;
            // Remove the extension because scid database contains 3 files (*.si4, *.sg4, *.sn4)
            file = file.substr(0, file.find_last_of("."));
        } else if (extension == "pgn") {
            codec = ICodecDatabase::Codec::PGN;
        } else {
            throw scid_error("Unrecognized file format.", ERROR_CodecUnsupFeat);
        }

        if (DBasePool::find(file.c_str()) != 0) {
            throw scid_error("File already in use.", ERROR_FileInUse);
        }

        scidBaseT* db = DBasePool::getFreeSlot();

        if (db == 0) {
            throw scid_error("The database opening limit is reached", ERROR_Full);
        }

        Progress progress;

        errorT err = db->Open(codec, fMode, file.c_str(), progress);

        if (err != OK && err != ERROR_NameDataLoss && fMode == FMODE_Both) {
            err = db->Open(ICodecDatabase::Codec::SCID4, FMODE_ReadOnly, file.c_str(), progress);
        }

        progress.report(1, 1);

        if (err != OK && err != ERROR_NameDataLoss) {
            throw scid_error("Unable to open database", err);
        }

        return DBasePool::switchCurrent(db);
    }

    std::string base_filename(int db_handle)
    {
        return DBasePool::getBase(db_handle)->getFileName();
    }

    unsigned int base_numgames(int db_handle)
    {
        return DBasePool::getBase(db_handle)->numGames();
    }

    void base_gameslist(int db_handle, const char* ordering, const char* filterId, unsigned int start, unsigned int count, std::map<unsigned int, game_entry>* map)
    {
        scidBaseT* db = DBasePool::getBase(db_handle);

        if (count == 0) {
            // Get all games
            count = db->numGames();
        }

        const HFilter filter = db->getFilter(filterId);

        if (filter == NULL) {
            throw scid_error("Filter bad arguments", ERROR_BadArg);
        }

        gamenumT* idxList = new gamenumT[count];

        count = db->listGames(ordering, (size_t) start, (size_t) count, filter, idxList);

        // The name base file in memory.
        const NameBase* nb = db->getNameBase();

        for (uint i = 0, item = start; i < count; ++i, item++) {

            uint idx = idxList[i];
            ASSERT(filter->get(idx) != 0);
            game_entry entry;

            uint ply = filter->get(idx) - 1;

            const IndexEntry* ie = db->getIndexEntry(idx);

            entry.result = RESULT_STR[ie->GetResult()];
            entry.movesNumber = (ie->GetNumHalfMoves() + 1) / 2;
            entry.whiteName = ie->GetWhiteName(nb);
            entry.blackName = ie->GetBlackName(nb);

            std::string eloStr;
            eloT welo = ie->GetWhiteElo();

            if (welo != 0) {
                eloStr = to_string(welo);
            } else {
                welo = ie->GetWhiteElo(nb);
                eloStr = to_string(welo);
                if (welo != 0) {
                    eloStr.insert(eloStr.begin(), '(');
                    eloStr.insert(eloStr.end(), ')');
                }
            }

            entry.whiteElo = eloStr;

            eloT belo = ie->GetBlackElo();

            if (belo != 0) {
                eloStr = to_string(belo);
            } else {
                belo = ie->GetBlackElo(nb);
                eloStr = to_string(belo);
                if (belo != 0) {
                    eloStr.insert(eloStr.begin(), '(');
                    eloStr.insert(eloStr.end(), ')');
                }
            }

            entry.blackElo = eloStr;

            char date[16];
            date_DecodeToString(ie->GetDate(), date);
            entry.date = date;
            entry.eventName = ie->GetEventName(nb);
            entry.roundName = ie->GetRoundName(nb);
            entry.siteName = ie->GetSiteName(nb);
            entry.nagCount = ie->GetNagCount();
            entry.commentCount = ie->GetCommentCount();
            entry.variationCount = ie->GetVariationCount();
            entry.deletedFlag = ie->GetDeleteFlag();

            char flags[16];
            ie->GetFlagStr(flags, "WBMENPTKQ!?U123456");
            entry.flags = flags;

            char eco[6];
            eco_ToExtendedString(ie->GetEcoCode(), eco);
            entry.eco = eco;

            entry.endMaterial = matsig_makeString(ie->GetFinalMatSig());
            entry.startFlag = ie->GetStartFlag();

            char event_date[16];
            date_DecodeToString(ie->GetEventDate(), event_date);
            entry.eventDate = event_date;

            entry.year = ie->GetYear();
            entry.rating = ie->GetRating(nb);
            FastGame game = db->getGame(ie);
            entry.firstMoves = game.getMoveSAN(ply, 10);
            entry.index = idx;
            entry.ply = ply;

            (*map)[item] = entry;
        }

        delete[] idxList;
    }

    void base_getGame(int db_handle, std::vector<game_posInfos> &dest)
    {
        scidBaseT* db = DBasePool::getBase(db_handle);

        db->game->MoveToStart();

        do {
            if (db->game->AtVarStart() && !db->game->AtStart())
                continue;

            dest.emplace_back();
            game_posInfos &pos = dest.back();
            // game_posInfos pos;
            pos.RAVdepth = db->game->GetVarLevel();
            pos.RAVnum = db->game->GetVarNumber();
            char strBuf[256];
            db->game->currentPos()->PrintFEN(strBuf, FEN_ALL_FIELDS);
            pos.FEN = strBuf;

            // for (byte* nag = db->game->GetNags(); *nag; nag++) {
            //    pos.NAGs.push_back(*nag);
            // }

            pos.comment = db->game->GetMoveComment();
            db->game->GetPrevSAN(strBuf);
            pos.lastMoveSAN = strBuf;
            // dest.push_back(pos);

        } while (db->game->MoveForwardInPGN() == OK);
    }

    void base_getGame(int db_handle, unsigned int entry_index, std::vector<game_posInfos> &dest)
    {
        scidBaseT* db = DBasePool::getBase(db_handle);
        const IndexEntry* ie = db->getIndexEntry(entry_index);

        std::vector<scidBaseT::GamePos> firstDest;
        db->getGame(*ie, firstDest);

        std::vector<scidBaseT::GamePos>::iterator it;

        for (it = firstDest.begin(); it != firstDest.end(); it++) {
            scidBaseT::GamePos ScidPos = *it;
            game_posInfos pos;
            pos.RAVdepth = ScidPos.RAVdepth;
            pos.RAVnum = ScidPos.RAVnum;
            pos.FEN = ScidPos.FEN;
            pos.comment = ScidPos.comment;
            pos.lastMoveSAN = ScidPos.lastMoveSAN;

            for (size_t iNag = 0, nNag = ScidPos.NAGs.size(); iNag < nNag; iNag++) {
                char temp[20];
                game_printNag(ScidPos.NAGs[iNag], temp, true, PGN_FORMAT_Plain);
                if (!pos.NAGs.empty())
                    pos.NAGs + " ";
                pos.NAGs + temp;
            }

            dest.push_back(pos);
        }
    }

    void game_load(int db_handle, unsigned int gnum)
    {
        scidBaseT* db = DBasePool::getBase(db_handle);

        if (!db->inUse) {
            throw scid_error("Database was not openned.", ERROR_FileNotOpen);
        }

        db->gameAlterations.clear();

        // Check the game number is valid
        if (gnum < 0  ||  gnum > db->numGames()) {
            throw scid_error("Invalid game number.", ERROR_BadArg);
        }

        const char * corruptMsg = "Sorry, this game appears to be corrupt.";

        const IndexEntry* ie = db->getIndexEntry(gnum);

        if (db->getGame(ie, db->bbuf) != OK) {
            throw scid_error(corruptMsg, ERROR_Corrupt);
        }

        if (db->game->Decode(db->bbuf, GAME_DECODE_ALL) != OK) {
            throw scid_error(corruptMsg, ERROR_Corrupt);
        }

        if (db->dbFilter->Get(gnum) > 0) {
            db->game->MoveToPly(db->dbFilter->Get(gnum) - 1);
        } else {
            db->game->MoveToPly(0);
        }

        db->game->LoadStandardTags(ie, db->getNameBase());
        db->gameNumber = gnum;
        db->gameAltered = false;
    }

    void pos_moves(int db_handle, std::vector<std::string> &dest)
    {
        scidBaseT* db = DBasePool::getBase(db_handle);
        Position * p = db->game->GetCurrentPos();
        sanListT sanList;

        p->CalcSANStrings(&sanList, SAN_NO_CHECKTEST);

        for (uint i=0; i < sanList.num; i++) {
            dest.push_back(sanList.list[i]);
        }
    }

    bool move_add(int db_handle, unsigned int sq1, unsigned int sq2, unsigned int promo)
    {
        scidBaseT* db = DBasePool::getBase(db_handle);

        if (promo == 0) { promo = EMPTY; }

        char s[8];
        s[0] = square_FyleChar(sq1);
        s[1] = square_RankChar(sq1);
        s[2] = square_FyleChar(sq2);
        s[3] = square_RankChar(sq2);

        if (promo == EMPTY) {
            s[4] = 0;
        } else {
            s[4] = piece_Char(promo);
            s[5] = 0;
        }

        simpleMoveT sm;

        Position * pos = db->game->GetCurrentPos();

        errorT err = pos->ReadCoordMove(&sm, s, s[4] == 0 ? 4 : 5, true);

        if (err == OK) {
            err = db->game->AddMove(&sm);

            if (err == OK) {
                db->gameAltered = true;
                return true;
            }
        }

        return false;
    }

    void move_to(int db_handle, unsigned int move)
    {
        scidBaseT* db = DBasePool::getBase(db_handle);
        db->game->MoveToStart();
        int i = 0;

        do {

            if (db->game->AtVarStart() && !db->game->AtStart())
                continue;

            if (i == move) break;

            i++;

        } while (db->game->MoveForwardInPGN() == OK);
    }

    std::string pos_fen(int db_handle)
    {
        scidBaseT* db = DBasePool::getBase(db_handle);
        char boardStr[200];
        db->game->GetCurrentPos()->PrintFEN(boardStr, FEN_ALL_FIELDS);
        return (std::string) boardStr;
    }
}


