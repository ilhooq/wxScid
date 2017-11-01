/*
 * ChessBoard.cpp
 *
 */

#include <wx/wx.h>
#include <wx/dcgraph.h>

#include "ChessBoard.h"
#include "../Squares.h"

ChessBoardSquare::ChessBoardSquare()
{
  rect = wxRect(0, 0, 0, 0);
  color = NULL;
  piece = NULL;
}

void ChessBoardSquare::setPosition(int x, int y)
{
  rect.SetPosition(wxPoint(x, y));
}

void ChessBoardSquare::setWidth(int width)
{
  rect.SetSize(wxSize(width, width));
}

bool ChessBoardSquare::hitTest(wxPoint pt)
{
  return rect.Contains(pt);
}

void ChessBoardSquare::addPiece(ChessBoardPiece* p)
{
  piece = p;
}

void ChessBoardSquare::removePiece()
{
  delete piece;
  piece = NULL;
}

ChessBoardPiece::ChessBoardPiece(char type, wxImage * img) :
    x(0), y(0), width(0), height(0), show(true)
{
  pieceType = type;
  image = img;
}

bool ChessBoardPiece::hitTest(wxPoint pt)
{
  wxRect rect = getRect();
  return rect.Contains(pt);
}

void ChessBoardPiece::setRect(wxRect rect)
{
  x = rect.x;
  y = rect.y;
  width = rect.width;
  height = rect.height;
}

wxRect ChessBoardPiece::getRect()
{
  return wxRect(x, y, width, height);
}

wxBitmap ChessBoardPiece::getBitmap()
{
  wxImage img = image->Scale(width, height);
  wxBitmap bmp(img);
  return bmp;
}

BEGIN_EVENT_TABLE(ChessBoard, wxWindow)

EVT_MOTION(ChessBoard::onMotion)
EVT_LEFT_DOWN(ChessBoard::onLeftDown)
EVT_LEFT_UP(ChessBoard::onLeftUp)

EVT_PAINT(ChessBoard::onPaint)
EVT_SIZE(ChessBoard::onSize)

END_EVENT_TABLE()

ChessBoard::ChessBoard(wxWindow* parent, wxString themeDir) :
#ifdef __WXMSW__
 // Force Full repain on windows to avoid flickering
 // wxWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN ),
 wxWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE),
#else
 wxWindow(parent, wxID_ANY),
#endif
 theme(themeDir + "/merida.png", wxBITMAP_TYPE_PNG),
 backgroundColor(255, 255, 255, wxALPHA_OPAQUE),
 wSquareColor(244, 238, 215, wxALPHA_OPAQUE),
 bSquareColor(160, 137, 44, wxALPHA_OPAQUE),
 flipped(false),
 size(96, 96),
 boardSize(96),
 squareSize(boardSize / 8),
 dragStartPos(0, 0)
{
  themeImages[ChessBoard::wRook]   = theme.GetSubImage(wxRect(0, 0, 132, 132));
  themeImages[ChessBoard::wKnight] = theme.GetSubImage(wxRect(132, 0, 132, 132));
  themeImages[ChessBoard::wBishop] = theme.GetSubImage(wxRect(264, 0, 132, 132));
  themeImages[ChessBoard::wQueen]  = theme.GetSubImage(wxRect(396, 0, 132, 132));
  themeImages[ChessBoard::wKing]   = theme.GetSubImage(wxRect(528, 0, 132, 132));
  themeImages[ChessBoard::wPawn]   = theme.GetSubImage(wxRect(660, 0, 132, 132));

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
      dc.DrawBitmap(squares[i].piece->getBitmap(), squares[i].piece->x, squares[i].piece->y);
    }
  }
}

void ChessBoard::onSize(wxSizeEvent & evt)
{
  updateCoords();
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
    // bmp.SaveFile("test.bmp", wxBITMAP_TYPE_PNG);
    //wxIcon iconImg;
    //iconImg.CopyFromBitmap(bmb);
    //dragImage = new wxDragImage(iconImg);
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

void ChessBoard::addPiece(char pieceType, Square square)
{
  ChessBoardPiece * piece = new ChessBoardPiece(pieceType, &themeImages[pieceType]);
  squares[square].addPiece(piece);
}

void ChessBoard::removePiece(Square square)
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
  updateCoords();
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

