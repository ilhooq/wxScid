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

EVT_MOTION(ChessBoard::onMotion)
EVT_LEFT_DOWN(ChessBoard::onLeftDown)
EVT_LEFT_UP(ChessBoard::onLeftUp)

EVT_PAINT(ChessBoard::onPaint)
END_EVENT_TABLE()

ChessBoard::ChessBoard(wxWindow* parent, wxString themeDir, const wxWindowID id) :
#ifdef __WXMSW__
    // Force Full repain on windows to avoid flickering
    // wxWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN ),
    wxWindow(parent, id, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE),
#else
    wxWindow(parent, id),
#endif
    theme(themeDir + "/merida.png", wxBITMAP_TYPE_PNG),
    backgroundColor(255, 255, 255, wxALPHA_OPAQUE),
    wSquareColor(244, 238, 215, wxALPHA_OPAQUE),
    bSquareColor(160, 137, 44,wxALPHA_OPAQUE),
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
            squares[i].color = fileIdx % 2 ? &wSquareColor : &bSquareColor;
        } else {
            squares[i].color = fileIdx % 2 ? &bSquareColor : &wSquareColor;
        }

        fileIdx++;

        if (fileIdx == 8) {
            rankIdx++;
            fileIdx = 0;
        }
    }
#ifdef __WXMSW__
    // Avoid flickering on Windows
    SetBackgroundStyle(wxBG_STYLE_PAINT);
#endif
}

void ChessBoard::onPaint(wxPaintEvent & evt)
{
    // wxPaintDC pdc = wxPaintDC(this);
    // try {
    //wxGCDC dc = wxGCDC(pdc);
    // } catch () {
    //	dc = pdc
    // }

    updateCoords();

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
            dc.DrawBitmap(squares[i].piece->getBitmap(), squares[i].piece->x,
                    squares[i].piece->y);
        }
    }
}

void ChessBoard::onLeftDown(wxMouseEvent & evt)
{
    // Did the mouse go down on one of our pieces ?
    ChessBoardPiece* piece = findPiece(evt.GetPosition());

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

void ChessBoard::onLeftUp(wxMouseEvent & evt)
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
        char squareFrom = findSquareIndex(dragPiece);
        char squareTo = findSquareIndex(evt.GetPosition());

        if (squareFrom != squareTo) {
            dragPiece->setRect(squares[squareTo].rect);

            // Move piece
            squares[squareFrom].piece = NULL;
            squares[squareTo].piece = dragPiece;
        }

        dragPiece->show = true;
        RefreshRect(dragPiece->getRect());
        dragPiece = NULL;
    }
}

void ChessBoard::onMotion(wxMouseEvent & evt)
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
        RefreshRect(dragPiece->getRect(), true);
        Update();

        wxBitmap bmp = dragPiece->getBitmap();
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

void ChessBoard::addPiece(Pieces pieceType, Squares square)
{
    ChessBoardPiece * piece = new ChessBoardPiece(pieceType, &themeImages[pieceType]);
    squares[square].addPiece(piece);
}

void ChessBoard::removePiece(Squares square)
{
    squares[square].removePiece();
}

ChessBoardPiece* ChessBoard::findPiece(wxPoint pt)
{
    for (int i = 0; i < 64; i++) {
        if (squares[i].piece != NULL && squares[i].piece->hitTest(pt)) {
            return squares[i].piece;
        }
    }
    return NULL;
}

void ChessBoard::flip()
{
    flipped = !flipped;
    // updateCoords();
    Refresh();
}

char ChessBoard::findSquareIndex(wxPoint pt)
{
    for (int i = 0; i < 64; i++) {
        if (squares[i].hitTest(pt)) {
            return i;
        }
    }
    return -1;
}

char ChessBoard::findSquareIndex(ChessBoardPiece* p)
{
    for (int i = 0; i < 64; i++) {
        if (squares[i].piece != NULL && squares[i].piece == p) {
            return i;
        }
    }

    return -1;
}

char ChessBoard::getRankIndex(char squareIndex)
{
    return flipped ? squareIndex / 8 : 7 - squareIndex / 8;
}

char ChessBoard::getFileIndex(char squareIndex)
{
    return flipped ? 7 - squareIndex % 8 : squareIndex % 8;
}

void ChessBoard::updateCoords()
{
    size = GetClientSize();

    boardSize = size.x < size.y ? size.x : size.y;
    squareSize = boardSize / 8;

    // Calculate squares sizes and positions
    int squareX = 0, squareY = 0;

    for (int i = 0; i < 64; i++) {

        squareX = getFileIndex(i) * squareSize;
        squareY = getRankIndex(i) * squareSize;

        squares[i].setPosition(squareX, squareY);
        squares[i].setWidth(squareSize);

        // Calculate piece sizes and positions
        if (squares[i].piece != NULL) {
            squares[i].piece->setRect(squares[i].rect);
        }
    }
}

void ChessBoard::Clear()
{
    for (int i = 0; i < 64; i++) {
        if (squares[i].piece != NULL) {
            squares[i].removePiece();
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

                    case '1': fileIndex += 1; break;
                    case '2': fileIndex += 2; break;
                    case '3': fileIndex += 3; break;
                    case '4': fileIndex += 4; break;
                    case '5': fileIndex += 5; break;
                    case '6': fileIndex += 6; break;
                    case '7': fileIndex += 7; break;
                    case '8': fileIndex += 8; break;
                    case 'P': addPiece(ChessBoard::wPawn,   (ChessBoard::Squares) squareIndex); fileIndex++; break;
                    case 'K': addPiece(ChessBoard::wKing,   (ChessBoard::Squares) squareIndex); fileIndex++; break;
                    case 'Q': addPiece(ChessBoard::wQueen,  (ChessBoard::Squares) squareIndex); fileIndex++; break;
                    case 'N': addPiece(ChessBoard::wKnight, (ChessBoard::Squares) squareIndex); fileIndex++; break;
                    case 'B': addPiece(ChessBoard::wBishop, (ChessBoard::Squares) squareIndex); fileIndex++; break;
                    case 'R': addPiece(ChessBoard::wRook,   (ChessBoard::Squares) squareIndex); fileIndex++; break;
                    case 'p': addPiece(ChessBoard::bPawn,   (ChessBoard::Squares) squareIndex); fileIndex++; break;
                    case 'k': addPiece(ChessBoard::bKing,   (ChessBoard::Squares) squareIndex); fileIndex++; break;
                    case 'q': addPiece(ChessBoard::bQueen,  (ChessBoard::Squares) squareIndex); fileIndex++; break;
                    case 'n': addPiece(ChessBoard::bKnight, (ChessBoard::Squares) squareIndex); fileIndex++; break;
                    case 'b': addPiece(ChessBoard::bBishop, (ChessBoard::Squares) squareIndex); fileIndex++; break;
                    case 'r': addPiece(ChessBoard::bRook,   (ChessBoard::Squares) squareIndex); fileIndex++; break;

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


