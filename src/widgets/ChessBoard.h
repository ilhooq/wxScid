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

    ChessBoardPiece(char type, wxImage * img) : x(0), y(0), width(0), height(0), show(true)
    {
        pieceType = type;
        image = img;
    }

    bool hitTest(wxPoint pt)
    {
        return getRect().Contains(pt);
    }

    void setRect(wxRect rect)
    {
        x = rect.x;
        y = rect.y;
        width = rect.width;
        height = rect.height;
    }

    wxRect getRect()
    {
        return wxRect(x, y, width, height);
    }

    wxBitmap getBitmap()
    {
        return wxBitmap(image->Scale(width, height));
    }
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

    void setPosition(int x, int y)
    {
        rect.SetPosition(wxPoint(x, y));
    }

    void setWidth(int width)
    {
        rect.SetSize(wxSize(width, width));
    }

    bool hitTest(wxPoint pt)
    {
        return rect.Contains(pt);
    }

    void addPiece(ChessBoardPiece* p)
    {
        piece = p;
    }

    void removePiece()
    {
        delete piece;
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

    ChessBoard(wxWindow* parent, wxString themeDir);

    void onPaint(wxPaintEvent & evt);

    void onSize(wxSizeEvent & evt);
    void onLeftDown(wxMouseEvent & evt);
    void onLeftUp(wxMouseEvent & evt);
    void onMotion(wxMouseEvent & evt);

    void addPiece(char pieceType, Squares square);
    void removePiece(Squares square);
    void flip();

protected:
    wxImage theme;
    wxImage themeImages[11];
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

    char getRankIndex(char squareIndex);
    char getFileIndex(char squareIndex);
    ChessBoardPiece* findPiece(wxPoint pt);
    char findSquareIndex(wxPoint pt);
    char findSquareIndex(ChessBoardPiece* p);
    void updateCoords();

DECLARE_EVENT_TABLE()
};

#endif /* WIDGETS_CHESSBOARD_H_ */
