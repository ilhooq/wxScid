/*
 * ChessBoard.cpp
 *
 */

#include <wx/wx.h>
#include <wx/dcgraph.h>

#include "ChessBoard.h"

const char squareNames[64][3] =
{
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"
};

BEGIN_EVENT_TABLE(ChessBoard, wxWindow)
    EVT_MOTION(ChessBoard::OnMotion)
    EVT_LEFT_DOWN(ChessBoard::OnLeftDown)
    EVT_LEFT_UP(ChessBoard::OnLeftUp)
    EVT_PAINT(ChessBoard::OnPaint)
END_EVENT_TABLE()

ChessBoard::ChessBoard(wxWindow* parent, const wxWindowID id, const wxString themeDir, const wxPoint& pos, const wxSize& size, long style) :
    wxWindow(parent, id, pos, size, style),
    theme(themeDir + "/merida.png", wxBITMAP_TYPE_PNG),
    backgroundColor(255, 255, 255, wxALPHA_OPAQUE),
    wSquareColor(244, 238, 215, wxALPHA_OPAQUE),
    bSquareColor(160, 137, 44, wxALPHA_OPAQUE),
    flipped(false),
    size(96, 96),
    boardSize(96),
    squareSize(boardSize / 8),
    dragStartPos(0, 0),
    side(WHITE)
{
    themeImages[ChessBoard::wRook]   = theme.GetSubImage(wxRect(0, 0, 132, 132));
    themeImages[ChessBoard::wKnight] = theme.GetSubImage(wxRect(132, 0, 132, 132));
    themeImages[ChessBoard::wBishop] = theme.GetSubImage(wxRect(264, 0, 132, 132));
    themeImages[ChessBoard::wQueen]  = theme.GetSubImage(wxRect(396, 0, 132, 132));
    themeImages[ChessBoard::wKing]   = theme.GetSubImage(wxRect(528, 0, 132, 132));
    themeImages[ChessBoard::wPawn]   = theme.GetSubImage(wxRect(660, 0, 132, 132));

    themeImages[ChessBoard::bRook]   = theme.GetSubImage(wxRect(0,   132, 132, 132));
    themeImages[ChessBoard::bKnight] = theme.GetSubImage(wxRect(132, 132, 132, 132));
    themeImages[ChessBoard::bBishop] = theme.GetSubImage(wxRect(264, 132, 132, 132));
    themeImages[ChessBoard::bQueen]  = theme.GetSubImage(wxRect(396, 132, 132, 132));
    themeImages[ChessBoard::bKing]   = theme.GetSubImage(wxRect(528, 132, 132, 132));
    themeImages[ChessBoard::bPawn]   = theme.GetSubImage(wxRect(660, 132, 132, 132));

    dragPiece = NULL;
    dragImage = NULL;

    char rankIdx = 0, fileIdx = 0;

    for (int i = 0; i < 64; i++) {
        if (rankIdx % 2) {
            squares[i].color = fileIdx % 2 ? &bSquareColor : &wSquareColor;
        } else {
            squares[i].color = fileIdx % 2 ? &wSquareColor : &bSquareColor;
        }

        fileIdx++;

        if (fileIdx == 8) {
            rankIdx++;
            fileIdx = 0;
        }
    }

#ifdef __WXMSW__
    // Avoid flickering on Windows
    // Force full repaint when resizing
    SetWindowStyle(GetWindowStyle() | wxFULL_REPAINT_ON_RESIZE);
    SetBackgroundStyle(wxBG_STYLE_PAINT);
#endif
}

void ChessBoard::Flip()
{
    flipped = !flipped;
    Refresh();
}

void ChessBoard::Clear()
{
    for (int i = 0; i < 64; i++) {
        if (squares[i].piece != NULL) {
            squares[i].RemovePiece();
        }
    }
    Refresh();
}


bool ChessBoard::LoadPositionFromFen(wxString FEN)
{
    Clear();

    int i = 0, part = 0, rankIndex = 7, fileIndex = 0, squareIndex = 0;

    const char* fen = FEN.c_str();

    for (i=0; i < FEN.Length(); i++) {

        if (fen[i] == ' ') {
            part++;
            continue;
        }

        switch (part) {
            case  0:
                squareIndex = 8*rankIndex + fileIndex;
                switch(fen[i]) {
                    case '/':
                        rankIndex -=1;
                        fileIndex = 0;
                        break;

                    #define ADDPIECE(piece) AddPiece(piece, (ChessBoard::Squares) squareIndex); fileIndex++

                    case '1': fileIndex += 1; break;
                    case '2': fileIndex += 2; break;
                    case '3': fileIndex += 3; break;
                    case '4': fileIndex += 4; break;
                    case '5': fileIndex += 5; break;
                    case '6': fileIndex += 6; break;
                    case '7': fileIndex += 7; break;
                    case '8': fileIndex += 8; break;
                    case 'P': ADDPIECE(ChessBoard::wPawn); break;
                    case 'K': ADDPIECE(ChessBoard::wKing); break;
                    case 'Q': ADDPIECE(ChessBoard::wQueen); break;
                    case 'N': ADDPIECE(ChessBoard::wKnight); break;
                    case 'B': ADDPIECE(ChessBoard::wBishop); break;
                    case 'R': ADDPIECE(ChessBoard::wRook); break;
                    case 'p': ADDPIECE(ChessBoard::bPawn); break;
                    case 'k': ADDPIECE(ChessBoard::bKing); break;
                    case 'q': ADDPIECE(ChessBoard::bQueen); break;
                    case 'n': ADDPIECE(ChessBoard::bKnight); break;
                    case 'b': ADDPIECE(ChessBoard::bBishop); break;
                    case 'r': ADDPIECE(ChessBoard::bRook); break;

                    #undef ADDPIECE

                    default:
                        return false;
                }
                break;
            case 1:
                side = (fen[i] == 'w') ? WHITE : BLACK;
                break;
        }
    }

    Refresh();

    return true;
}

void ChessBoard::AddPiece(Pieces pieceType, Squares square)
{
    ChessBoardPiece * piece = new ChessBoardPiece(pieceType, &themeImages[pieceType]);
    squares[square].AddPiece(piece);
}

void ChessBoard::RemovePiece(Squares square)
{
    squares[square].RemovePiece();
}

//////// Begin private implementation ////////

void ChessBoard::UpdateCoords()
{
    size = GetClientSize();

    boardSize = size.x < size.y ? size.x : size.y;
    squareSize = boardSize / 8;

    // Calculate squares sizes and positions
    int squareX = 0, squareY = 0;

    for (int i = 0; i < 64; i++) {

        squareX = GetFileIndex(i) * squareSize;
        squareY = GetRankIndex(i) * squareSize;

        squares[i].SetPosition(squareX, squareY);
        squares[i].SetWidth(squareSize);

        // Calculate piece sizes and positions
        if (squares[i].piece != NULL) {
            squares[i].piece->SetRect(squares[i].rect);
        }
    }
}

void ChessBoard::OnPaint(wxPaintEvent & evt)
{
    // wxPaintDC pdc = wxPaintDC(this);
    // try {
    //wxGCDC dc = wxGCDC(pdc);
    // } catch () {
    //	dc = pdc
    // }

    // Don't paint the board under 10px to avoid crash
    if (GetClientSize().x < 10 || GetClientSize().y < 10) {
        return;
    }

    UpdateCoords();

    wxPaintDC pdc(this);
    wxGCDC dc(pdc);

    // No draw borders
    dc.SetPen(wxNullPen);

    // Draw the board container
    dc.SetBrush(wxBrush(backgroundColor));
    dc.DrawRectangle(wxRect(0, 0, size.x, size.y));

    // Draw squares
    for (int i = 0; i < 64; i++) {
        dc.SetBrush(wxBrush(*squares[i].color));
        dc.DrawRectangle(squares[i].rect);
        // Draw Piece
        if (squares[i].piece != NULL && squares[i].piece->show == true) {
            dc.DrawBitmap(squares[i].piece->GetBitmap(), squares[i].piece->x,
                    squares[i].piece->y);
        }
    }
}

void ChessBoard::OnLeftDown(wxMouseEvent & evt)
{
    // Did the mouse go down on one of our pieces ?
    ChessBoardPiece* piece = FindPiece(evt.GetPosition());

    /*
     * If a piece was 'hit', then set that as the shape we're going to
     * drag around. Get our start position. Dragging has not yet started.
     * That will happen once the mouse moves, OR the mouse is released.
     */
    if (piece != NULL) {
        dragPiece = piece;
        dragStartPos = evt.GetPosition();
    }
}

void ChessBoard::OnLeftUp(wxMouseEvent & evt)
{
    if (dragImage == NULL || dragPiece == NULL) {
        return;
    }

    // Hide the image, end dragging, and nuke out the drag image.
    dragImage->Hide();
    dragImage->EndDrag();
    delete dragImage;
    dragImage = NULL;

    // Reposition the piece
    if (dragPiece != NULL) {
        char squareFrom = FindSquareIndex(dragPiece);
        char squareTo = FindSquareIndex(evt.GetPosition());

        if (squareTo != -1 && squareFrom != squareTo) {
            dragPiece->SetRect(squares[squareTo].rect);

            // Move piece
            squares[squareFrom].piece = NULL;
            squares[squareTo].piece = dragPiece;
        }

        dragPiece->show = true;
        RefreshRect(dragPiece->GetRect());
        dragPiece = NULL;
    }
}

void ChessBoard::OnMotion(wxMouseEvent & evt)
{
    // Ignore mouse movement if we're not dragging.
    if (dragPiece == NULL || !evt.Dragging() || !evt.LeftIsDown()) {
        return;
    }

    // if we have a piece to drage, but haven't started dragging yet
    if (dragPiece != NULL && dragImage == NULL) {
        // Only start the drag after having moved a couple pixels
        /*
         int tolerance = 2;
         wxPoint pt = evt.GetPosition();
         dx = abs(pt.x - self.dragStartPos.x)
         dy = abs(pt.y - self.dragStartPos.y)
         if dx <= tolerance and dy <= tolerance:
         return
         */

        // Refresh the area of the window where the piece was so it will get erased.
        dragPiece->show = false;
        RefreshRect(dragPiece->GetRect(), true);
        Update();

        wxBitmap bmp = dragPiece->GetBitmap();
        dragImage = new wxDragImage(bmp);
        wxPoint hotspot = dragStartPos - wxPoint(dragPiece->x, dragPiece->y);

        dragImage->BeginDrag(hotspot, this, false);
        dragImage->Move(evt.GetPosition());
        dragImage->Show();
    }

    // if we have shape and image then move it, posibly highlighting another shape.
    else if (dragPiece != NULL && dragImage != NULL) {

        // Now move it and show it again if needed
        dragImage->Move(evt.GetPosition());
    }
}

ChessBoardPiece* ChessBoard::FindPiece(wxPoint pt)
{
    for (int i = 0; i < 64; i++) {
        if (squares[i].piece != NULL && squares[i].piece->HitTest(pt)) {
            return squares[i].piece;
        }
    }
    return NULL;
}

int ChessBoard::FindSquareIndex(wxPoint pt)
{
    for (int i = 0; i < 64; i++) {
        if (squares[i].HitTest(pt)) {
            return i;
        }
    }

    return -1;
}

int ChessBoard::FindSquareIndex(ChessBoardPiece* p)
{
    for (int i = 0; i < 64; i++) {
        if (squares[i].piece != NULL && squares[i].piece == p) {
            return i;
        }
    }

    return -1;
}

int ChessBoard::GetRankIndex(int squareIndex)
{
    return flipped ? squareIndex / 8 : 7 - squareIndex / 8;
}

int ChessBoard::GetFileIndex(int squareIndex)
{
    return flipped ? 7 - squareIndex % 8 : squareIndex % 8;
}


