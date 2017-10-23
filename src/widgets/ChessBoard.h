/*
 * ChessBoard.h
 *
 */

#ifndef WIDGETS_CHESSBOARD_H_
#define WIDGETS_CHESSBOARD_H_

#include "wx/window.h"

#if defined(__WXMSW__)
#include "wx/generic/dragimgg.h"
#define wxDragImage wxGenericDragImage
#else
#include "wx/dragimag.h"
#endif

#include "../Squares.h"

class ChessBoardPiece
{
public:
  int x;
  int y;
  int width;
  int height;
  char pieceType;
  wxImage * image;
  bool show;

  ChessBoardPiece(char type, wxImage * img);
  bool hitTest(wxPoint pt);
  void setRect(wxRect rect);
  wxRect getRect();
  wxBitmap getBitmap();
};

class ChessBoardSquare
{
public:
  wxRect rect;wxColor* color;
  ChessBoardPiece * piece;
  ChessBoardSquare();
  void setPosition(int x, int y);
  void setWidth(int width);
  bool hitTest(wxPoint pt);
  void addPiece(ChessBoardPiece* piece);
  void removePiece();
};

class ChessBoard: public wxWindow
{

public:
  enum
  {
    wRook,
    wKnight,
    wBishop,
    wQueen,
    wKing,
    wPawn,
    bRook,
    bKnight,
    bBishop,
    bQueen,
    bKing,
    bPawn
  };

  ChessBoard(wxWindow* parent, wxString themeDir);

  void onPaint(wxPaintEvent & evt);

  void onSize(wxSizeEvent & evt);
  void onLeftDown(wxMouseEvent & evt);
  void onLeftUp(wxMouseEvent & evt);
  void onMotion(wxMouseEvent & evt);

  void addPiece(char pieceType, Square square);
  void removePiece(Square square);
  void flip();

DECLARE_EVENT_TABLE()

protected:
  wxImage theme;
  wxImage themeImages[11];wxColor backgroundColor;wxColor wSquareColor;wxColor bSquareColor;
  wxSize size;
  int boardSize;
  int squareSize;
  ChessBoardSquare squares[64];
  bool flipped;
  ChessBoardPiece* dragPiece;wxDragImage * dragImage;
  wxPoint dragStartPos;

  char getRankIndex(char squareIndex);
  char getFileIndex(char squareIndex);
  ChessBoardPiece* findPiece(wxPoint pt);
  char findSquareIndex(wxPoint pt);
  char findSquareIndex(ChessBoardPiece* p);
  void updateCoords();
};

#endif /* WIDGETS_CHESSBOARD_H_ */
