/* $Id: GUIFBP.h,v 1.5 1996/02/13 22:00:39 cwikla Exp $ */
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

#ifndef _GUIFBP_h
#define _GUIFBP_h

#include <Xm/XmP.h>
#include <Xm/BulletinBP.h>

#include "GUIFB.h"
#include "IconList.h"

typedef struct _XmGUIFileBoxClassPart
{
	int blammo;
} XmGUIFileBoxClassPart;

typedef struct _XmGUIFileBoxClassRec
{
	CoreClassPart core_class;
	CompositeClassPart composite_class;
	ConstraintClassPart constraint_class;
	XmManagerClassPart manager_class;
	XmBulletinBoardClassPart bulletin_board_class;
	XmGUIFileBoxClassPart guiFileBox_class;
} XmGUIFileBoxClassRec, *XmGUIFileBoxWidgetClass;

enum { guiFBFilterLabel = 0, guiFBDirectoriesLabel, guiFBFilesLabel, guiFBSelectionLabel, guiFBLastLabel };

#define CHEEZY_HASH_SIZE 256

typedef struct _XmGUIFileBoxPart
{
	GUIFBIconPair *iconPairs;
	int iconPairsCount;
	unsigned char dialogMode;

	Boolean dirty;
	Boolean twoColumn;
	Boolean ignoreValueChange;
	Boolean manageChildLock;

	Pixmap defaultFolder;
	Pixmap defaultPage;
	Pixmap defaultUpFolder;

	const char **dirList;
	const char **fileList;

	IconListElement *iconElements;

	int upDirectoryRow;
	int dirTotal;
	int fileTotal;
	int currentSearchPos[2];

	String directoryString;
	String patternString;
	String selectionString;

	GUIFBIconPair  *cheezyHash[CHEEZY_HASH_SIZE];

	XtCallbackList activateCallback;
	XtCallbackList fileBoxDialogCallback;
	XtCallbackList okCallback;
	XtCallbackList cancelCallback;
	XtCallbackList helpCallback;

	XtCallbackList updateWrapperCallback;

	Widget okButtonWidget;
	Widget cancelButtonWidget;
	Widget helpButtonWidget;
	Widget filterButtonWidget;
	Widget advancedButtonWidget;
	Widget standardButtonWidget;

	Widget titleWidget;
	Widget separatorWidget;

	Widget optionWidget;
	Widget menuWidget;
	Widget listWidget[2];
	Widget textWidget;
	Widget filterWidget;
	Widget workWidget;

	Widget labelWidget[guiFBLastLabel];
} XmGUIFileBoxPart;

typedef struct _XmGUIFileBoxRec
{
	CorePart core;
	CompositePart	 composite;
	ConstraintPart constraint;
	XmManagerPart manager;
	XmBulletinBoardPart bulletin_board;
	XmGUIFileBoxPart guiFileBox;
} XmGUIFileBoxRec, *XmGUIFileBoxPtr;

#if NEED_BBCONSTRAINT
typedef struct _XmBulletinBoardConstraintPart
{
	char blammo;
} XmBulletinBoardConstraintPart;
#endif /* NEED_BBCONSTRAINT */


typedef struct _XmGUIFileBoxConstraintPart
{
	unsigned char childType;
} XmGUIFileBoxConstraintPart;

typedef struct _XmGUIFileBoxConstraintRec
{
	XmManagerConstraintPart manager;
	XmBulletinBoardConstraintPart bulletin_board;
	XmGUIFileBoxConstraintPart guiFileBox;
}	XmGUIFileBoxConstraintRec, *XmGUIFileBoxConstraintPtr;

#endif /* _GUIFBP_h */
