// **********************************
//
// SysPointers 1.0 by Enrico Demarin
// (mccoy@maya.dei.unipd.it)
//
// **********************************

#define INCL_WIN
#define INCL_GPICONTROL
#define INCL_PM
#define INCL_GPI
#define INCL_DOS

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "sysptr.h"
#include "dialog.h"


#define PTRKEY "PM_SysPointer"

// strings

CHAR *ST_PINSTALLED = "Custom Pointer installed -> %s";
CHAR *ST_PNOTINSTALLED = "Using default system pointer.";
CHAR *ST_FINDFILE = "Find pointer...";
CHAR *ST_CANTCHANGE = "Failed to change pointer.";
CHAR *ST_CANTDELETE = "Failed to delete pointer.";
CHAR *ST_SHUTDOWN = "You need to perform a shutdown to restore default pointers.";
CHAR *ST_REMOVE = "Custom pointer removed.";


typedef struct NODE
{
	ULONG active;
	CHAR name[CCHMAXPATH];
}   PTRNODE;


// functions prototypes

int main(void);
MRESULT EXPENTRY MyDlgWndProc(HWND, ULONG, MPARAM, MPARAM);
void updatedialog(HWND);
BOOL choosepointer(HWND);
BOOL deletecurrent(HWND);
void togglepointer(void);


#define DPOINTERS 19 // # of pointers

// descriptions

char *pszDescr[DPOINTERS] = {
	"Arrow pointer",
	"Text insertion pointer",
	"Hourglass pointer",
	"Sizing pointer",
	"Move pointer",
	"Resize NW-SE",
	"Resize NE-SW",
	"Resize W-E",
	"Resize N-S",
	"Application icon",
	"Information icon",
	"Question icon",
	"Error icon",
	"Warning icon",
	"'Illegal' pointer",
	"File icon",
	"Folder icon",
	"Multiple file selection",
        "Program icon"};

// pointer # identifiers

int iIndex[DPOINTERS] = {
	SPTR_ARROW, SPTR_TEXT, SPTR_WAIT, SPTR_SIZE,
	SPTR_MOVE, SPTR_SIZENWSE, SPTR_SIZENESW,
	SPTR_SIZEWE, SPTR_SIZENS, SPTR_APPICON,
	SPTR_ICONINFORMATION, SPTR_ICONQUESTION, SPTR_ICONERROR,
	SPTR_ICONWARNING, SPTR_ILLEGAL, SPTR_FILE,
	SPTR_FOLDER, SPTR_MULTFILE, SPTR_PROGRAM};

// global vars
LONG ScreenSizeX = WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN) + 2;
	LONG ScreenSizeY = WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);


int main(void)
{
	HAB hab;
	HMQ hmq;
	QMSG qmsg;

	// in...

	hab = WinInitialize(0);
	hmq = WinCreateMsgQueue(hab, 0L);


	// popup dialog...

	WinDlgBox(HWND_DESKTOP, NULLHANDLE, (PFNWP) MyDlgWndProc, 0, MAINDIALOG, NULL);

	// ...out

	WinDestroyMsgQueue(hmq);
	WinTerminate(hab);

	return 0;
}

 

// choose a pointer (file dialog)

BOOL choosepointer(HWND hwnd)
{
	HWND hwndDialog;
	FILEDLG fileDlgInfo;
	USHORT index;
	CHAR pszTmp1[32];
	PTRNODE ptrNode;
	ULONG size=sizeof(ULONG);
	ICONINFO iconinfo;

 	memset(&iconinfo,0,sizeof(ICONINFO));

	iconinfo.cb=sizeof(ICONINFO);
	iconinfo.fFormat=ICON_FILE;
 	

	ptrNode.active = TRUE;

	index = SHORT1FROMMP(WinSendDlgItemMsg(hwnd, MYLBOX, LM_QUERYSELECTION, MPFROMLONG(0L), MPFROMLONG(0L)));
        index = iIndex[index];

	sprintf(pszTmp1, "%u", index);

	memset(&fileDlgInfo, 0, sizeof(FILEDLG));
	fileDlgInfo.cbSize = sizeof(fileDlgInfo);


	fileDlgInfo.fl = FDS_OPEN_DIALOG | FDS_CENTER;
	fileDlgInfo.pszTitle = ST_FINDFILE;
	strcpy(fileDlgInfo.szFullFile, "*.PTR");
	hwndDialog = WinFileDlg(HWND_DESKTOP, hwnd, &fileDlgInfo);

	if (hwndDialog && (fileDlgInfo.lReturn == DID_OK))
	{
		strcpy(ptrNode.name, fileDlgInfo.szFullFile);
		size+=strlen(ptrNode.name)+1;
	
		iconinfo.pszFileName=ptrNode.name;
		WinSetSysPointerData(HWND_DESKTOP,index,&iconinfo);
		togglepointer();
		return PrfWriteProfileData(HINI_USERPROFILE, (PSZ) PTRKEY,
						   (PSZ) pszTmp1, (PVOID) & ptrNode, size);


	}
	else
		return FALSE;
}

// delete pointer definition from OS2.INI

BOOL deletecurrent(HWND hwnd)
{
	USHORT index;
	CHAR pszTmp1[32];
	BOOL res;

	index = SHORT1FROMMP(WinSendDlgItemMsg(hwnd, MYLBOX, LM_QUERYSELECTION, MPFROMLONG(0L), MPFROMLONG(0L)));

	sprintf(pszTmp1, "%u", iIndex[index]);
	res= (BOOL) PrfWriteProfileString(HINI_USERPROFILE, (PSZ) PTRKEY, (PSZ) pszTmp1, NULL);

	
	WinSetSysPointerData(HWND_DESKTOP,index,NULL);
	 
	togglepointer();

		
	// Delete key

	return res;
}

// update dialog box

void updatedialog(HWND hwnd)
{
	USHORT index;
	CHAR pszTmp1[32];
	CHAR pszTmp2[CCHMAXPATH];
	CHAR pszTmp3[CCHMAXPATH + 32];
	BOOL res;
	HPOINTER hptrCurrent=NULL;
	PTRNODE ptrNode;
	ULONG buflen = sizeof(PTRNODE);
	
	index = SHORT1FROMMP(WinSendDlgItemMsg(hwnd, MYLBOX, LM_QUERYSELECTION, MPFROMLONG(0L), MPFROMLONG(0L)));
	index = iIndex[index];
	sprintf(pszTmp1, "%u", index);

	res = PrfQueryProfileData(HINI_USERPROFILE, (PSZ) PTRKEY, (PSZ) pszTmp1, (PVOID) & ptrNode, &buflen);


	if (ptrNode.name[0] == 0)
		res = FALSE;

	WinEnableControl(hwnd, UNINSTALL, res);

	if (res)
		sprintf(pszTmp3, ST_PINSTALLED, ptrNode.name);
	else
		strcpy(pszTmp3, ST_PNOTINSTALLED);

	WinSetDlgItemText(hwnd, INFOS, pszTmp3);

	

	hptrCurrent = WinQuerySysPointer(HWND_DESKTOP, index,FALSE);
	                   
	WinSendDlgItemMsg(hwnd, CURRSHAPE, VM_SETITEM, MPFROM2SHORT(1, 1), MPFROMLONG(hptrCurrent));
	
	
}


// dialog box window procedure

MRESULT EXPENTRY MyDlgWndProc(HWND hwnd,
							      ULONG msg,
							      MPARAM mp1,
							      MPARAM mp2)
{
	switch (msg)
	{
		case WM_INITDLG:
		{

			SWP swp;

			// center dialog

			WinQueryWindowPos(hwnd, (PSWP) & swp);
			WinSetWindowPos(hwnd, (HWND) 0,
							((SHORT) ((ScreenSizeX - swp.cx) / 2)),
							((SHORT) ((ScreenSizeY - swp.cy) / 2)),
							0, 0, SWP_MOVE);

			// fill listbox

			WinSendDlgItemMsg(hwnd, MYLBOX, LM_DELETEALL, NULLHANDLE, NULLHANDLE);
			for (int i = 0; i < DPOINTERS; i++)
				WinSendDlgItemMsg(hwnd, MYLBOX, LM_INSERTITEM, MPFROMSHORT(LIT_END),
								  MPFROMP(pszDescr[i]));
			// Select first

			WinSendDlgItemMsg(hwnd, MYLBOX, LM_SELECTITEM, MPFROMSHORT(0), MPFROMSHORT(TRUE));

			// update

			updatedialog(hwnd);
		} break;

	case WM_CONTROL:
		if (SHORT1FROMMP(mp1) == MYLBOX)
			switch (SHORT2FROMMP(mp1))
			{
			case LN_ENTER:		// enter or double-click on lb item
				if (choosepointer(hwnd))
					updatedialog(hwnd);
				return 0;
				break;
			case LN_SELECT:	// user selected another lb item
				updatedialog(hwnd);
				return 0;
				break;
			
			}
		break;

	case WM_COMMAND:
		switch (SHORT1FROMMP(mp1))
		{
		case SELECTFILE:		// install push button pressed

			if (choosepointer(hwnd))
			{
				updatedialog(hwnd);
			}

			else
				WinSetDlgItemText(hwnd, INFOS, ST_CANTCHANGE);
			return 0;
			break;
		case UNINSTALL:		// uninstall push button pressed
			if (deletecurrent(hwnd))
			{
				WinMessageBox(HWND_DESKTOP, hwnd, 
					ST_SHUTDOWN,
					ST_REMOVE, 0,
					MB_MOVEABLE | MB_INFORMATION | MB_OK);
					
				updatedialog(hwnd);
			}

			else
				WinSetDlgItemText(hwnd, INFOS, ST_CANTDELETE);
			return 0;
			break;

		case DISMISS:			// ok push button pressed
			WinDismissDlg(hwnd, TRUE);
			return 0;
		} break;


	}
	// default processing
	return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

// enable new pointer

void togglepointer(void)
{
 HPOINTER hptrCurrent;

 
 hptrCurrent=WinQuerySysPointer(HWND_DESKTOP,SPTR_WAIT,FALSE);
 WinSetPointer(HWND_DESKTOP,hptrCurrent);
 hptrCurrent=WinQuerySysPointer(HWND_DESKTOP,SPTR_ARROW,FALSE);
 WinSetPointer(HWND_DESKTOP,hptrCurrent);
}	
