#include <windows.h>
#include "Fill.h"

DRAWING_DATA gDrawData; // global data

void reDraw(HWND hwnd)
{
  InvalidateRect(hwnd, NULL, 1);
}

void drawPoint(int x, int y)
{
  SetPixel(gDrawData.hdcMem, x, y, CLR_BOUNDARY);
}

void drawPartialPoly()
{
  MoveToEx(gDrawData.hdcMem, 
    gDrawData.cornerPts[0][0].x,gDrawData.cornerPts[0][0].y, NULL);
  for(int i=1;i < gDrawData.nCornerPts; i++)
  {

    // If it is not the first point, join the point
    if (i%3 != 0)
    {
      LineTo(gDrawData.hdcMem, 
          gDrawData.cornerPts[i/3][i%3].x,gDrawData.cornerPts[i/3][i%3].y);
    }
         
    MoveToEx(gDrawData.hdcMem, 
         gDrawData.cornerPts[i/3][i%3].x,gDrawData.cornerPts[i/3][i%3].y, NULL);
    
    // If it is the last point, join the point to the initial point of the triangle
    if (i%3 == 2)
  	{
  		LineTo(gDrawData.hdcMem, 
          gDrawData.cornerPts[i/3][0].x,gDrawData.cornerPts[i/3][0].y);
    
	  }
  }
}


void drawPoly()
{
  drawPartialPoly();
}

void setDrawMode(MODE mode, HWND hwnd)
{
  gDrawData.drawMode = mode;
  setupMenus(hwnd);
}

void createMemoryBitmap(HDC hdc)
{
  gDrawData.hdcMem = CreateCompatibleDC(hdc);
  gDrawData.hbmp = CreateCompatibleBitmap(hdc, 
       gDrawData.maxBoundary.cx, gDrawData.maxBoundary.cy);
  SelectObject(gDrawData.hdcMem, gDrawData.hbmp);
  PatBlt(gDrawData.hdcMem, 0, 0, gDrawData.maxBoundary.cx, 
       gDrawData.maxBoundary.cy, PATCOPY);
}

void initialize(HWND hwnd, HDC hdc)
{
  gDrawData.nCornerPts = 0;
  gDrawData.hDrawPen=CreatePen(PS_SOLID, 1, CLR_BOUNDARY);

  // initialize fill pen colors for various regions
  gDrawData.hFillPen[MODE_CLEAR]=CreatePen(PS_SOLID, 1, CLR_BG);
  gDrawData.hFillPen[MODE_X]=CreatePen(PS_SOLID, 1, CLR_DEFAULT);
  gDrawData.hFillPen[MODE_Y]=CreatePen(PS_SOLID, 1, CLR_DEFAULT);
  gDrawData.hFillPen[MODE_Z]=CreatePen(PS_SOLID, 1, CLR_DEFAULT);
  gDrawData.hFillPen[MODE_XY]=CreatePen(PS_SOLID, 1, CLR_XY);
  gDrawData.hFillPen[MODE_XZ]=CreatePen(PS_SOLID, 1, CLR_XZ);
  gDrawData.hFillPen[MODE_YZ]=CreatePen(PS_SOLID, 1, CLR_YZ);
  gDrawData.hFillPen[MODE_XYZ]=CreatePen(PS_SOLID, 1, CLR_XYZ);

  gDrawData.maxBoundary.cx = GetSystemMetrics(SM_CXSCREEN);
  gDrawData.maxBoundary.cy = GetSystemMetrics(SM_CYSCREEN);
  createMemoryBitmap(hdc);
  setDrawMode(READY_MODE, hwnd);
}

void cleanup()
{
  DeleteDC(gDrawData.hdcMem);
}

void reset(HWND hwnd)
{
  gDrawData.nCornerPts = 0;
  gDrawData.drawMode = READY_MODE;

  PatBlt(gDrawData.hdcMem, 0, 0, gDrawData.maxBoundary.cx, 
         gDrawData.maxBoundary.cy, PATCOPY);

  reDraw(hwnd);
  setupMenus(hwnd);
}

void addPointToPolygon(HWND hwnd, int x, int y)
{
  /* the coordinates of the points are stored in an array */
	int nCornerPts = gDrawData.nCornerPts;
  if (gDrawData.nCornerPts < nMaxNoOfCornerPts)
  {
    SelectObject(gDrawData.hdcMem, gDrawData.hDrawPen);
    drawPoint(x,y);
    gDrawData.cornerPts[nCornerPts/3][nCornerPts%3].x = x;
    gDrawData.cornerPts[nCornerPts/3][nCornerPts%3].y = y;
    gDrawData.nCornerPts++;
  }
  /* if all 3 triangles are selected, render them */
  else
  {
    processPolygon(hwnd);
  }
  drawPartialPoly();
}


void drawImage(HDC hdc)
{
  BitBlt(hdc, 0, 0, gDrawData.maxBoundary.cx, 
    gDrawData.maxBoundary.cy, gDrawData.hdcMem, 0, 0, SRCCOPY);
}

void processLeftButtonDown(HWND hwnd, int x, int y)
{
  switch (gDrawData.drawMode)
  {
    case DRAW_MODE:
      addPointToPolygon(hwnd,x,y);
      reDraw(hwnd);
      break;

    default:
      return;
  }
}

void processPolygon(HWND hwnd)
{
  if (gDrawData.drawMode == DRAW_MODE and gDrawData.nCornerPts >= 8)
  {
    drawPoly();
    setDrawMode(DRAWN_MODE, hwnd);
    reDraw(hwnd);
  }
  else
  {
  	MessageBox(hwnd,
     "Invalid Input", 
            "Warning",MB_OK);
  }
}

void processCommonCommand(int cmd, HWND hwnd)
{
  int response;
  switch(cmd)
  {
    case ID_CLEAR:
      reset(hwnd);
      setDrawMode(READY_MODE, hwnd);
      break;
    case ID_DRAW_POLY:
      setDrawMode(DRAW_MODE, hwnd);
      break;
    case ID_EXIT:
      response=MessageBox(hwnd,"Quit the program?", "EXIT",
                          MB_YESNO);
      if(response==IDYES)
        PostQuitMessage(0);
      break;
    default:
      break;
  }
}

LRESULT CALLBACK WindowF(HWND hwnd,UINT message,WPARAM wParam,
                         LPARAM lParam)
{
  HDC hdc;
  PAINTSTRUCT ps;
  int x,y;

  switch(message)
  {
    case WM_CREATE:
      hdc = GetDC(hwnd);
      initialize(hwnd, hdc);
      ReleaseDC(hwnd, hdc);
      break;

    case WM_COMMAND:
      processCommand(LOWORD(wParam), hwnd);
      break;

    case WM_LBUTTONDOWN:
      x = LOWORD(lParam);
      y = HIWORD(lParam);
      processLeftButtonDown(hwnd, x,y);
      break;

    case WM_PAINT:
      hdc = BeginPaint(hwnd, &ps);
      drawImage(hdc);
      EndPaint(hwnd, &ps);
      break;

    case WM_DESTROY:
      cleanup();
      PostQuitMessage(0);
      break;

    default:
      break;
  }
  // Call the default window handler
  return DefWindowProc(hwnd, message, wParam, lParam);
}
