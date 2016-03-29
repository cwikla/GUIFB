/* $Id: GUIFBT.c,v 1.4 1996/02/13 22:00:40 cwikla Exp $ */
/*
 * Copyright 1996 John L. Cwikla
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appears in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of John L. Cwikla or
 * Wolfram Research, Inc not be used in advertising or publicity
 * pertaining to distribution of the software without specific, written
 * prior permission.    John L. Cwikla and Wolfram Research, Inc make no
 * representations about the suitability of this software for any
 * purpose. It is provided "as is" without express or implied warranty.
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
 *  John L. Cwikla
 *  X Programmer
 *  Wolfram Research Inc.
 *
 *  cwikla@wri.com
*/

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/CoreP.h>
#include <X11/CompositeP.h>
#include <X11/ShellP.h>
#include <Xm/PushB.h>
#include <Xm/SeparatoG.h>

#if SMARTMB
#include "SmartMB.h"
#endif
#include "GUIFB.h"
#include "guifbff.h"

#include <stdio.h>

#define APPNAME "GUIFBTest"
#define APPCLASS "GUIFBTest"

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

static void QuitIt(_w, _nil, _event)
Widget _w;
void *_nil;
XEvent *_event;
{
	int n;
	Arg warg[2];

	if (_event->type == ButtonPress)
		if (((XButtonEvent *)_event)->button == 3)
		{
			printf("Have a nice day. --JLC\n");
			exit(1);
		}
}

static Widget AdvancedButton, StandardButton;

static void changeMode(Widget _w, XtPointer _data, XtPointer _call)
{
	Widget fbw;
	unsigned char mode;
	Arg warg[2];
	int n;

	fbw = (Widget)_data;

	n = 0;
	XtSetArg(warg[n], XmNdialogMode, &mode); n++;
	XtGetValues(fbw, warg, n);

	if (mode == XmGUIFB_STANDARD_MODE)
	{
		XtManageChild(AdvancedButton);
		XtUnmanageChild(StandardButton);
		XmGUIFileBoxSetMode(fbw, XmGUIFB_ADVANCED_MODE);
	}
	else
	{
        XtManageChild(StandardButton);
        XtUnmanageChild(AdvancedButton);
        XmGUIFileBoxSetMode(fbw, XmGUIFB_STANDARD_MODE);
	}
}

main(argc, argv)
int argc;
char *argv[];
{
	char *dir, *bestMatch;
	char buffer[25];
	Widget guifb, toplevel, rc, smartMB;
	XtAppContext app;
	Display *theDisplay;
	int theScreenNumber;
	Arg warg[3];
	int n, i;
	CompositeWidget comp;
	const char **dirList, **fileList;
	int dirListSize, fileListSize;
	unsigned char mode;
	char *pattern;

	XtToolkitInitialize();
	app = XtCreateApplicationContext();

	theDisplay = XtOpenDisplay (app, NULL, APPNAME, APPCLASS, 
		NULL, 0, &argc, argv);

	if (!theDisplay)
	{
		printf("%s: can't open display, exiting...", APPNAME);
		exit (0);
	}

	theScreenNumber = DefaultScreen(theDisplay);

	toplevel = XtAppCreateShell (APPNAME, APPCLASS,
		applicationShellWidgetClass, theDisplay, NULL, 0);

	pattern = "*.nb *.ma *.m";

	n = 0;
	XtSetArg(warg[n], XmNpattern, pattern); n++;
#if SMARTMB
	guifb = XmCreateGUIFileBoxInSmartMB(toplevel, "_guifb", warg, n);
	XtManageChild(XtParent(guifb));
#else
	guifb = XmCreateGUIFileBox(toplevel, "guifb", warg, n);
	XtManageChild(guifb);
#endif /* SMARTMB */

	XtRealizeWidget(toplevel);

	XtAppMainLoop(app);
}
