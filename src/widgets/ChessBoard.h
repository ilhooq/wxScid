/*
 * ChessBoard.h
 *
 */

#ifndef WIDGETS_CHESSBOARD_H_
#define WIDGETS_CHESSBOARD_H_

#include <wx/window.h>

#if defined(__WXMSW__)
#include <wx/generic/dragimgg.h>
#define wxDragImage wxGenericDragImage
#else
#include <wx/dragimag.h>
#endif

#include <wx/event.h>

wxDECLARE_EVENT(EVT_CAN_DROP_PIECE, wxCommandEvent);

class ChessBoardPiece
{
public:
    int x;
    int y;
    int width;
    int height;
    short pieceType;
    wxImage * image;
    bool show;

    ChessBoardPiece(short type, wxImage * img) : x(0), y(0), width(0), height(0), show(true)
    {
        pieceType = type;
        image = img;
    }

    bool HitTest(wxPoint pt)
    {
        return GetRect().Contains(pt);
    }

    void SetRect(wxRect rect)
    {
        x = rect.x;
        y = rect.y;
        width = rect.width;
        height = rect.height;
    }

    wxRect GetRect()
    {
        return wxRect(x, y, width, height);
    }

    wxBitmap GetBitmap()
    {
        wxASSERT(width > 0 && height > 0);
        return wxBitmap(image->Scale(width, height));
    }
};

struct PieceMove
{
    int from;
    int to;
    bool canDrop;
};

class ChessBoardSquare
{
public:
    wxRect rect;
    wxColor *color;
    ChessBoardPiece *piece;

    ChessBoardSquare() : rect(0, 0, 0, 0)
    {
        color = NULL;
        piece = NULL;
    }

    void SetPosition(int x, int y)
    {
        rect.SetPosition(wxPoint(x, y));
    }

    void SetWidth(int width)
    {
        rect.SetSize(wxSize(width, width));
    }

    bool HitTest(wxPoint pt)
    {
        return rect.Contains(pt);
    }

    void AddPiece(ChessBoardPiece* p)
    {
        piece = p;
    }

    void RemovePiece()
    {
        if (piece) delete piece;
        piece = NULL;
    }
};

class ChessBoard: public wxWindow
{
public:
    enum Pieces
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

    enum Sides
    {
        WHITE,
        BLACK
    };

    enum Squares
    {
        a1, b1, c1, d1, e1, f1, g1, h1,
        a2, b2, c2, d2, e2, f2, g2, h2,
        a3, b3, c3, d3, e3, f3, g3, h3,
        a4, b4, c4, d4, e4, f4, g4, h4,
        a5, b5, c5, d5, e5, f5, g5, h5,
        a6, b6, c6, d6, e6, f6, g6, h6,
        a7, b7, c7, d7, e7, f7, g7, h7,
        a8, b8, c8, d8, e8, f8, g8, h8
    };

    ChessBoard(wxWindow* parent,
               const wxWindowID id,
               const wxString themeDir,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0);

    void AddPiece(Pieces pieceType, Squares square);
    void RemovePiece(Squares square);
    void Flip();
    void Clear();
    bool LoadPositionFromFen(wxString FEN);

private:
    wxImage theme;
    wxImage themeImages[12];
    wxColor backgroundColor;
    wxColor wSquareColor;
    wxColor bSquareColor;
    wxSize size;
    int boardSize;
    int squareSize;
    ChessBoardSquare squares[64];
    bool flipped;
    ChessBoardPiece* dragPiece;
    wxDragImage * dragImage;
    wxPoint dragStartPos;
    // Side to move (Black or white)
    short side;

    void OnPaint(wxPaintEvent & evt);
    void OnLeftDown(wxMouseEvent & evt);
    void OnLeftUp(wxMouseEvent & evt);
    void OnMotion(wxMouseEvent & evt);
    int GetRankIndex(int squareIndex);
    int GetFileIndex(int squareIndex);
    ChessBoardPiece* FindPiece(wxPoint pt);
    int FindSquareIndex(wxPoint pt);
    int FindSquareIndex(ChessBoardPiece* p);
    void UpdateCoords();

DECLARE_EVENT_TABLE()
};

#endif /* WIDGETS_CHESSBOARD_H_ */
