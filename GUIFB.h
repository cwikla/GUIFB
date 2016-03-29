/* $Id: GUIFB.h,v 1.2 1996/02/11 22:05:17 cwikla Exp $ */

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

#ifndef _GUIFB_h
#define _GUIFB_h

#include <Xm/Xm.h>
#include <Xm/BulletinB.h>

#include "IconList.h"

#if FRONTEND
#define SUBPLUSS 1
#define SMARTMB 1
#endif /* FRONTEND */

extern WidgetClass xmGUIFileBoxWidgetClass;

typedef struct _XmGUIFileBoxRec *XmGUIFileBoxWidget;
typedef struct _XmGUIFileBoxConstraintRec *XmGUIFileBoxConstraint;

#ifndef XmIsGUIFileBox
#define XmIsGUIFileBox(a) (XtIsSubclass(a, xmGUIFileBoxWidgetClass))
#endif

#define XmNtwoColumn "twoColumn"
#define XmCTwoColumn "TwoColumn"

#define XmNworkWidget "workWidget"
#define XmCWorkWidget "WorkWidget"

#define XmNiconPairs "iconPairs"
#define XmCIconPairs "IconPairs"

#define XmNiconPairsCount "iconPairsCount"
#define XmCIconPairsCount "IconPairsCount"

#define XmNtitleWidget "titleWidget"
#define XmCTitleWidget "TitleWidget"

#define XmRGUIFBIconPairs "GUIFBIconPairs"
#define XmRGUIFBDialogMode "GUIFBDialogMode"

#define XmNdialogMode "dialogMode"
#define XmCDialogMode "DialogMode"

#define XmNupdateWrapperCallback "updateWrapperCallback"

#define XmNfileBoxDialogCallback "fileBoxDialogCallback"

#define XmGUIFB_STANDARD_MODE 1
#define XmGUIFB_ADVANCED_MODE 2

typedef struct _GUIFBIconPair
{
	IconListPixmap iconPixmap;
	const char *string;
	void *private;
} GUIFBIconPair;

typedef struct _XmGUIFileBoxCallbacKStruct
{
	int reason;
	XEvent *event;
	const char *directory;
	const char *file;
	const char *fullname;
} XmGUIFileBoxCallbackStruct;

extern Widget XmCreateGUIFileBox(Widget _parent, const char *_name, ArgList _warg, Cardinal _numWarg);
extern void XmGUIFileBoxSetMode(Widget _w, unsigned char _mode);
extern Widget XmCreateGUIFileBoxInSmartMB(Widget _parent, const char *_name, ArgList _args, Cardinal _numArgs);
extern void XmGUIFileBoxUpdate(Widget _w);
extern void XmGUIFileBoxSetParameters(Widget _w, const char *_directory, const char *_pattern,
				const char *_selection);
extern char *XmGUIFileBoxGetDirectory(Widget _w);
extern char *XmGUIFileBoxGetPattern(Widget _w);
extern Widget XmGUIFileBoxGetChild(Widget _w, int _child);


#ifndef XmUNSPECIFIED
#define XmUNSPECIFIED 0x2
#endif

#define XmDIALOG_FILES_LIST XmDIALOG_LIST
#define XmDIALOG_FILES_LIST_LABEL XmDIALOG_LIST_LABEL

enum {XmDIALOG_TITLE_LABEL=666, XmDIALOG_ADVANCED_BUTTON, XmDIALOG_STANDARD_BUTTON};

#define XmCR_UPDATE_WRAPPER_PRE 777
#define XmCR_UPDATE_WRAPPER_POST 778


#endif /* _GUIFB_h */
