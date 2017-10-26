/*
* Copyright (C) 2013-2015  Fulvio Benini

* This file is part of Scid (Shane's Chess Information Database).
*
* Scid is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation.
*
* Scid is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Scid.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef FASTGAME_H
#define FASTGAME_H

#include "common.h"
#include "fullmove.h"
#include "movegen.h"
#include "position.h"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>

class FastBoard {
	uint8_t board_[64];
	uint8_t nPieces_[2][8];
	struct P_LIST {
		squareT sq;
		pieceT piece;
	} list[2][16];

	enum { KING_IDX_ = 0, EMPTY_SQ_ = 0xFF };

public:
	FastBoard() {}
	FastBoard(Position& pos) { Init(pos); }

	void Init() {
		static Position StdStartPos(Position::getStdStart());
		static FastBoard StdStart(StdStartPos);
		*this = StdStart;
	}

	void Init(Position& pos) {
		std::fill_n(board_, 64, EMPTY_SQ_);
		std::fill_n(nPieces_[WHITE], 8, 0);
		std::fill_n(nPieces_[BLACK], 8, 0);

		nPieces_[WHITE][0] = pos.GetCount(WHITE);
		for (uint8_t i = 0, n = nPieces_[WHITE][0]; i < n; ++i) {
			squareT sq = pos.GetList(WHITE)[i];
			pieceT piece = piece_Type(pos.GetBoard()[sq]);
			ASSERT(piece != KING || i == KING_IDX_);
			list[WHITE][i].sq = sq;
			list[WHITE][i].piece = piece;
			board_[sq] = i;
			nPieces_[WHITE][piece] += 1;
		}

		nPieces_[BLACK][0] = pos.GetCount(BLACK);
		for (uint8_t i = 0, n = nPieces_[BLACK][0]; i < n; ++i) {
			squareT sq = pos.GetList(BLACK)[i];
			pieceT piece = piece_Type(pos.GetBoard()[sq]);
			ASSERT(piece != KING || i == KING_IDX_);
			list[BLACK][i].sq = sq;
			list[BLACK][i].piece = piece;
			board_[sq] = i;
			nPieces_[BLACK][piece] += 1;
		}
	}

	bool isEqual(const pieceT* board, const uint8_t* nPiecesW, const uint8_t* nPiecesB) const {
		if (!std::equal(nPiecesW, nPiecesW + 8, nPieces_[WHITE]))
			return false;
		if (!std::equal(nPiecesB, nPiecesB + 8, nPieces_[BLACK]))
			return false;

		for (int i=0, n = nPieces_[WHITE][0]; i < n; i++) {
			const P_LIST* p = & list[WHITE][i];
			if (board[p->sq] != piece_Make(WHITE, p->piece)) return false;
		}
		for (int i=0, n = nPieces_[BLACK][0]; i < n; i++) {
			const P_LIST* p = & list[BLACK][i];
			if (board[p->sq] != piece_Make(BLACK, p->piece)) return false;
		}
		return true;
	}

	template <colorT color>
	squareT getSquare(uint8_t idx) const {
		return list[color][idx].sq;
	}

	template <colorT color>
	pieceT getPiece(uint8_t idx) const {
		return list[color][idx].piece;
	}

	template <colorT color>
	uint8_t getCount(pieceT p = 0) const {
		ASSERT(p < 8);
		return nPieces_[color][p];
	}

	template <colorT color>
	void castle(squareT king_to, squareT rook_from, squareT rook_to) {
		const uint8_t rook_idx = board_[rook_from];
		const squareT king_from = list[color][KING_IDX_].sq;
		list[color][rook_idx].sq = rook_to;
		list[color][KING_IDX_].sq = king_to;
		board_[rook_to] = rook_idx;
		board_[king_to] = KING_IDX_;
		board_[rook_from] = EMPTY_SQ_;
		board_[king_from] = EMPTY_SQ_;
	}

	template <colorT color>
	pieceT move(uint8_t idx, squareT to, pieceT promo) {
		if (promo != INVALID_PIECE) {
			list[color][idx].piece = promo;
			nPieces_[color][PAWN] -= 1;
			nPieces_[color][promo] += 1;
		}
		board_[list[color][idx].sq] = EMPTY_SQ_;
		list[color][idx].sq = to;
		return remove<1 - color>(to, idx);
	}

	template <colorT color>
	pieceT remove(squareT sq, uint8_t newIdx = EMPTY_SQ_) {
		const uint8_t oldIdx = board_[sq];
		board_[sq] = newIdx;
		if (oldIdx == EMPTY_SQ_)
			return INVALID_PIECE;

		pieceT res = list[color][oldIdx].piece;
		nPieces_[color][res] -= 1;
		nPieces_[color][0] -= 1;
		if (oldIdx != nPieces_[color][0]) {
			list[color][oldIdx] = list[color][nPieces_[color][0]];
			ASSERT(list[color][oldIdx].sq != sq);
			board_[list[color][oldIdx].sq] = oldIdx;
		}
		return res;
	}

	/**
	 * Given the actual board position, find if the last move needs to be made
	 * unambiguous and if it gives check (or <TODO> mate), and then sets the
	 * appropriate bits in @e lastmove.
	 * @param lastmove: the last move played.
	 */
	void fillSANInfo(FullMove& lastmove) {
		squareT lastFrom = lastmove.getFrom();
		squareT lastTo = lastmove.getTo();
		colorT lastCol = lastmove.getColor();
		pieceT lastPt = lastmove.getPiece();

		if (lastPt == PAWN) {
			if (lastmove.isPromo())
				lastPt = lastmove.getPromo();
		} else if (nPieces_[lastCol][lastPt] > 1) {
			int ambiguity = ambiguousMove(lastFrom, lastTo, lastCol, lastPt);
			if (ambiguity)
				lastmove.setAmbiguity(ambiguity != 5, ambiguity >= 5);
		}

		// Look for checks
		ASSERT(nPieces_[WHITE][0] >= 1 && nPieces_[BLACK][0] >= 1);

		squareT enemyKingSq = list[color_Flip(lastCol)][0].sq;
		const P_LIST* piecesEnd = list[lastCol] + nPieces_[lastCol][0];
		bool direct_check = lastPt != KING && movegen::attack<uint8_t>(
		                                          lastTo, enemyKingSq, lastCol,
		                                          lastPt, board_, EMPTY_SQ_);
		if (direct_check || // Look for a discovered check
		    find_attacker_slider(enemyKingSq, list[lastCol] + 1, piecesEnd) !=
		        piecesEnd) {
			lastmove.setCheck();

			// TODO: Find if it's mate:
			// - it's not mate if the king can move to a safe square
			// - it's mate if it's double check or the attacker cannot be
			//   captured or blocked.
		}
	}

private:
	int ambiguousMove(squareT lastFrom, squareT lastTo, colorT lastCol,
	                  pieceT lastPt) {
		int ambiguity = 0;

		squareT kingSq = list[lastCol][0].sq;
		colorT enemyCol = color_Flip(lastCol);
		for (size_t i = 1, n = nPieces_[lastCol][0]; i < n; i++) {
			if (list[lastCol][i].piece != lastPt)
				continue; // Skip: different type

			squareT sq = list[lastCol][i].sq;
			if (sq == lastTo)
				continue; // Skip: this is the analyzed piece

			board_[lastFrom] = board_[sq];
			board_[sq] = EMPTY_SQ_;

			bool pseudoLegal = movegen::pseudo<uint8_t>(
			    sq, lastTo, lastCol, lastPt, board_, EMPTY_SQ_);

			std::pair<pieceT, squareT> pin;
			if (pseudoLegal)
				pin = movegen::opens_ray<uint8_t>(sq, lastTo, kingSq, board_,
				                                  EMPTY_SQ_);
			board_[sq] = board_[lastFrom];
			board_[lastFrom] = EMPTY_SQ_;

			if (!pseudoLegal)
				continue; // Skip: illegal move

			if (pin.first != INVALID_PIECE) {
				uint8_t idx = board_[pin.second];
				if (idx != EMPTY_SQ_ && idx < nPieces_[enemyCol][0] &&
				    list[enemyCol][idx].sq == pin.second) {
					pieceT pt = list[enemyCol][idx].piece;
					if (pt == QUEEN || pt == pin.first)
						continue; // Skip: pinned piece
				}
			}

			// Ambiguity:
			// 1 (0001) --> need from-file (preferred) or from-rank
			// 3 (0011) --> need from-file
			// 5 (0101) --> need from-rank
			// 7 (0111) --> need both from-file and from-rank
			ambiguity |= 1;
			if (square_Rank(lastFrom) == square_Rank(sq)) {
				ambiguity |= 2; // 0b0010
			} else if (square_Fyle(lastFrom) == square_Fyle(sq)) {
				ambiguity |= 4; // 0b0100
			}
		}

		return ambiguity;
	}

	const P_LIST* find_attacker_slider(squareT destSq, const P_LIST* pieces,
	                                   const P_LIST* piecesEnd) {
		for (; pieces != piecesEnd; ++pieces) {
			pieceT pt = (*pieces).piece;
			if (pt != QUEEN && pt != ROOK && pt != BISHOP)
				continue;

			if (movegen::attack_slider<uint8_t>((*pieces).sq, destSq, pt,
			                                    board_, EMPTY_SQ_)) {
				break;
			}
		}
		return pieces;
	}
};

class FastGame {
	FastBoard board_;
	const byte* v_it_;
	const byte* v_end_;
	colorT cToMove_;

public:
	static FastGame Create(const byte* v_begin, const byte* v_end) {
		const byte* v_it = v_begin;
		while (v_it < v_end) {
			byte b = *v_it++;
			if (b == 0) {
				if (v_it >= v_end) break; // Error
				byte haveFEN = *v_it++ & 1;
				if (haveFEN == 0) {
					return FastGame(v_it, v_end);
				} else {
					const char* FENstring = (char*) v_it;
					while (v_it < v_end) {
						if (*v_it++ == 0) return FastGame(FENstring, v_it, v_end);
					}
					break; // FEN error
				}
			} else if (b == 255) { // Skip special 3-byte binary encoding of EventDate
				v_it += 3;
			} else { // Skip tags
				enum { MAX_TAG_LEN = 240 };
				if (b <= MAX_TAG_LEN) v_it += b;
				if (v_it < v_end) v_it += *v_it +1;
			}
		}

		return FastGame(0,0); // Error default to StdStart and empty buffer
	}

	FullMove getMove(int ply_to_skip) {
		FullMove move;
		Dummy dummy;

		for (int ply=0; ply <= ply_to_skip; ply++, cToMove_ = 1 - cToMove_) {
			if (cToMove_ == WHITE) {
				if (! DecodeNextMove <WHITE>(move, dummy)) break;
			} else {
				if (! DecodeNextMove <BLACK>(move, dummy)) break;
			}

			if (ply == ply_to_skip) {
				board_.fillSANInfo(move);
				cToMove_ = 1 - cToMove_;
				return move;
			}
		}
		return FullMove();
	}

	std::string getMoveSAN(int ply_to_skip, int count) {
		std::stringstream res;
		Dummy dummy;
		for (int ply=0; ply < ply_to_skip + count; ply++, cToMove_ = 1 - cToMove_) {
			FullMove move;
			if (cToMove_ == WHITE) {
				if (! DecodeNextMove <WHITE>(move, dummy)) break;
				if (ply < ply_to_skip) continue;
				if (ply > ply_to_skip) res << "  ";
				res << (1 + ply/2) << ".";
			} else {
				if (! DecodeNextMove <BLACK>(move, dummy)) break;
				if (ply < ply_to_skip) continue;
				if (ply == ply_to_skip) res << (1 + ply/2) << "...";
				else res << " ";
			}
			board_.fillSANInfo(move);
			res << move.getSAN();
		}
		return res.str();
	}

	template <colorT toMove>
	int search(const byte* board, const uint8_t (&nPieces) [2][8]) {
		int ply = 1;
		Dummy dummy;
		MinPieces minP(nPieces);

		if (cToMove_ != toMove) {
			if (! DecodeNextMove<1 - toMove>(dummy, minP)) return 0;
			ply += 1;
		}
		for (;;) {
			if (board_.isEqual(board, nPieces[WHITE], nPieces[BLACK])) return ply;
			if (! DecodeNextMove<toMove>(dummy, minP)) return 0;
			if (! DecodeNextMove<1 - toMove>(dummy, minP)) return 0;
			ply += 2;
		}
		return 0;
	}

private:
	FastGame(const byte* v_it, const byte* v_end)
	: v_it_ (v_it), v_end_(v_end), cToMove_(WHITE) {
		board_.Init();
	}

	FastGame(const char* FEN, const byte* v_it, const byte* v_end)
	: v_it_ (v_it), v_end_(v_end) {
		Position StartPos;
		if (FEN == 0 || StartPos.ReadFromFEN(FEN) != OK) StartPos.StdStart();
		board_.Init(StartPos);
		cToMove_ = StartPos.GetToMove();
	}

	template <colorT toMove, typename P1, typename P2>
	inline bool DecodeNextMove(P1& p1, const P2& p2) {
		enum { ENCODE_NAG = 11, ENCODE_COMMENT, ENCODE_START_MARKER, ENCODE_END_MARKER, ENCODE_END_GAME };
		enum { ENCODE_FIRST = 11, ENCODE_LAST = 15 };

		while (v_it_ < v_end_) {
			byte b = *v_it_++;
			if (b < ENCODE_FIRST || b > ENCODE_LAST) return doPly<toMove>(b, p1, p2);
			if (b == ENCODE_END_GAME || b == ENCODE_END_MARKER) return false;
			if (b == ENCODE_NAG) {v_it_++; continue; }
			if (b == ENCODE_START_MARKER) {
				uint nestCount = 1;
				do {
					if (v_it_ >= v_end_) return false;
					switch (*v_it_++) {
					case ENCODE_NAG: v_it_++; break;
					case ENCODE_START_MARKER: nestCount++; break;
					case ENCODE_END_MARKER: nestCount--; break;
					case ENCODE_END_GAME: return false;
					}
				} while (nestCount > 0);
			}
		}
		return false;
	}

	template <colorT toMove, typename P1, typename P2>
	inline bool doPly(byte v, P1& lastMove, const P2& minPieces) {
		byte idx_piece_moving = v >> 4;
		byte move = v & 0x0F;
		pieceT moving_piece = board_.getPiece<toMove>(idx_piece_moving);
		squareT from = board_.getSquare<toMove>(idx_piece_moving);
		squareT to;
		pieceT promo = INVALID_PIECE;
		bool enPassant = false;
		switch (moving_piece) {
			case PAWN: 	 to = decodePawn<toMove>(from, move, promo, enPassant); break;
			case BISHOP: to = decodeBishop(from, move); break;
			case KNIGHT: to = decodeKnight(from, move); break;
			case ROOK:   to = decodeRook(from, move); break;
			case QUEEN:
				if (move != square_Fyle(from)) to = decodeRook(from, move);
				else if (v_it_ < v_end_) to = decodeQueen2byte(*v_it_++);
				else return false;
				break;
			default: // Default to KING
				if (move == 0) { // NULL MOVE
					lastMove.reset(toMove, KING, 0, 0);
					return true;
				}
				if (move > 8) { // CASTLE
					const squareT black = (toMove == WHITE) ? 0 : 56;
					squareT king_to, rook_from, rook_to;
					if (move == 10) { // King Side
						king_to = black + G1;
						rook_from = black + H1;
						rook_to = black + F1;
					} else { // Queen Side
						king_to = black + C1;
						rook_from = black + A1;
						rook_to = black + D1;
					}
					const byte king_idx = 0;
					lastMove.resetCastle(toMove, board_.getSquare<toMove>(king_idx), rook_from);
					board_.castle<toMove>(king_to, rook_from, rook_to);
					// ClearCastlingRights;
					return true;
				}
				to = decodeKing(from, move);
		}

		lastMove.reset(toMove, moving_piece, from, to, promo);
		const colorT enemy = 1 - toMove;
		pieceT captured = board_.move<toMove> (idx_piece_moving, to, promo);
		if (captured == INVALID_PIECE) {
			if (!enPassant) return true;
			captured = PAWN;
			squareT sq = (toMove == WHITE) ? to - 8 : to + 8;
			board_.remove<enemy>(0x3F & sq);
		}
		lastMove.setCapture(captured, enPassant);

		return minPieces(enemy, captured, board_.getCount<enemy>(),
			board_.getCount<enemy>(PAWN) + board_.getCount<enemy>(captured));
	}

	/**
	 * decode*() - decode a move from Scid format
	 * @from: start square of the moving piece
	 * @val: index of the target square
	 *
	 * Excluding queens, the other chess pieces cannot reach more than 16 target
	 * squares from any given position. This allow to store the target square of
	 * a move into 4 bits, as an index of all the possible target squares.
	 * Return:
	 * - the target square
	 * Error handling:
	 * - Debug code will check if the decoded value is a valid [0-63] square.
	 * - Release code will force the returned valid to be a valid [0-63] square
	 *   but, for performance reasons, do not report invalid encoded moves.
	 */
	static inline squareT square_forceValid(int sq) {
		ASSERT(sq >= 0 && sq <= 63);
		return 0x3F & static_cast<squareT>(sq);
	}
	static inline squareT decodeKing(squareT from, byte val) {
		ASSERT(val <= 8);
		static const int sqdiff[] = {0, -9, -8, -7, -1, 1, 7, 8, 9};
		int to = static_cast<int>(from) + sqdiff[val];
		return square_forceValid(to);
	}
	static inline squareT decodeQueen2byte(byte val) {
		int to = static_cast<int>(val) - 64;
		return square_forceValid(to);
	}
	static inline squareT decodeBishop(squareT from, byte val) {
		int fylediff = static_cast<int>(square_Fyle(val)) -
		               static_cast<int>(square_Fyle(from));
		int to = (val >= 8) ? static_cast<int>(from) - 7 * fylediff
		                    : static_cast<int>(from) + 9 * fylediff;
		return square_forceValid(to);
	}
	static inline squareT decodeKnight(squareT from, byte val) {
		ASSERT(val <= 16);
		static const int sqdiff[] = {0, -17, -15, -10, -6, 6, 10, 15, 17, 0, 0, 0, 0, 0, 0, 0};
		int to = static_cast<int>(from) + sqdiff[val];
		return square_forceValid(to);
	}
	static inline squareT decodeRook(squareT from, byte val) {
		ASSERT(val <= 16);
		if (val >= 8)
			return square_Make(square_Fyle(from), (val - 8));
		else
			return square_Make(val, square_Rank(from));
	}
	template <colorT color>
	static inline squareT decodePawn(squareT from, byte val, pieceT& promo,
	                                 bool& enPassant) {
		ASSERT(val <= 16);
		static const int sqdiff [] = { 7,8,9, 7,8,9, 7,8,9, 7,8,9, 7,8,9, 16 };
		static const pieceT promoPieceFromVal [] = {
			0,0,0,QUEEN,QUEEN,QUEEN, ROOK,ROOK,ROOK, BISHOP,BISHOP,BISHOP,KNIGHT,KNIGHT,KNIGHT,0
		};
		promo = promoPieceFromVal[val];
		enPassant = (val == 0 || val == 2);
		int to = (color == WHITE) ? static_cast<int>(from) + sqdiff[val]
		                          : static_cast<int>(from) - sqdiff[val];
		return square_forceValid(to);
	}

	struct Dummy {
		void reset(colorT, pieceT, squareT, squareT, pieceT = 0) {}
		void resetCastle(colorT, squareT, squareT) {}
		void setCapture(pieceT, bool) {}
		bool operator()(colorT, pieceT, uint8_t, uint8_t) const { return true; }
	};

	class MinPieces{
		const uint8_t (&m_)[2][8];
	public:
		MinPieces(const uint8_t (&m)[2][8]) : m_(m) {}
		bool operator()(colorT col, pieceT p, uint8_t tot, uint8_t p_count) const {
			return (tot >= m_[col][0] && p_count >= (m_[col][PAWN] + m_[col][p]));
		}
	};

};


#endif
