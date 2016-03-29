/* $Id: SmartMBT.c,v 1.2 1994/09/28 22:20:01 cwikla Exp $ */
/*
 * Copyright 1994 John L. Cwikla
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appears in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of John L. Cwikla or
 * Wolfram Research, Inc not be used in advertising or publicity
 * pertaining to distribution of the software without specific, written
 * prior permission.	John L. Cwikla and Wolfram Research, Inc make no
 * representations about the suitability of this software for any
 * purpose.	It is provided "as is" without express or implied warranty.
 *
 * John L. Cwikla and Wolfram Research, Inc disclaim all warranties with
 * regard to this software, including all implied warranties of
 * merchantability and fitness, in no event shall John L. Cwikla or
 * Wolfram Research, Inc be liable for any special, indirect or
 * consequential damages or any damages whatsoever resulting from loss of
 * use, data or profits, whether in an action of contract, negligence or
 * other tortious action, arising out of or in connection with the use or
 * performance of this software.
 *
 * Author:
 *	John L. Cwikla
 *	X Programmer
 *	Wolfram Research Inc.
 *
 *	cwikla@wri.com
*/

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Core.h>

#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/Separator.h>
#include <Xm/RowColumn.h>
#include <Xm/DialogS.h>

#include "SmartMB.h"

#define APPNAME "SmartMBTest"
#define APPCLASS "SmartMBTest"

static void QuitIt(_w, _nil, _event)
Widget _w;
caddr_t _nil;
XEvent *_event;
{
	if (_event->type == ButtonPress)
		if (((XButtonEvent *)_event)->button == 3)
		{
			printf("Have a nice day. --JLC\n");
			exit(1);
		}
}

main(argc, argv)
int argc;
char *argv[];
{
	Widget row, toplevel, smb, defButton, popup;
	XtAppContext app;
	Display *theDisplay;
	Arg warg[8];
	int n, i;

	XtToolkitInitialize();
	app = XtCreateApplicationContext();
	
	theDisplay = XtOpenDisplay (app, NULL, APPNAME, APPCLASS, 
		NULL, 0, &argc, argv);

	if (!theDisplay)
	{
		printf("%s: can't open display, exiting...", APPNAME);
		exit (0);
	}

	toplevel = XtAppCreateShell (APPNAME, APPCLASS,
		applicationShellWidgetClass, theDisplay, NULL, 0);

	smb = XtCreateManagedWidget("SmartMessageBoxInformation", xmSmartMessageBoxWidgetClass,
		toplevel, NULL, 0);

	XtCreateManagedWidget("Separator", xmSeparatorWidgetClass, smb, NULL, 0);
	XtCreateManagedWidget("MessageLabel", xmLabelWidgetClass, smb, NULL, 0);

	n = 0;
	defButton = XtCreateManagedWidget("OK", xmPushButtonWidgetClass, smb, warg, n);
	XtCreateManagedWidget("Help", xmPushButtonWidgetClass, smb, NULL, 0);

	n = 0;
	XtSetArg(warg[n], XmNdefaultButton, defButton); n++;
	XtSetValues(smb, warg, n);

	XtRealizeWidget(toplevel);

	toplevel = XtAppCreateShell (APPNAME, APPCLASS,
		applicationShellWidgetClass, theDisplay, NULL, 0);

	smb = XtCreateManagedWidget("SmartMessageBox", xmSmartMessageBoxWidgetClass,
		toplevel, NULL, 0);

	XtCreateManagedWidget("Separator", xmSeparatorWidgetClass, smb, NULL, 0);
	row = XtCreateManagedWidget("RowCol", xmRowColumnWidgetClass, smb, NULL, 0);

	for(i=0;i<3;i++)
		XtCreateManagedWidget("Control Button", xmPushButtonWidgetClass, row, NULL, 0);

	n = 0;
	defButton = XtCreateManagedWidget("OK", xmPushButtonWidgetClass, smb, warg, n);
	XtCreateManagedWidget("Apply", xmPushButtonWidgetClass, smb, NULL, 0);
	XtCreateManagedWidget("Cancel", xmPushButtonWidgetClass, smb, NULL, 0);
	XtCreateManagedWidget("Help", xmPushButtonWidgetClass, smb, NULL, 0);

	n = 0;
	XtSetArg(warg[n], XmNdefaultButton, defButton); n++;
	XtSetValues(smb, warg, n);

	XtRealizeWidget(toplevel);

	toplevel = XmCreateDialogShell(smb, "popup_shell", NULL, 0);

	smb = XtCreateWidget("SmartMessageBox", xmSmartMessageBoxWidgetClass,
		toplevel, NULL, 0);

	XtCreateManagedWidget("NoSeparatorLabel", xmLabelWidgetClass, smb, NULL, 0);

	n = 0;
	defButton = XtCreateManagedWidget("OK", xmPushButtonWidgetClass, smb, warg, n);
	XtCreateManagedWidget("Help", xmPushButtonWidgetClass, smb, NULL, 0);

	n = 0;
	XtSetArg(warg[n], XmNdefaultButton, defButton); n++;
	XtSetValues(smb, warg, n);

	XtManageChild(smb);
	XtPopup(toplevel, XtGrabNone);

	toplevel = XtAppCreateShell (APPNAME, APPCLASS,
		applicationShellWidgetClass, theDisplay, NULL, 0);

	smb = XtCreateManagedWidget("SmartMessageBox", xmSmartMessageBoxWidgetClass,
		toplevel, NULL, 0);

	for(i=0;i<5;i++)
		defButton = XtCreateManagedWidget("Buttons", xmPushButtonWidgetClass, smb, NULL, 0);

	XtSetArg(warg[n], XmNdefaultButton, defButton); n++;
	XtSetValues(smb, warg, n);

	XtRealizeWidget(toplevel);


	XtAppMainLoop(app);
}
