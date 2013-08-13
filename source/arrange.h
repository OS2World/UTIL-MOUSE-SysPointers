// ********************************************************************
//
// ARRANGE.C definitions for EDDIE
//
// ********************************************************************
// Version:1.0 (C)1994 EDM Soft
//

#define CASC_EDGE_NUM 	      	  2
#define CASC_EDGE_DENOM   		  3

// spazio per il testo dell'icona
#define TEXT 10		

#define ICON_BOTTOM			20
#define ICON_OFFSET			12
#define ICON_PARK_NUM       		5
#define ICON_PARK_DENOM     	3
#define CLASS_NAME_LENGTH		8

// prototipi di funzione

BOOL RemoveSysMenuInMenu( HWND hwnd) ;
BOOL ArrangeWindowPositions( PRECTL prc, SHORT cWnd, PSWP aswp, USHORT fStyle) ;
BOOL SetMainTitleText(	HWND, char *, BOOL) ;
BOOL SetTilePositions( PRECTL prc, SHORT cWnd, PSWP aswp);
SHORT CeilSquareRoot( USHORT us);
BOOL SetCascadePositions( PRECTL prc, SHORT cWnd, PSWP aswp);
BOOL SetCascadeParams( PRECTL prc, SHORT *pxEdge, SHORT *pyEdge, SHORT *pxDelta, SHORT *pyDelta, SHORT *cMaxWnd);
BOOL GetArrangeSwp(USHORT *, SWP *, USHORT *, SWP *, HWND);
BOOL GetArrangeRectangle(PRECTL, BOOL, HWND);
BOOL ArrangeIconPositions(USHORT, PSWP);
BOOL ArrangeWindows( HWND, USHORT, USHORT, BOOL) ;
BOOL ArrangeWindowPositions(PRECTL, SHORT, PSWP, USHORT) ;
