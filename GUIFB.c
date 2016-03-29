/* $Id: GUIFB.c,v 1.11 1996/03/04 17:28:06 cwikla Exp $ */
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
 * prior permission.	John L. Cwikla and Wolfram Research, Inc make no
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
#include <X11/CoreP.h>

#include <stdio.h>
#include <unistd.h>

#if XPM
#include <X11/xpm.h>
#endif /* XPM */

#include <Xm/Xm.h>
#include <Xm/BulletinBP.h>
#include <Xm/TextF.h>
#include <Xm/SeparatoG.h>
#include <Xm/Label.h>
#include <Xm/XmosP.h>

#include <Xm/PushBG.h>

#include "IconList.h"

#include "GUIFBP.h"
#include "xreg.h"
#include "guifbff.h"

#if SMARTMB
#include "SmartMB.h"
#endif

#if SUBPLUSS
#include "SubPlusS.h"

#define CREATE_OPTION_MENU(a,b,c,d) XmPlusCreateOptionMenu(a,b,c,d)
#define CREATE_PULLDOWN_MENU(a,b,c,d) XmPlusCreatePulldownMenu(a,b,c,d)

#else

#include <Xm/RowColumn.h>
#include <Xm/MenuShell.h>

#define CREATE_OPTION_MENU(a,b,c,d) XmCreateOptionMenu(a,b,c,d)
#define CREATE_PULLDOWN_MENU(a,b,c,d) XmCreatePulldownMenu(a,b,c,d)

#endif /* SUBPLUSS */


#define managerTranslations _XmBulletinB_defaultTranslations
extern void _XmBulletinBoardSetDefaultShadow(); /* Why rewrite? */

#define CORE(a) ((a)->core)
#define WIDTH(a) ((a)->core.width)
#define HEIGHT(a) ((a)->core.height)
#define COMP(a) ((a)->composite)
#define MANAGER(a) ((a)->manager)
#define GUIFB(a) ((a)->guiFileBox)

#define MENU(a) ((a)->guiFileBox.menuWidget)
#define OPTION(a) ((a)->guiFileBox.optionWidget)
#define LIST1(a) ((a)->guiFileBox.listWidget[0])
#define LIST2(a) ((a)->guiFileBox.listWidget[1])
#define FILTER(a) ((a)->guiFileBox.filterWidget)
#define LABELS(a) ((a)->guiFileBox.labelWidget)
#define FILTER_LABEL(a) ((a)->guiFileBox.labelWidget[guiFBFilterLabel])
#define DIRECTORIES_LABEL(a) ((a)->guiFileBox.labelWidget[guiFBDirectoriesLabel])
#define FILES_LABEL(a) ((a)->guiFileBox.labelWidget[guiFBFilesLabel])
#define SELECTION_LABEL(a) ((a)->guiFileBox.labelWidget[guiFBSelectionLabel])
#define TEXTF(a) ((a)->guiFileBox.textWidget)
#define WORK(a) ((a)->guiFileBox.workWidget)
#define ICON_PAIRS(a) ((a)->guiFileBox.iconPairs)
#define NUM_ICON_PAIRS(a) ((a)->guiFileBox.iconPairsCount)
#define TITLE(a) ((a)->guiFileBox.titleWidget)
#define SEP(a) ((a)->guiFileBox.separatorWidget)
#define MODE(a) ((a)->guiFileBox.dialogMode)

#define OK_BUTTON(a) ((a)->guiFileBox.okButtonWidget)
#define CANCEL_BUTTON(a) ((a)->guiFileBox.cancelButtonWidget)
#define FILTER_BUTTON(a) ((a)->guiFileBox.filterButtonWidget)
#define ADVANCED_BUTTON(a) ((a)->guiFileBox.advancedButtonWidget)
#define STANDARD_BUTTON(a) ((a)->guiFileBox.standardButtonWidget)
#define HELP_BUTTON(a) ((a)->guiFileBox.helpButtonWidget)

#define DLIST(a) ((a)->guiFileBox.dirList)
#define FLIST(a) ((a)->guiFileBox.fileList)
#define DTOTAL(a) ((a)->guiFileBox.dirTotal)
#define FTOTAL(a) ((a)->guiFileBox.fileTotal)

#define CHEEZY_HASH(a) ((a)->guiFileBox.cheezyHash)

#define UP_DIR(a) ((a)->guiFileBox.upDirectoryRow)

#define DEF_FOLDER(a) ((a)->guiFileBox.defaultFolder)
#define DEF_PAGE(a) ((a)->guiFileBox.defaultPage)
#define DEF_UP_FOLDER(a) ((a)->guiFileBox.defaultUpFolder)

#define ELEMENTS(a) ((a)->guiFileBox.iconElements)

#define MCLASS(a) ((a)->guiFileBox_class)

#define FBDIR(a) ((a)->guiFileBox.directoryString)
#define PATTERN(a) ((a)->guiFileBox.patternString)
#define SELECTION(a) ((a)->guiFileBox.selectionString)

#define MANAGE_LOCK(a) ((a)->guiFileBox.manageChildLock)

#define LIST1_SEARCH_POS(a) ((a)->guiFileBox.currentSearchPos[0])
#define LIST2_SEARCH_POS(a) ((a)->guiFileBox.currentSearchPos[1])

#define IGNORE_TFV(a) ((a)->guiFileBox.ignoreValueChange)

#define NAME_HAS_DIR(a) (strchr((a), '/'))

#define GOOD(a) ((a) && XtIsManaged(a))

#define MAX(a,b) (((a) > (b)) ? (a) : (b))

#if XmVersion >= 2000
extern void _XmSeparatorFix(XmGeoMatrix,int,XmGeoMajorLayout,XmKidGeometry);
#endif

static void classInit(void);
static void initialize(XmGUIFileBoxWidget _request, XmGUIFileBoxWidget _new, String *_args, Cardinal *_numArgs);
static Boolean setValues(XmGUIFileBoxWidget _current, XmGUIFileBoxWidget _request, XmGUIFileBoxWidget _new);
static void destroy(XmGUIFileBoxWidget _fbw);
static void changeManaged(XmGUIFileBoxWidget _fbw);
static void rectifyDirectory(XmGUIFileBoxWidget _fbw);

static Boolean noGeoRequest(XmGeoMatrix _geoSpec);
static XmGeoMatrix geoMatrixCreate(Widget _w, Widget _from, XtWidgetGeometry *_pref);
#if 0
static Boolean cvtStringToGUIFBIconPairs(Display *_display, XrmValuePtr _args,
	Cardinal *_numArgs, XrmValuePtr _from, XrmValuePtr _to, XtPointer *_data);
#endif
static Boolean cvtStringToGUIFBDialogMode(Display *_display, XrmValuePtr _args,
	Cardinal *_numArgs, XrmValuePtr _from, XrmValuePtr _to, XtPointer *_data);
static void updateFileList(XmGUIFileBoxWidget _fbw, const char *_dir, const char *_compete, const char *_pattern);
static void activate(Widget _w, XtPointer _client, XtPointer _call);
static void filter(Widget _w, XtPointer _client, XtPointer _call);
static void updateFileTextField(Widget _w, XtPointer _client, XtPointer _call);
static void textFieldSearch(Widget _w, XtPointer _client, XtPointer _call);
static void resetCurrentSearchPos(Widget _w, XtPointer _client, XtPointer _call);
static void updatePattern(XmGUIFileBoxWidget _fbw, const char *_string, Boolean _free);
static void updateSelection(XmGUIFileBoxWidget _fbw, const char *_dir, const char *_string, Boolean _free);

static void _TitleFix(XmGeoMatrix _geoSpec, int _action, XmGeoMajorLayout _layoutPtr, XmKidGeometry _rowPtr);

static void completeSelection(Widget _w, XEvent *_event, String *_args, Cardinal *_numArgs);
static void scrollToFirstFile(Widget _w, XEvent *_event, String *_args, Cardinal *_numArgs);

static XtActionsRec actions[] =
{
	{"GUIFileBoxCompleteSelection", (XtActionProc)completeSelection},
	{"GUIFileBoxScrollToFirstFile", (XtActionProc)scrollToFirstFile},
};

#define TheOffset(field) XtOffset(XmGUIFileBoxWidget, guiFileBox.field)
static XtResource resources[] =
{
	{XmNtitleWidget, XmCTitleWidget, XtRWidget, sizeof(Widget),
		TheOffset(titleWidget), XtRImmediate, (XtPointer)NULL},
	{XmNworkWidget, XmCWorkWidget, XtRWidget, sizeof(Widget),
		TheOffset(workWidget), XtRImmediate, (XtPointer)NULL},
	{XmNiconPairs, XmCIconPairs, XmRGUIFBIconPairs, sizeof(XtPointer),
		TheOffset(iconPairs), XtRImmediate, (XtPointer)NULL},
	{XmNiconPairsCount, XmCIconPairsCount, XtRInt, sizeof(int),
		TheOffset(iconPairsCount), XtRImmediate, (XtPointer)0},
	{XmNdirectory, XmCDirectory, XtRString, sizeof(String),
		TheOffset(directoryString), XtRImmediate, (XtPointer)XmUNSPECIFIED},
	{XmNpattern, XmCPattern, XtRString, sizeof(String),
		TheOffset(patternString), XtRImmediate, (XtPointer)NULL},
	{XmNactivateCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
		TheOffset(activateCallback), XtRImmediate, (XtPointer)NULL},
	{XmNdialogMode, XmCDialogMode, XmRGUIFBDialogMode, sizeof(unsigned char),
		TheOffset(dialogMode), XtRImmediate, (XtPointer)XmGUIFB_STANDARD_MODE},
	{XmNfileBoxDialogCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
		TheOffset(fileBoxDialogCallback), XtRImmediate, (XtPointer)NULL},
	{XmNokCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
		TheOffset(okCallback), XtRImmediate, (XtPointer)NULL},
	{XmNcancelCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
		TheOffset(cancelCallback), XtRImmediate, (XtPointer)NULL},
	{XmNhelpCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
		TheOffset(helpCallback), XtRImmediate, (XtPointer)NULL},
	{XmNupdateWrapperCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
		TheOffset(updateWrapperCallback), XmRImmediate, (XtPointer)NULL},
};

#undef TheOffset

#if 0
#define TheOffset(field) XtOffset(XmGUIFileBoxConstraint, guiFileBox.field)
static XtResource constraintResources[] =
{
};
#undef TheOffset
#endif

#include "page.xbm"
#include "folder.xbm"
#include "upfolder.xbm"

XmGUIFileBoxClassRec xmGUIFileBoxClassRec =
{
	{
		(WidgetClass)&xmBulletinBoardClassRec,				/* superclass */
		"XmGUIFileBox",								 /* class_name */
		(Cardinal)sizeof(XmGUIFileBoxRec),						/* widget size */
		(XtProc)classInit,										/* class_init */
		(XtWidgetClassProc)NULL,										/* class_part_init */
		(XtEnum)FALSE,										 /* class_inited */
		(XtInitProc)initialize,									/* initialize */
		(XtArgsProc)NULL,										/* init_hook */
		XtInheritRealize,							/* realize */
		(XtActionList)actions,										/* actions */
		(Cardinal)XtNumber(actions),											 /* num_actions */
		(XtResourceList)resources,									 /* resources */
		(Cardinal)XtNumber(resources),						 /* num_resources */
		NULLQUARK,									 /* xrm_class */
		FALSE,										 /* compress_motion */
		(XtEnum)FALSE,										 /* compress_exposur */
		FALSE,										 /* compress enterleave */
		FALSE,										/* visibility_interest */
		(XtWidgetProc)destroy,										/* destroy */
		(XtWidgetProc)XtInheritResize,
		XtInheritExpose,
		(XtSetValuesFunc)setValues,									 /* set_values */
		(XtArgsFunc)NULL,										/* set_values_hook */
		XtInheritSetValuesAlmost,					/* set_values_almost */
		(XtArgsProc)NULL,										/* get_values_hook */
		XtInheritAcceptFocus,						/* accept_focus */
		XtVersion,									 /* version */
		(XtPointer)NULL,										/* callback_private */
		XtInheritTranslations,
		(XtGeometryHandler)XtInheritQueryGeometry,			/* query_geometry */
		XtInheritDisplayAccelerator,				 /* display_accelerator */
		(XtPointer)NULL,										/* extension */
	},
	{ /* Composite Part */
		(XtGeometryHandler)XtInheritGeometryManager,							 /* geometry_manager */
		(XtWidgetProc)changeManaged,								 /* change_managed */
		(XtWidgetProc)XtInheritInsertChild, /*insertChild,						/* inherit_child */
		(XtWidgetProc)XtInheritDeleteChild, /* deleteChild,						/* delete_child */
		NULL,										/* extension */
	},
	{	/* Constraint Part */
		(XtResourceList)NULL, /* constraintResources, */
		0, /* XtNumber(constraintResources), */
		(Cardinal)0, /* sizeof(XmGUIFileBoxConstraintRec), */
		(XtInitProc)NULL,
		(XtWidgetProc)NULL,
		(XtSetValuesFunc)NULL, /* constraintSetValues, */
		(XtPointer)NULL,
	},
	{	/* XmManager Part */
		XmInheritTranslations,
		NULL,
		0,	
		NULL,
		0,	
		XmInheritParentProcess,
		NULL,
	},
	{ /* XmBulletinBoard Part */
		FALSE,
		geoMatrixCreate,
		XmInheritFocusMovedProc,	
		NULL,
	},
	{ /* XmGUIFileBox Part */
		0,
	},
};

WidgetClass xmGUIFileBoxWidgetClass = (WidgetClass)&xmGUIFileBoxClassRec;


static void classInit(void)
{
#if 0
	XtSetTypeConverter(XtRString, XmRGUIFBIconPairs, cvtStringToGUIFBIconPairs,
		(XtConvertArgList)NULL, 0,
			XtCacheAll, (XtDestructor)NULL);
#endif
   XtSetTypeConverter(XtRString, XmRGUIFBDialogMode, cvtStringToGUIFBDialogMode,
		(XtConvertArgList)NULL, 0,
			XtCacheAll, (XtDestructor)NULL);
}

static void initialize(XmGUIFileBoxWidget _request, XmGUIFileBoxWidget _new, String *_args, Cardinal *_numArgs)
{
	char *ptr;
	Arg warg[7];
	int n, i;
	unsigned char oldMode;
	Dimension maxWidth, maxHeight;

	ELEMENTS(_new) = NULL;
	IGNORE_TFV(_new) = FALSE;
	MANAGE_LOCK(_new) = FALSE;
	DLIST(_new) = NULL;
	FLIST(_new) = NULL;
	DTOTAL(_new) = 0;
	FTOTAL(_new) = 0;
	OK_BUTTON(_new)= NULL;
	CANCEL_BUTTON(_new)= NULL;
	FILTER_BUTTON(_new)= NULL;
	ADVANCED_BUTTON(_new)= NULL;
	STANDARD_BUTTON(_new)= NULL;
	HELP_BUTTON(_new)= NULL;
	LIST1_SEARCH_POS(_new) = 0;
	LIST2_SEARCH_POS(_new) = 0;
	SELECTION(_new) = NULL;

	for(i=0;i<CHEEZY_HASH_SIZE;i++)
		CHEEZY_HASH(_new)[i] = NULL;

	for(i=0;i<NUM_ICON_PAIRS(_new);i++)
	{
		ptr = strrchr(ICON_PAIRS(_new)[i].string, '.');
		if (ptr)
		{
			ptr++;
			ICON_PAIRS(_new)[i].private = CHEEZY_HASH(_new)[(int)(*ptr)];
			CHEEZY_HASH(_new)[(int)(*ptr)] = ICON_PAIRS(_new)+i;
		}
	}

	n = 0;
	XtSetArg(warg[n], XmNnavigationType, XmEXCLUSIVE_TAB_GROUP); n++;
	TITLE(_new) = XtCreateWidget("_title", xmLabelWidgetClass, (Widget)_new, warg, n);

	n = 0;
	XtSetArg(warg[n], XmNnavigationType, XmEXCLUSIVE_TAB_GROUP); n++;
	OPTION(_new) = CREATE_OPTION_MENU((Widget)_new, "_option", warg, n);
	MENU(_new) = CREATE_PULLDOWN_MENU((Widget)_new, "_menu", NULL, 0);

	n = 0;
	XtSetArg(warg[n], XmNsubMenuId, MENU(_new)); n++;
	XtSetValues(OPTION(_new), warg, n);
	XtManageChild(OPTION(_new));

	n = 0;
	XtSetArg(warg[n], XmNnavigationType, XmEXCLUSIVE_TAB_GROUP); n++;
	FILTER_LABEL(_new) = XtCreateManagedWidget("Filter", xmLabelWidgetClass, (Widget)_new, warg, n);
	DIRECTORIES_LABEL(_new) = XtCreateManagedWidget("Directories", xmLabelWidgetClass, (Widget)_new, warg, n);
	FILES_LABEL(_new) = XtCreateManagedWidget("Files", xmLabelWidgetClass, (Widget)_new, warg, n);
	SELECTION_LABEL(_new) = XtCreateManagedWidget("Selection", xmLabelWidgetClass, (Widget)_new, warg, n);

	n = 0;
	XtSetArg(warg[n], XmNnavigationType, XmEXCLUSIVE_TAB_GROUP); n++;
	XtSetArg(warg[n], XmNeditMode, XmSINGLE_LINE); n++;
	XtSetArg(warg[n], XmNrows, 1); n++;
	FILTER(_new) = XtCreateWidget("_filter", xmTextFieldWidgetClass, (Widget)_new, warg, n);
	
	UP_DIR(_new) = -1;


	n = 0;
	XtSetArg(warg[n], XmNorientation, XmHORIZONTAL); n++;
	SEP(_new) = XtCreateManagedWidget("_separator", xmSeparatorGadgetClass, (Widget)_new, warg, n);

	maxWidth = MAX(folder_width, page_width);
	maxWidth = MAX(maxWidth, upfolder_width);
	maxHeight = MAX(folder_height, page_height);
	maxHeight = MAX(maxHeight, upfolder_height);

	n = 0;
	XtSetArg(warg[n], XmNiconWidth, maxWidth); n++;
	XtSetArg(warg[n], XmNiconHeight, maxHeight); n++;
	XtSetArg(warg[n], XmNusingBitmaps, TRUE); n++;
	XtSetArg(warg[n], XmNnavigationType, XmEXCLUSIVE_TAB_GROUP); n++;
	LIST1(_new) = XmCreateScrolledIconList((Widget)_new,  "_list1", warg, n);
	LIST2(_new) = XmCreateScrolledIconList((Widget)_new,  "_list2", warg, n);


	n = 0;
	XtSetArg(warg[n], XmNnavigationType, XmEXCLUSIVE_TAB_GROUP); n++;
	TEXTF(_new) = XtCreateManagedWidget("_textField", xmTextFieldWidgetClass, (Widget)_new, warg, n);

	DEF_FOLDER(_new) = XCreateBitmapFromData(XtDisplay((Widget)_new), 
							RootWindowOfScreen(XtScreen(_new)),
							folder_bits, folder_width, folder_height);

	DEF_PAGE(_new) = XCreateBitmapFromData(XtDisplay((Widget)_new),
							RootWindowOfScreen(XtScreen(_new)),
							page_bits, page_width, page_height);

	DEF_UP_FOLDER(_new) = XCreateBitmapFromData(XtDisplay((Widget)_new),
							RootWindowOfScreen(XtScreen(_new)),
							upfolder_bits, upfolder_width, upfolder_height);

	rectifyDirectory(_new);

	if (PATTERN(_new))
		updatePattern(_new, PATTERN(_new), FALSE);

	if (SELECTION(_new))
		updateSelection(_new, FBDIR(_new), SELECTION(_new), FALSE);

	updateFileList(_new, FBDIR(_new), NULL, PATTERN(_new));

	oldMode = MODE(_new);
	MODE(_new) = -1;
	XmGUIFileBoxSetMode((Widget)_new, oldMode);

	XtAddCallback(LIST1(_new), XmNvalueChangedCallback, (XtCallbackProc)updateFileTextField, (XtPointer)_new);
	XtAddCallback(LIST2(_new), XmNvalueChangedCallback, (XtCallbackProc)updateFileTextField, (XtPointer)_new);

	XtAddCallback(LIST1(_new), XmNactivateCallback, (XtCallbackProc)activate, (XtPointer)_new);
	XtAddCallback(LIST2(_new), XmNactivateCallback, (XtCallbackProc)activate, (XtPointer)_new);
	XtAddCallback(TEXTF(_new), XmNactivateCallback, (XtCallbackProc)activate, (XtPointer)_new);

	XtAddCallback(FILTER(_new), XmNactivateCallback, (XtCallbackProc)filter, (XtPointer)_new);

	XtAddCallback(TEXTF(_new), XmNvalueChangedCallback, (XtCallbackProc)textFieldSearch, (XtPointer)_new);
	XtAddCallback(TEXTF(_new), XmNfocusCallback, (XtCallbackProc)resetCurrentSearchPos, (XtPointer)_new);
}

static void updateSelection(XmGUIFileBoxWidget _fbw, const char *_dir, const char *_string, Boolean _free)
{
	char buffer[MAX_PATH_NAME_LENGTH];
	char *ptr;

	if (_string && (_string != SELECTION(_fbw)))
	{
		if (NAME_HAS_DIR(_string))
		{
			if (_string[0] == '/')
			{
				strcpy(buffer, _string);
				ptr = strrchr(buffer, '/');
				ptr++;
				*ptr = 0;

				if (FBDIR(_fbw) && _free)
					XtFree((void *)FBDIR(_fbw));
				FBDIR(_fbw) = XtNewString(buffer);

				ptr = strrchr(_string, '/');
				ptr++;

				if (SELECTION(_fbw) && _free)
					XtFree((void *)SELECTION(_fbw));

				if (strlen(ptr))
					SELECTION(_fbw) = XtNewString(ptr);
				else
					SELECTION(_fbw) = NULL;
			}
		}
		else
		{
			if (SELECTION(_fbw) && _free)
				XtFree((void *)SELECTION(_fbw));
			SELECTION(_fbw) = XtNewString(_string);
		}
	}
	else
	if (!_string && SELECTION(_fbw))
	{
		XtFree((void *)SELECTION(_fbw));
		SELECTION(_fbw) = NULL;
	}

	if (_dir != FBDIR(_fbw))
	{
		if (_string && !NAME_HAS_DIR(_string))
		{
			if (FBDIR(_fbw) && _free)
				XtFree((void *)FBDIR(_fbw));
			FBDIR(_fbw) = XtNewString(_dir);
		}
	}

	strcpy(buffer, FBDIR(_fbw));
	if (buffer[strlen(FBDIR(_fbw))-1] != '/')
		strcat(buffer, "/");
	if (SELECTION(_fbw))
		strcat(buffer, SELECTION(_fbw));

	IGNORE_TFV(_fbw) = TRUE;
	XmTextFieldSetString(TEXTF(_fbw), buffer);
	XmTextFieldSetCursorPosition(TEXTF(_fbw), 
					  XmTextFieldGetLastPosition(TEXTF(_fbw)));
	IGNORE_TFV(_fbw) = FALSE;
}

static void updatePattern(XmGUIFileBoxWidget _fbw, const char *_string, Boolean _free)
{
	if (PATTERN(_fbw) && _free)
		XtFree((void *)PATTERN(_fbw));
	if (_string)
		PATTERN(_fbw) = XtNewString(_string);
	else
		PATTERN(_fbw) = NULL;
		
	XmTextFieldSetString(FILTER(_fbw), _string ? (char *)_string : "");
	XmTextFieldSetCursorPosition(FILTER(_fbw),
					  XmTextFieldGetLastPosition(FILTER(_fbw)));
}

static void getFullFilename(const char *_dir, const char *_path, char *_buffer)
{
	if (_path && ((_path[0] == '/') || !_dir))
		strcpy(_buffer, _path);
	else
	if (_dir)
	{
		strcpy(_buffer, _dir);
		if (_path)
		{
			if (_dir[strlen(_dir)-1] != '/')
				strcat(_buffer, "/");
			strcat(_buffer, _path);
		}
	}
	else
		_buffer[0] = 0;
}

static void menuCB(Widget _w, XtPointer _data, XtPointer _call)
{
	XmGUIFileBoxWidget fbw;
	CompositeWidget cw;
	int i;
	char *ptr;

	cw = (CompositeWidget)XtParent(_w);
	fbw = (XmGUIFileBoxWidget)_data;
	for(i=0;i<COMP(cw).num_children;i++)
	{
		if (_w == COMP(cw).children[i])
			break;
	}

	if (i >= (COMP(cw).num_children-1))
		return;

	ptr = FBDIR(fbw);
	while(i>=0)
	{
		ptr = strchr(ptr, '/');
		ptr++;
		i--;
	}
	*ptr = 0;

	updateFileList(fbw, FBDIR(fbw), NULL, PATTERN(fbw));
}

static void menuAdjust(XmGUIFileBoxWidget _fbw)
{
	Widget button;
	int oldChildCount, i, done;
	char oldChar;
	CompositeWidget cw;
	char buffer[MAX_PATH_NAME_LENGTH];
	char *ptr, *thisPtr;
	Arg warg[2];
	int n;
	XmString xmstring;

	cw = (CompositeWidget)MENU(_fbw);
	oldChildCount = COMP(cw).num_children;

	strcpy(buffer, FBDIR(_fbw));

	done = 0;
	thisPtr = buffer;
	ptr = buffer;
	while(!done)
	{
		ptr = strchr(ptr, '/');
		if (ptr)
		{
			oldChar = *(ptr+1);
			*(ptr+1) = 0;
			xmstring = XmStringCreateSimple(thisPtr);
			ptr++;
			*ptr = oldChar;
			thisPtr = ptr;

			n = 0;
			XtSetArg(warg[n], XmNlabelString, xmstring); n++;
			button = XtCreateWidget("_button", xmPushButtonGadgetClass, MENU(_fbw), warg, n);
		}
		else
		{
			done = 1;
			if (*thisPtr)
			{
				xmstring = XmStringCreateSimple(thisPtr);
				n = 0;
				XtSetArg(warg[n], XmNlabelString, xmstring); n++;
				button = XtCreateWidget("_button", xmPushButtonGadgetClass, MENU(_fbw), warg, n);
			}
			else
				continue;
		}
		XmStringFree(xmstring);
		XtAddCallback(button, XmNactivateCallback, (XtCallbackProc)menuCB, (XtPointer)_fbw);
	}

	n = 0;
	XtSetArg(warg[n], XmNmenuHistory, COMP(cw).children[COMP(cw).num_children-1]); n++;
	XtSetValues(MENU(_fbw), warg, n);

	XtManageChildren(COMP(cw).children+oldChildCount, COMP(cw).num_children-oldChildCount);
	XtUnmanageChildren(COMP(cw).children, oldChildCount);

	for(i=0;i<oldChildCount;i++)
		XtDestroyWidget(COMP(cw).children[i]);
}

static void updateFileList(XmGUIFileBoxWidget _fbw, const char *_dir, const char *_completion, const char *_pattern)
{
	IconListElement *thisElement;
	int i, status;
	char *bestMatch, *ptr;
	const char **newDirList, **newFileList;
	int newDirCount, newFileCount;
	Boolean found;
	XmGUIFileBoxCallbackStruct fbcs;

	if (XtHasCallbacks((Widget)_fbw, XmNupdateWrapperCallback))
	{
		fbcs.reason = XmCR_UPDATE_WRAPPER_PRE;
		fbcs.event = NULL;
		fbcs.directory = NULL;
		fbcs.file = NULL;
		fbcs.fullname = NULL;
		XtCallCallbacks((Widget)_fbw, XmNupdateWrapperCallback, (XtPointer)&fbcs);
	}

	status = GUIFBFindMatchedFiles(_dir, _completion, _pattern, TRUE, TRUE, FALSE, 
					&bestMatch, &newDirList, &newFileList,
					&newDirCount, &newFileCount);

	if (status == GUIFB_ERROR)
	{
		GUIFBFreeFileLists(DLIST(_fbw), FLIST(_fbw), DTOTAL(_fbw), FTOTAL(_fbw));
		DLIST(_fbw) = NULL;
        FLIST(_fbw) = NULL;
        DTOTAL(_fbw) = 0;
        FTOTAL(_fbw) = 0;
		XBell(XtDisplay(_fbw), 0);
	}
	else
	{
		GUIFBFreeFileLists(DLIST(_fbw), FLIST(_fbw), DTOTAL(_fbw), FTOTAL(_fbw));
		DLIST(_fbw) = newDirList;
		FLIST(_fbw) = newFileList;
		DTOTAL(_fbw) = newDirCount;
		FTOTAL(_fbw) = newFileCount;

		if (bestMatch)
		{
#if 0
			if (_subdir && (_subdir[0] != '/'))
			{
				int blen;
				blen = strlen(bestMatch);
				bestMatch[blen-strlen(_subdir)] = 0;
			}
#endif
			if (FBDIR(_fbw))
				free(FBDIR(_fbw));
			FBDIR(_fbw) = bestMatch;
		}

		menuAdjust(_fbw);
	
		if (ELEMENTS(_fbw))
			XtFree((void *)ELEMENTS(_fbw));

		ELEMENTS(_fbw) = (IconListElement *)XtMalloc(sizeof(IconListElement) * (DTOTAL(_fbw)+FTOTAL(_fbw)));
		thisElement = ELEMENTS(_fbw);
	
		UP_DIR(_fbw) = -1;
		for(i=0;i<DTOTAL(_fbw);i++)
		{
			if (strcmp(DLIST(_fbw)[i], "..") == 0)
			{
				UP_DIR(_fbw) = i;
				thisElement->iconPixmap.pixmap = DEF_UP_FOLDER(_fbw);
				thisElement->iconPixmap.width = upfolder_width;
				thisElement->iconPixmap.height = upfolder_height;
			}
			else
			{
				thisElement->iconPixmap.pixmap = DEF_FOLDER(_fbw);
				thisElement->iconPixmap.width = folder_width;
				thisElement->iconPixmap.height = folder_height;
			}
			thisElement->iconPixmap.mask = None;
			thisElement->iconPixmap.isBitmap = TRUE;
			thisElement++->string = DLIST(_fbw)[i];
		}
		for(i=0;i<FTOTAL(_fbw);i++)
		{
			thisElement->string = FLIST(_fbw)[i];	
			found = FALSE;
			if (NUM_ICON_PAIRS(_fbw))
			{
				GUIFBIconPair *thisHash;
				ptr = strrchr(FLIST(_fbw)[i], '.');
				if ( ptr)
				{
					thisHash = CHEEZY_HASH(_fbw)[(int)(*(ptr+1))];
					while(!found && (thisHash != NULL))
						if (strcmp(thisHash->string, ptr) == 0)
						{
							thisElement->iconPixmap = thisHash->iconPixmap;
							found = TRUE;
						}
						else
							thisHash = (GUIFBIconPair *)thisHash->private;
				}
			}
			if (!found)
			{
				thisElement->iconPixmap.pixmap = DEF_PAGE(_fbw);
				thisElement->iconPixmap.mask = None;
				thisElement->iconPixmap.isBitmap = TRUE;
				thisElement->iconPixmap.width = page_width;
				thisElement->iconPixmap.height = page_height;
			}
			thisElement++;
		}
	}

	if (MODE(_fbw) == XmGUIFB_ADVANCED_MODE)
	{
		if (GOOD(LIST1(_fbw)))
			XmIconListSetItems(LIST1(_fbw), ELEMENTS(_fbw), DTOTAL(_fbw));
		if (GOOD(LIST2(_fbw)))
			XmIconListSetItems(LIST2(_fbw), ELEMENTS(_fbw)+DTOTAL(_fbw), FTOTAL(_fbw));
	}
	else
	if (GOOD(LIST1(_fbw)))
		XmIconListSetItems(LIST1(_fbw), ELEMENTS(_fbw), DTOTAL(_fbw)+FTOTAL(_fbw));
	
	LIST1_SEARCH_POS(_fbw) = 0;
	LIST2_SEARCH_POS(_fbw) = DTOTAL(_fbw);

	if (XtHasCallbacks((Widget)_fbw, XmNupdateWrapperCallback))
	{
		fbcs.reason = XmCR_UPDATE_WRAPPER_POST;
		XtCallCallbacks((Widget)_fbw, XmNupdateWrapperCallback, (XtPointer)&fbcs);
	}
}

static void changeManaged(XmGUIFileBoxWidget _fbw)
{
	if (MANAGE_LOCK(_fbw))
		return;

	MANAGE_LOCK(_fbw) = TRUE;
	if (GOOD(TITLE(_fbw)) && !GOOD(SEP(_fbw)))
		XtManageChild(SEP(_fbw));
	else
	if (!GOOD(TITLE(_fbw)) && GOOD(SEP(_fbw)))
		XtUnmanageChild(SEP(_fbw));
	MANAGE_LOCK(_fbw) = FALSE;

   	(*((CompositeWidgetClass)xmGUIFileBoxWidgetClass->core_class.superclass)->composite_class.change_managed)((Widget)_fbw);
}

static void destroy(XmGUIFileBoxWidget _fbw)
{
}

static Boolean setValues(XmGUIFileBoxWidget _current, XmGUIFileBoxWidget _request, XmGUIFileBoxWidget _new)
{
	Boolean redo = FALSE;

	BB_InSetValues(_current) = TRUE;

/* do stuff here */

	BB_InSetValues(_current) = FALSE;

	if(redo && (XtClass(_new) == xmGUIFileBoxWidgetClass))
	{
		_XmBulletinBoardSizeUpdate( (Widget) _new);
		return FALSE;
	}

	return redo;
}

static XmGeoMatrix geoMatrixCreate(Widget _w, Widget _from, XtWidgetGeometry *_pref)
{
	XmGUIFileBoxWidget fbw = (XmGUIFileBoxWidget)_w;
	XmGeoMatrix geoSpec;
	register XmGeoRowLayout	layoutPtr;
	register XmKidGeometry boxPtr;
	Cardinal numKids;
	Boolean newRow;
	int nrows;

	nrows = 0;

	numKids = COMP(fbw).num_children;

	if (GOOD(TITLE(fbw)))
		nrows++;

	if (GOOD(FILTER_LABEL(fbw)))
		nrows++;

	if (GOOD(FILTER(fbw)))
		nrows++;

	if (GOOD(DIRECTORIES_LABEL(fbw)) || GOOD(FILES_LABEL(fbw)))
		nrows++;

	if (GOOD(SEP(fbw)))
		nrows++;

	if (GOOD(OPTION(fbw)))
		nrows++;

	if (GOOD(LIST1(fbw)) || (GOOD(LIST2(fbw))))
		nrows++;

	if (GOOD(WORK(fbw)))
		nrows++;

	if (GOOD(SELECTION_LABEL(fbw)))
		nrows++;

	if (GOOD(TEXTF(fbw)))
		nrows++;

	geoSpec = _XmGeoMatrixAlloc(nrows, numKids, 0);
	geoSpec->composite = (Widget)fbw;
	geoSpec->instigator = (Widget)_from;
	if (_pref)
		geoSpec->instig_request = *_pref;
	geoSpec->margin_w = 0; /* BB_MarginWidth(fbw) + MANAGER(fbw).shadow_thickness; */
	geoSpec->margin_h = 0; /* BB_MarginHeight(fbw) + MANAGER(fbw).shadow_thickness; */
	geoSpec->no_geo_request = noGeoRequest;

	layoutPtr = &(geoSpec->layouts->row);
	boxPtr = geoSpec->boxes;

	if (GOOD(TITLE(fbw)) && _XmGeoSetupKid(boxPtr, TITLE(fbw)))
	{
		layoutPtr->fill_mode = XmGEO_PACK;
		layoutPtr->stretch_height = 0;
		layoutPtr->space_above = 0; /* BB_MarginHeight(fbw); */
		layoutPtr++;
		boxPtr += 2;
	}

	if( GOOD(SEP(fbw)) && _XmGeoSetupKid( boxPtr, SEP(fbw)))
	{
		layoutPtr->fix_up = _XmSeparatorFix;
		layoutPtr->space_above = BB_MarginHeight(fbw);
		boxPtr += 2;
		layoutPtr++;
	}

	if ( GOOD(OPTION(fbw)) && _XmGeoSetupKid(boxPtr, OPTION(fbw)))
	{
		layoutPtr->fill_mode = XmGEO_CENTER;
		/* layoutPtr->fit_mode = XmGEO_AVERAGING; */
		/* layoutPtr->even_width = 1; */
		/* layoutPtr->even_height = 1; */
		layoutPtr->space_above = BB_MarginHeight(fbw);
		layoutPtr++;
		boxPtr += 2;
	}

	if (GOOD(FILTER_LABEL(fbw)) && _XmGeoSetupKid(boxPtr, FILTER_LABEL(fbw)))
	{
		layoutPtr->fill_mode = XmGEO_EXPAND;
		layoutPtr->even_width = 0;
		layoutPtr->even_height = 1;
		layoutPtr->stretch_height = 0;
		layoutPtr->space_above = BB_MarginHeight(fbw);
		layoutPtr++;
		boxPtr += 2;
	}

	if (GOOD(FILTER(fbw)) && _XmGeoSetupKid(boxPtr, FILTER(fbw)))
	{
		layoutPtr->fill_mode = XmGEO_EXPAND;
		layoutPtr->space_above = 0; /* BB_MarginHeight(fbw); */
		layoutPtr++;
		boxPtr += 2;
	}

	newRow = FALSE;
	if (GOOD(DIRECTORIES_LABEL(fbw)) && _XmGeoSetupKid(boxPtr, DIRECTORIES_LABEL(fbw)))
	{
		layoutPtr->fill_mode = XmGEO_EXPAND;
		layoutPtr->fit_mode = XmGEO_AVERAGING;
		layoutPtr->even_width = 1;
		layoutPtr->even_height = 1;
		layoutPtr->space_above = BB_MarginHeight(fbw);
		layoutPtr->space_between = BB_MarginWidth(fbw);
		newRow = TRUE;
		boxPtr++;
	}

	if (GOOD(FILES_LABEL(fbw)) && _XmGeoSetupKid(boxPtr, FILES_LABEL(fbw)))
	{
		layoutPtr->fill_mode = XmGEO_EXPAND;
		layoutPtr->fit_mode = XmGEO_AVERAGING;
		layoutPtr->even_width = 1;
		layoutPtr->even_height = 1;
		layoutPtr->space_above = BB_MarginHeight(fbw);
		layoutPtr->space_between = BB_MarginWidth(fbw);
		newRow = TRUE;
		boxPtr++;
	}

	if (newRow)
	{
		layoutPtr++;
		boxPtr++;
	}

	newRow = FALSE;
	if (GOOD(LIST1(fbw)) && _XmGeoSetupKid(boxPtr, XmIconListGetScrolledWindow(LIST1(fbw))))
	{
		layoutPtr->stretch_height = 1;
		layoutPtr->fill_mode = XmGEO_EXPAND;
		layoutPtr->fit_mode = XmGEO_AVERAGING;
		layoutPtr->even_width = 1;
		layoutPtr->even_height = 1;
		layoutPtr->space_above = GOOD(DIRECTORIES_LABEL(fbw)) ? 0 : BB_MarginHeight(fbw);
		layoutPtr->space_between = BB_MarginWidth(fbw);
		newRow = TRUE;
		boxPtr++;
	}

	if (GOOD(LIST2(fbw)) && _XmGeoSetupKid(boxPtr, XmIconListGetScrolledWindow(LIST2(fbw))))
	{
		layoutPtr->stretch_height = 1;
		layoutPtr->fill_mode = XmGEO_EXPAND;
		layoutPtr->fit_mode = XmGEO_AVERAGING;
		layoutPtr->even_width = 1;
		layoutPtr->even_height = 1;
		layoutPtr->space_above = 0; /* BB_MarginHeight(fbw); */
		layoutPtr->space_between = BB_MarginWidth(fbw);
		newRow = TRUE;
		boxPtr++;
	}

	if (newRow)
	{
		layoutPtr++;
		boxPtr++;
	}

	if (GOOD(WORK(fbw)) && _XmGeoSetupKid(boxPtr, WORK(fbw)))
	{
		layoutPtr->fill_mode = XmGEO_CENTER;
		layoutPtr->fit_mode = XmGEO_AVERAGING;
		layoutPtr->even_width = 1;
		layoutPtr->even_height = 1;
		layoutPtr->space_above = BB_MarginHeight(fbw);
		boxPtr += 2;
		layoutPtr++;
	}

	if (GOOD(SELECTION_LABEL(fbw)) && _XmGeoSetupKid(boxPtr, SELECTION_LABEL(fbw)))
	{
		layoutPtr->fill_mode = XmGEO_EXPAND;
		layoutPtr->even_width = 0;
		layoutPtr->even_height = 1;
		layoutPtr->space_above = BB_MarginHeight(fbw);
		layoutPtr++;
		boxPtr += 2;
	}

	if (GOOD(TEXTF(fbw)) && _XmGeoSetupKid(boxPtr, TEXTF(fbw)))
	{
		layoutPtr->fill_mode = XmGEO_EXPAND;
		layoutPtr->stretch_height = 0;
		layoutPtr->even_height = 1;
		layoutPtr->even_width = 0;
		layoutPtr->space_above = 0; /* BB_MarginHeight(fbw); */
		layoutPtr++;
	}

	layoutPtr->space_above = 0; /* BB_MarginHeight(fbw); */
	layoutPtr->end = TRUE;
	return(geoSpec);
}

static Boolean noGeoRequest(XmGeoMatrix _geoSpec)
{
	if(BB_InSetValues(_geoSpec->composite) && 
		(XtClass(_geoSpec->composite) == xmGUIFileBoxWidgetClass))
			return(TRUE);

	return( FALSE);
}

static void splitCompletion(XmGUIFileBoxWidget _fbw, char *_string, char *_buffer)
{
	char *ptr;

	ptr = strrchr(_string, '/');
	if (ptr == NULL)
		strcpy(_buffer, _string);
	else
	{
		ptr++;
		strcpy(_buffer, ptr);
		*ptr = 0;
	}
	strcpy(_buffer, "*");
}

static void completeDirectory(XmGUIFileBoxWidget _fbw, Widget _w, const char *_string)
{
	char buffer[MAX_PATH_NAME_LENGTH];
	const char *newString;
	int count;

	if (_w != TEXTF(_fbw))
		return;

	if (NAME_HAS_DIR(_string) && (_string[0] != '/'))
	{
		strcpy(buffer, FBDIR(_fbw));
		if (buffer[strlen(buffer)-1] != '/')
			strcat(buffer, "/");
		strcat(buffer, _string);
	}
	else
		strcpy(buffer, _string);

	newString = GUIFBCompleteSelection(buffer, PATTERN(_fbw), &count);
	if (newString == NULL)
	{
		XBell(XtDisplay(_fbw), 0);
		return;
	}

	if ((newString[strlen(newString)-1] != '/') && (strcmp(newString, buffer) == 0))
	{
		free((void *)newString);
		XBell(XtDisplay(_fbw), 0);
		return;
	}

#if 0
	if (count < 2)
	{
   		updateSelection(_fbw, NULL, newString, TRUE);
		updateFileList(_fbw, FBDIR(_fbw), NULL, PATTERN(_fbw));
	}
	else
#endif
	{
		updateSelection(_fbw, NULL, newString, TRUE);
		updateFileList(_fbw, FBDIR(_fbw), SELECTION(_fbw), PATTERN(_fbw));
	}

	free((void *)newString);
}

static void scrollToFirstFile(Widget _w, XEvent *_event, String *_args, Cardinal *_numArgs)
{
	XmGUIFileBoxWidget fbw;

	if (XmIsIconList(_w))
		fbw = (XmGUIFileBoxWidget)XtParent(XmIconListGetScrolledWindow(_w));
	else
		fbw = (XmGUIFileBoxWidget)XtParent(_w);

	if (!XmIsGUIFileBox((Widget)fbw))
	{
		XtAppWarningMsg(XtDisplayToApplicationContext(XtDisplay(_w)), "GUIFileBoxScrollToFirstFile()", "wrongWidgetClass",
		"TranslationError",
		"The action GUIFileBoxScrollToFirstFile() was unable to find a widget of class XmGUIFileBoxWidgetClass.",
		(String *)NULL, (Cardinal *)NULL);
		return;
	}

	if (MODE(fbw) == XmGUIFB_STANDARD_MODE)
		XmIconListScrollToRow(LIST1(fbw), DTOTAL(fbw), TRUE, TRUE, TRUE);
}


typedef struct _HolderStruct
{
	Widget widget;
	int startPos;
	int endPos;
	int searchCount;
} HolderStruct;

static void completeSelection(Widget _w, XEvent *_event, String *_args, Cardinal *_numArgs)
{
	XmGUIFileBoxWidget fbw;
	char *string;
	int len, j;
	int i, charCount;
	Boolean done;
	IconListElement *thisElement, *nextElement;
	HolderStruct holder[2];
	int holderCount;

	if (!XmIsTextField(_w))
	{
		XtAppWarningMsg(XtDisplayToApplicationContext(XtDisplay(_w)), "GUIFileBoxCompleteSelection()", "wrongWidgetClass",
		"TranslationError",
		"The action GUIFileBoxCompleteSelection() must be used on an XmTextField.",
		(String *)NULL, (Cardinal *)NULL);

		return;
	}

	fbw = (XmGUIFileBoxWidget)XtParent(_w);
	if (!XmIsGUIFileBox((Widget)fbw))
	{
		XtAppWarningMsg(XtDisplayToApplicationContext(XtDisplay(_w)), "GUIFileBoxCompleteSelection()" , "wrongWidgetClass",
		"TranslationError",
		"The action GUIFileBoxCompleteSelection() was unable to find a widget of class XmGUIFileBoxWidgetClass.",
		(String *)NULL, (Cardinal *)NULL);
		return;
	}

	if (ELEMENTS(fbw) == NULL)
		return;

	string = XmTextFieldGetString(_w);
	len = strlen(string);

	if (len == 0)
	{
		XtFree((void *)string);
		XBell(XtDisplay(_w), 0);
		return;
	}

	if (NAME_HAS_DIR(string))
	{
		completeDirectory(fbw, _w, string);
		return;
	}

	holderCount = -1;

	thisElement = ELEMENTS(fbw)+LIST1_SEARCH_POS(fbw);
	if (strncmp(thisElement->string, string, len) == 0)
	{
		holderCount++;
		holder[holderCount].widget = LIST1(fbw);
		holder[holderCount].startPos = LIST1_SEARCH_POS(fbw);
		holder[holderCount].endPos = (MODE(fbw) == XmGUIFB_ADVANCED_MODE) ? DTOTAL(fbw) : (DTOTAL(fbw) + FTOTAL(fbw));
		holder[holderCount].searchCount = 0;
	}

	if (MODE(fbw) == XmGUIFB_ADVANCED_MODE)
	{
		thisElement = ELEMENTS(fbw)+LIST2_SEARCH_POS(fbw);
		if (strncmp(thisElement->string, string, len) == 0)
		{
			holderCount++;
			holder[holderCount].widget = LIST2(fbw);
			holder[holderCount].startPos = LIST2_SEARCH_POS(fbw);
			holder[holderCount].endPos = DTOTAL(fbw)+FTOTAL(fbw);
			holder[holderCount].searchCount = 0;
		}
	}

	if (holderCount == -1)
	{
		XBell(XtDisplay(fbw), 0);
		XtFree((void *)string);
		return;
	}

	holderCount++;
	for(i=0;i<holderCount;i++)
	{
		thisElement = ELEMENTS(fbw)+holder[i].startPos;
		j = holder[i].startPos;
		done = FALSE;
		while(!done)
		{
			if (strncmp(thisElement->string, string, len) == 0)
			{
				holder[i].searchCount++;
				j++;
				done = (j == holder[i].endPos);
				thisElement++;
			}
			else
				done = TRUE;
		}
	}

	charCount = len;

	thisElement = ELEMENTS(fbw)+holder[0].startPos;
	done = FALSE;
	while(!done)
	{
		if (thisElement->string[charCount] == 0)
			done = TRUE;
		else
		for(i=0;!done && (i<holderCount);i++)
		{
			holder[i].endPos = holder[i].startPos + holder[i].searchCount;
			for(j=holder[i].startPos;!done && (j<holder[i].endPos);j++)
			{
				nextElement = ELEMENTS(fbw)+j;
				if (thisElement->string[charCount] != nextElement->string[charCount])
					done = TRUE;
			}
		}
		if (!done)
			charCount++;
	}

	XtFree((void *)string);

	if (charCount != len)
	{
		string = (char *)XtMalloc(sizeof(char) * (charCount+1));
		strncpy(string, thisElement->string, charCount);
		string[charCount] = 0;

		IGNORE_TFV(fbw) = TRUE;
		XmTextFieldSetString(_w, string);
		XmTextFieldSetCursorPosition(_w, XmTextFieldGetLastPosition(_w));
		IGNORE_TFV(fbw) = FALSE;

		XtFree((void *)string);
	}
	else
		XBell(XtDisplay(_w), 0);
}

static void resetCurrentSearchPos(Widget _w, XtPointer _client, XtPointer _call)
{
	XmGUIFileBoxWidget fbw = (XmGUIFileBoxWidget)_client;

	LIST1_SEARCH_POS(fbw) = 0;
	LIST2_SEARCH_POS(fbw) = DTOTAL(fbw);
}

static void searchScrollList(XmGUIFileBoxWidget _fbw, Widget _list, char *_string, int _len, 
		int _restartPos, int _count, int *_firstPos)
{
	Boolean done;
	int i, j;
	int offset;
	char *startpos;

	if (NAME_HAS_DIR(_string))
		offset = strlen(FBDIR(_fbw));
	else
		offset = 0;

	if (strlen(_string) < offset)
		startpos = _string;
	else
	{
		_len -= offset;
		startpos = _string+offset;
	}

	done = FALSE;
	i = *_firstPos;
	j = 0;
	while(!done)
	{
		if (strncmp(startpos, (ELEMENTS(_fbw)+i)->string, _len) == 0)
		{
			*_firstPos = i;
			XmIconListScrollToRow(_list, i-_restartPos, TRUE, TRUE, TRUE);
			return;
		}
		else
		{
			i++;
			if (i == (_restartPos+_count))
				i = _restartPos;
			done = (i == (*_firstPos));
		}
	}
}

static void textFieldSearch(Widget _w, XtPointer _client, XtPointer _call)
{
	XmGUIFileBoxWidget fbw;
	int len;
	char *string;

	fbw = (XmGUIFileBoxWidget)_client;

	if (IGNORE_TFV(fbw) || !ELEMENTS(fbw))
		return;

	string = XmTextFieldGetString(_w);
	len = strlen(string);

	if (len == 0)
	{
		XtFree((void *)string);
		return;
	}

	if (MODE(fbw) != XmGUIFB_ADVANCED_MODE)
	{
		if (DTOTAL(fbw)+FTOTAL(fbw))
			searchScrollList(fbw, LIST1(fbw), string, len, 0, DTOTAL(fbw)+FTOTAL(fbw), &LIST1_SEARCH_POS(fbw));
	}
	else
	{
		if (DTOTAL(fbw))
			searchScrollList(fbw, LIST1(fbw), string, len, 0, DTOTAL(fbw), &LIST1_SEARCH_POS(fbw));
		if (FTOTAL(fbw))
			searchScrollList(fbw, LIST2(fbw), string, len, DTOTAL(fbw), FTOTAL(fbw), &LIST2_SEARCH_POS(fbw));
	}

	XtFree((void *)string);
}


static void updateFileTextField(Widget _w, XtPointer _client, XtPointer _call)
{
	XmGUIFileBoxWidget fbw;
	XmIconListCallbackStruct *ilcs;

	fbw = (XmGUIFileBoxWidget)_client;
	ilcs = (XmIconListCallbackStruct *)_call;

	if (ELEMENTS(fbw) == NULL)
		return;

	updateSelection(fbw, FBDIR(fbw), ilcs->element->string, TRUE);
}

static void simplifyDirectory(char *_buffer)
{
	int done;
	char *ptr, *lastPtr, *thisPtr;

	if (_buffer == (char *)NULL)
		return;

	ptr = _buffer;
	lastPtr = ptr;
	done = 0;
	while(!done)
	{
		thisPtr = ptr;
		if (*ptr++ == '/')
		{
			switch(*ptr)
			{
				case '.':
					switch(*(ptr+1))
					{
						case '.':
							if (*(ptr+2) == '/')
							{
								bcopy(ptr+2, lastPtr, strlen(ptr+2)+1);
								ptr = lastPtr;
							}
							else
							if (*(ptr+2) == 0)
								*(lastPtr+1) = 0;
						break;
						case '/':
							bcopy(ptr+1, thisPtr, strlen(ptr+1)+1);
							ptr = thisPtr;
						break;
					}
					break;
				case '/':
					while(*(ptr) == '/')
						ptr++;
					bcopy(thisPtr, ptr, strlen(ptr)+1);
					ptr = thisPtr;
					break;
			}
			lastPtr = thisPtr;
		}
		else
		if (*(thisPtr) == 0)
			done = 1;
	}
}

static void rectifyDirectory(XmGUIFileBoxWidget _fbw)
{
	int fSize, bSize, pwdSize, extra;
	char *buffer;

	if (FBDIR(_fbw) == (char *)XmUNSPECIFIED)
	{
		FBDIR(_fbw) = _XmOSGetHomeDirName();
		if (FBDIR(_fbw))
			FBDIR(_fbw) = XtNewString(FBDIR(_fbw));
		else
			FBDIR(_fbw) = XtNewString(".");
	}
	else
		FBDIR(_fbw) = XtNewString(FBDIR(_fbw));

	if (FBDIR(_fbw)[0] != '/')
	{
		bSize = MAX_PATH_NAME_LENGTH;
		buffer = XtMalloc(sizeof(char)*bSize);
/*
** Is sending buffer as NULL portable?
*/
		while (getcwd(buffer, MAX_PATH_NAME_LENGTH) == NULL)
		{
			bSize *= 2;
			buffer = XtRealloc(buffer, sizeof(char)*bSize);
		}

		fSize = strlen(FBDIR(_fbw));
		pwdSize = strlen(buffer);
		if ((FBDIR(_fbw)[0] != '/') && (buffer[pwdSize-1] != '/'))
			extra = 2;
		else
			extra = 1;

		if ((fSize+pwdSize+extra) > bSize)
		{
			bSize = fSize+pwdSize+extra;
			buffer = XtRealloc(buffer, bSize);
		}
		if (extra == 2)
			buffer[pwdSize] = '/';
		bcopy(FBDIR(_fbw), buffer+pwdSize+extra-1, fSize);
		buffer[fSize+pwdSize+extra-1] = 0;

		XtFree((void *)FBDIR(_fbw));
		FBDIR(_fbw) = buffer;
	}
	simplifyDirectory(FBDIR(_fbw));
	/* menuAdjust(_fbw); */
}

static void filter(Widget _w, XtPointer _client, XtPointer _call)
{
	XmGUIFileBoxWidget fbw;
	const char *tmp;

	fbw = (XmGUIFileBoxWidget)_client;

	if (ELEMENTS(fbw) == NULL)
		return;

	tmp = XmTextFieldGetString(FILTER(fbw));

	updatePattern(fbw, tmp, TRUE);
	updateFileList(fbw, FBDIR(fbw), NULL, PATTERN(fbw));

	XtFree((void *)tmp);
}

static void goUpDir(XmGUIFileBoxWidget _fbw)
{
	char buffer[MAX_PATH_NAME_LENGTH];
	char *ptr;

	ptr = strrchr(FBDIR(_fbw), '/');
	if (ptr)
	{
		if ( (ptr != FBDIR(_fbw)) && (*(ptr+1) == 0))
		{
			*ptr = 0;
			ptr = strrchr(FBDIR(_fbw), '/');
		}
		*(ptr+1) = 0;

		ptr = XmTextFieldGetString(TEXTF(_fbw));
		if (NAME_HAS_DIR(ptr))
		{
			strcpy(buffer, FBDIR(_fbw));
			if (SELECTION(_fbw))
			{
				if (buffer[strlen(buffer)-1] != '/')
					strcat(buffer, "/");
				strcat(buffer, SELECTION(_fbw));
			}
			updateSelection(_fbw, NULL, buffer, FALSE);
		}
		else
			updateSelection(_fbw, FBDIR(_fbw), SELECTION(_fbw), FALSE);
	}
}

static void fixUpDir(XmGUIFileBoxWidget _fbw, const char *_subdir)
{
	char buffer[MAX_PATH_NAME_LENGTH];

	if (!_subdir)
		return;

	strcpy(buffer, FBDIR(_fbw) ? FBDIR(_fbw) : "");
	if ( (buffer[strlen(buffer)-1] != '/') && (_subdir[0] != '/'))
		strcat(buffer, "/");
	strcat(buffer, _subdir);

	updateSelection(_fbw, buffer, SELECTION(_fbw), TRUE);
}

static void activate(Widget _w, XtPointer _client, XtPointer _call)
{
	XmIconListCallbackStruct *ilcs;
	XmGUIFileBoxCallbackStruct fbcs;
	XmGUIFileBoxWidget fbw;
	const char *string;
	char buffer[MAX_PATH_NAME_LENGTH];
	int needFree;

	fbw = (XmGUIFileBoxWidget)_client;

	if (!XtHasCallbacks((Widget)fbw, XmNactivateCallback))
		return;

	if (ELEMENTS(fbw) == NULL)
		return;

	fbcs.event = NULL;

	needFree = FALSE;
	if (_w == LIST1(fbw))
	{
		ilcs = (XmIconListCallbackStruct *)_call;
		if (ilcs->row < DTOTAL(fbw))
		{
			if (ilcs->row == UP_DIR(fbw))
			{
				goUpDir(fbw);
				updateFileList(fbw, FBDIR(fbw), NULL, PATTERN(fbw));
			}
			else
			{
				fixUpDir(fbw, ilcs->element->string);
				updateFileList(fbw, FBDIR(fbw), NULL, PATTERN(fbw));
			}
			updateSelection(fbw, FBDIR(fbw), NULL, TRUE);
			return;
		}
		fbcs.event = ilcs->event;
		updateSelection(fbw, FBDIR(fbw), ilcs->element->string, TRUE);
		string = ilcs->element->string;
		needFree = FALSE;
	}
	else
	if (_w == LIST2(fbw))
	{
		ilcs = (XmIconListCallbackStruct *)_call;
		updateSelection(fbw, FBDIR(fbw), ilcs->element->string, TRUE);
		string = ilcs->element->string;
		needFree = FALSE;
	}
	else
		string = XmTextFieldGetString(TEXTF(fbw));

	fbcs.reason = XmCR_ACTIVATE;
	fbcs.directory = FBDIR(fbw);
	fbcs.file = string;

	getFullFilename(fbcs.directory, fbcs.file, buffer);

	fbcs.fullname = buffer;

	XtCallCallbacks((Widget)fbw, XmNactivateCallback, &fbcs);

	if (needFree)
		XtFree((void *)fbcs.file);
}


static void toLower(char *_str1, char *_str2, int _length)
{
	int i;
	char *ptr;

	for(ptr=_str1,i=0;(ptr!=NULL) && (i<_length);ptr++,i++)
		*(_str2+i) = tolower(*ptr);
}

static Boolean cvtStringToGUIFBDialogMode(Display *_display, XrmValuePtr _args,
	Cardinal *_numArgs, XrmValuePtr _from, XrmValuePtr _to, XtPointer *_data)
{
	char *lower;
	static unsigned char dialogMode;
	Boolean badConversion = TRUE;

	if (*_numArgs != 0)
	{
		XtAppWarningMsg(XtDisplayToApplicationContext(_display), "cvtStringToGUIFBDialogMode", "wrongParamaters",
		"ResourceError",
		"cvtStringToGUIFBDialogMode needs no arguments.",
		(String *)NULL, (Cardinal *)NULL);
	}

	lower = XtNewString(_from->addr);
	toLower(_from->addr, lower, strlen(_from->addr));

	dialogMode = XmGUIFB_STANDARD_MODE;
	if ( strcmp(lower, "advanced_mode") == 0)
	{
		badConversion = 0;
		dialogMode = XmGUIFB_ADVANCED_MODE;
	}
	else
	if ( strcmp(lower, "standard_mode") == 0)
	{
		badConversion = 0;
		dialogMode = XmGUIFB_STANDARD_MODE;
	}
	else
	if ( strcmp(lower, "guifb_advanced_mode") == 0)
	{
		badConversion = 0;
		dialogMode = XmGUIFB_ADVANCED_MODE;
	}
	else
	if ( strcmp(lower, "guifb_standard_mode") == 0)
	{
		badConversion = 0;
		dialogMode = XmGUIFB_ADVANCED_MODE;
	}

	XtFree(lower);

	if (badConversion)
		XtDisplayStringConversionWarning(_display, _from->addr, XmRGUIFBDialogMode);
	else
	{
		if (_to->addr == NULL)
			_to->addr = (XtPointer)&dialogMode;
		else
		if (_to->size < sizeof(unsigned char))
			badConversion = TRUE;
		else
			*(unsigned char *)_to->addr = dialogMode;
			_to->size = sizeof(unsigned char);
	}

	return !badConversion;
}

#if 0
static Boolean cvtStringToGUIFBIconPairs(Display *_display, XrmValuePtr _args,
	Cardinal *_numArgs, XrmValuePtr _from, XrmValuePtr _to, XtPointer *_data)
{
	char *lower;
	static unsigned char childType;
	Boolean badConversion = TRUE;

	if (*_numArgs != 0)
	{
		XtAppWarningMsg(XtDisplayToApplicationContext(_display), "cvtStringToGUIFBChildType", "wrongParamaters",
		"ResourceError",
		"cvtStringToGUIFBChildType needs no arguments.",
		(String *)NULL, (Cardinal *)NULL);
	}

	lower = XtNewString(_from->addr);
	toLower(_from->addr, lower, strlen(_from->addr));

	XtFree(lower);

/*
	if (badConversion)
		XtDisplayStringConversionWarning(_display, _from->addr, XmRGUIFBChildType);
	else
	{
		if (_to->addr == NULL)
			_to->addr = (XtPointer)&childType;
		else
		if (_to->size < sizeof(unsigned char))
			badConversion = TRUE;
		else
			*(unsigned char *)_to->addr = childType;
			_to->size = sizeof(unsigned char);
	}
*/

	return !badConversion;
}
#endif


Widget XmCreateGUIFileBox(Widget _parent, const char *_name, ArgList _warg, Cardinal _numWarg)
{
	return (XtCreateWidget(_name, xmGUIFileBoxWidgetClass, _parent, _warg, _numWarg));
}

void XmGUIFileBoxSetMode(Widget _w, unsigned char _mode)
{
	XmGUIFileBoxWidget fbw;
	Widget manageList[10];
	int manageI;

	fbw = (XmGUIFileBoxWidget)_w;

	if (MODE(fbw) == _mode)
		return;

	manageI = 0;
	if (_mode == XmGUIFB_ADVANCED_MODE)
	{
		manageList[manageI++] = OPTION(fbw);
		manageList[manageI++] = FILTER(fbw);
		manageList[manageI++] = XmIconListGetScrolledWindow(LIST1(fbw));
		manageList[manageI++] = XmIconListGetScrolledWindow(LIST2(fbw));
		if (WORK(fbw))
			manageList[manageI++] = WORK(fbw);
		manageList[manageI++] = TEXTF(fbw);
		manageList[manageI++] = FILTER_LABEL(fbw);
		manageList[manageI++] = DIRECTORIES_LABEL(fbw);
		manageList[manageI++] = FILES_LABEL(fbw);
		XtManageChildren(manageList, manageI);
	}
	else
	if (_mode == XmGUIFB_STANDARD_MODE)
	{
		manageList[manageI++] = FILTER(fbw);
		manageList[manageI++] = DIRECTORIES_LABEL(fbw);
		manageList[manageI++] = FILTER_LABEL(fbw);
		manageList[manageI++] = FILES_LABEL(fbw);
		manageList[manageI++] = XmIconListGetScrolledWindow(LIST2(fbw));
		XtUnmanageChildren(manageList, manageI);
	}

	MODE(fbw) = _mode;

	if (MODE(fbw) == XmGUIFB_ADVANCED_MODE)
	{
		if (GOOD(LIST1(fbw)))
			XmIconListSetItems(LIST1(fbw), ELEMENTS(fbw), DTOTAL(fbw));
		if (GOOD(LIST2(fbw)))
			XmIconListSetItems(LIST2(fbw), ELEMENTS(fbw)+DTOTAL(fbw), FTOTAL(fbw));
	}
	else
	if (GOOD(LIST1(fbw)))
		XmIconListSetItems(LIST1(fbw), ELEMENTS(fbw), DTOTAL(fbw)+FTOTAL(fbw));
}

#if SMARTMB

static void fileBoxDialogCB(Widget _w, XtPointer _data, XtPointer _client)
{
	XmGUIFileBoxWidget fbw;
	XmGUIFileBoxCallbackStruct fbcs;
	XmPushButtonCallbackStruct *pbcs;
	char buffer[MAX_PATH_NAME_LENGTH];

	fbw = (XmGUIFileBoxWidget)_data;
	pbcs = (XmPushButtonCallbackStruct *)_client;

	fbcs.event = pbcs->event;
	fbcs.directory = FBDIR(fbw);
	fbcs.file = XmTextFieldGetString(TEXTF(fbw));
	fbcs.fullname = buffer;

	getFullFilename(fbcs.directory, fbcs.file, buffer);

	if (_w == OK_BUTTON(fbw))
	{
		if (XtHasCallbacks((Widget)fbw, XmNokCallback))
		{
			fbcs.reason = XmCR_OK;
			XtCallCallbacks((Widget)fbw, XmNokCallback, (void *)&fbcs);
		}
	}
	else
	if (_w == CANCEL_BUTTON(fbw))
	{
		if (XtHasCallbacks((Widget)fbw,XmNcancelCallback))
		{
			fbcs.reason = XmCR_CANCEL;
			XtCallCallbacks((Widget)fbw, XmNcancelCallback, (void *)&fbcs);
		}
	}
	else
	if (_w == HELP_BUTTON(fbw))
	{
		if (XtHasCallbacks((Widget)fbw, XmNhelpCallback))
		{
			fbcs.reason = XmCR_HELP;
			XtCallCallbacks((Widget)fbw, XmNhelpCallback, (void *)&fbcs);
		}
	}

	XtFree((void *)fbcs.file);
}

static void _TitleFix(XmGeoMatrix _geoSpec, int _action, XmGeoMajorLayout _layoutPtr, XmKidGeometry _rowPtr)
{
	_rowPtr->box.width = 1;
}


static void fileBoxChangeModeCB(Widget _w, XtPointer _data, XtPointer _client)
{
	XmGUIFileBoxWidget fbw;

	fbw = (XmGUIFileBoxWidget)_data;

	if ( (_w == ADVANCED_BUTTON(fbw)) && (MODE(fbw) == XmGUIFB_STANDARD_MODE))
	{
		XtUnmanageChild(ADVANCED_BUTTON(fbw));
		XtManageChild(STANDARD_BUTTON(fbw));
		XmProcessTraversal(STANDARD_BUTTON(fbw), XmTRAVERSE_CURRENT);
		XmGUIFileBoxSetMode((Widget)fbw, XmGUIFB_ADVANCED_MODE);
	}
	else
	if ((_w == STANDARD_BUTTON(fbw)) && (MODE(fbw) == XmGUIFB_ADVANCED_MODE))
	{
		XtUnmanageChild(STANDARD_BUTTON(fbw));
		XtManageChild(ADVANCED_BUTTON(fbw));
		XmProcessTraversal(ADVANCED_BUTTON(fbw), XmTRAVERSE_CURRENT);
		XmGUIFileBoxSetMode((Widget)fbw, XmGUIFB_STANDARD_MODE);
	}
}

static void fileBoxFilterCB(Widget _w, XtPointer _data, XtPointer _client)
{
	XmGUIFileBoxUpdate((Widget)_data);
}

static void filterFocusCB(Widget _w, XtPointer _data, XtPointer _call)
{
	Widget w;
	XmGUIFileBoxWidget fbw;
	XmAnyCallbackStruct *acs;
	Arg warg[1];
	int n;

	w = (Widget)_data;
	fbw = (XmGUIFileBoxWidget)XtParent(_w);
	acs = (XmAnyCallbackStruct *)_call;

	n = 0;
	XtSetArg(warg[n], XmNdefaultButton, acs->reason == XmCR_LOSING_FOCUS ? 
				OK_BUTTON(fbw) : NULL); n++;
	XtSetValues(w, warg, n);

	XmUpdateDisplay(_w); /* HMMM */
}

Widget XmCreateGUIFileBoxInSmartMB(Widget _parent, const char *_name, ArgList _args, Cardinal _numArgs)
{
	XmGUIFileBoxWidget fbw;
	CompositeWidget smComp;
	Widget smartmb;
	Arg warg[3];
	int n;

	smartmb = XtCreateWidget("_smartMB", xmSmartMessageBoxWidgetClass, _parent, _args, _numArgs);
	fbw = (XmGUIFileBoxWidget)XtCreateManagedWidget("_guiFileBox", xmGUIFileBoxWidgetClass, smartmb, _args, _numArgs);

	XtAddCallback(FILTER(fbw), XmNfocusCallback, (XtCallbackProc)filterFocusCB, smartmb);
	XtAddCallback(FILTER(fbw), XmNlosingFocusCallback, (XtCallbackProc)filterFocusCB, smartmb);

	n = 0;
	XtSetArg(warg[n], XmNchildType, XmCHILD_CONTROL); n++;
	XtSetValues((Widget)fbw, warg, n);

	n = 0;
	XtSetArg(warg[n], XmNchildType, XmCHILD_SEPARATOR); n++;
	XtSetArg(warg[n], XmNorientation, XmHORIZONTAL); n++;
	XtCreateManagedWidget("_separator", xmSeparatorGadgetClass, smartmb, warg, n);

	n = 0;
	XtSetArg(warg[n], XmNchildType, XmCHILD_ACTION); n++;
	XtSetArg(warg[n], XmNnavigationType, XmTAB_GROUP); n++;
	OK_BUTTON(fbw) = XtCreateManagedWidget("OK", xmPushButtonGadgetClass, smartmb, warg, n);

	n = 0;
	XtSetArg(warg[n], XmNchildType, XmCHILD_ACTION); n++;
	XtSetArg(warg[n], XmNnavigationType, XmTAB_GROUP); n++;
	CANCEL_BUTTON(fbw) = XtCreateManagedWidget("Cancel", xmPushButtonGadgetClass, smartmb, warg, n);

	n = 0;
	XtSetArg(warg[n], XmNchildType, XmCHILD_ACTION); n++;
	XtSetArg(warg[n], XmNnavigationType, XmTAB_GROUP); n++;
	FILTER_BUTTON(fbw) = XtCreateManagedWidget("Update", xmPushButtonGadgetClass, smartmb, warg, n);

	n = 0;
	XtSetArg(warg[n], XmNchildType, XmCHILD_ACTION); n++;
	XtSetArg(warg[n], XmNnavigationType, XmTAB_GROUP); n++;
	ADVANCED_BUTTON(fbw) = XtCreateManagedWidget("Advanced", xmPushButtonGadgetClass, smartmb, warg, n);

	n = 0;
	XtSetArg(warg[n], XmNchildType, XmCHILD_ACTION); n++;
	XtSetArg(warg[n], XmNnavigationType, XmTAB_GROUP); n++;
	STANDARD_BUTTON(fbw) = XtCreateManagedWidget("Standard", xmPushButtonGadgetClass, smartmb, warg, n);

	n = 0;
	XtSetArg(warg[n], XmNchildType, XmCHILD_ACTION); n++;
	XtSetArg(warg[n], XmNnavigationType, XmTAB_GROUP); n++;
	HELP_BUTTON(fbw) = XtCreateManagedWidget("Help", xmPushButtonGadgetClass, smartmb, warg, n);

	n = 0;
	XtSetArg(warg[n], XmNdefaultButton, OK_BUTTON(fbw)); n++;
	XtSetArg(warg[n], XmNcancelButton, CANCEL_BUTTON(fbw)); n++;
	XtSetValues(smartmb, warg, n);

	smComp = (CompositeWidget)smartmb;
	XtManageChildren (smComp->composite.children, smComp->composite.num_children);

	if (MODE((XmGUIFileBoxWidget)fbw) == XmGUIFB_ADVANCED_MODE)
		XtUnmanageChild(ADVANCED_BUTTON(fbw));
	else
		XtUnmanageChild(STANDARD_BUTTON(fbw));

	XtAddCallback(OK_BUTTON(fbw), XmNactivateCallback, (XtCallbackProc)fileBoxDialogCB, (XtPointer)fbw);
	XtAddCallback(CANCEL_BUTTON(fbw), XmNactivateCallback, (XtCallbackProc)fileBoxDialogCB, (XtPointer)fbw);

	XtAddCallback(FILTER_BUTTON(fbw), XmNactivateCallback, (XtCallbackProc)fileBoxFilterCB, (XtPointer)fbw);

	XtAddCallback(ADVANCED_BUTTON(fbw), XmNactivateCallback, (XtCallbackProc)fileBoxChangeModeCB, (XtPointer)fbw);
	XtAddCallback(STANDARD_BUTTON(fbw), XmNactivateCallback, (XtCallbackProc)fileBoxChangeModeCB, (XtPointer)fbw);

	XtAddCallback(HELP_BUTTON(fbw), XmNactivateCallback, (XtCallbackProc)fileBoxDialogCB, (XtPointer)fbw);

/*
** This tab stuff is stupid, but we have to fool Motif!  It doesn't quite work right for managed/unmanaged
** widgets.
*/

	XmAddTabGroup(smartmb);

	return (Widget)fbw;
}


#endif /* SMARTMB */


void XmGUIFileBoxUpdate(Widget _w)
{
	XmGUIFileBoxWidget fbw;

	if (!XmIsGUIFileBox(_w))
		return;

	fbw = (XmGUIFileBoxWidget)_w;

	updateFileList(fbw, FBDIR(fbw), NULL, PATTERN(fbw));
}

void XmGUIFileBoxSetParameters(Widget _w, const char *_directory, const char *_pattern,
		const char *_selection)
{
	XmGUIFileBoxWidget fbw;

	if (!XmIsGUIFileBox(_w))
		return;

	fbw = (XmGUIFileBoxWidget)_w;

	if (_directory)
	{
		if (FBDIR(fbw))
			XtFree((void *)FBDIR(fbw));
		FBDIR(fbw) = (char *)_directory; /* will be copied */
		rectifyDirectory(fbw);
	}

	if (_pattern)
		updatePattern(fbw, _pattern, TRUE);

	if (_directory || _pattern)
		updateFileList(fbw, FBDIR(fbw), NULL, PATTERN(fbw));

	if (_selection)
	{
		if (NAME_HAS_DIR(_selection))
			updateSelection(fbw, NULL, _selection, TRUE);
		else
			updateSelection(fbw, FBDIR(fbw), _selection, TRUE);

		textFieldSearch(TEXTF(fbw), (XtPointer)fbw, NULL);
	}
}

char *XmGUIFileBoxGetDirectory(Widget _w)
{
	if (XmIsGUIFileBox(_w))
		return (FBDIR((XmGUIFileBoxWidget)_w));
	else
		return NULL;
}

char *XmGUIFileBoxGetPattern(Widget _w)
{
	XmGUIFileBoxWidget fbw;
	char *ptr;
	if (XmIsGUIFileBox(_w))
	{
		fbw = (XmGUIFileBoxWidget)_w;
		return (PATTERN(fbw));
	}
	else
		return NULL;
}

Widget XmGUIFileBoxGetChild(Widget _w, int _child)
{
	XmGUIFileBoxWidget fbw;

	if (!XmIsGUIFileBox(_w))
		return NULL;

	fbw = (XmGUIFileBoxWidget)_w;
	switch(_child)
	{
		case XmDIALOG_TITLE_LABEL:
			return TITLE(fbw);

		case XmDIALOG_CANCEL_BUTTON:
			return CANCEL_BUTTON(fbw);

		case XmDIALOG_DEFAULT_BUTTON:
			return BB_DefaultButton(fbw);

		case XmDIALOG_OK_BUTTON:	 
			return OK_BUTTON(fbw);

		case XmDIALOG_FILTER_LABEL:
			return FILTER_LABEL(fbw);

		case XmDIALOG_FILTER_TEXT:	   
			return FILTER(fbw);

		case XmDIALOG_APPLY_BUTTON:
			return FILTER_BUTTON(fbw);

		case XmDIALOG_HELP_BUTTON:
			return HELP_BUTTON(fbw);

		case XmDIALOG_FILES_LIST:
			return LIST2(fbw);

		case XmDIALOG_FILES_LIST_LABEL:
			return FILES_LABEL(fbw);

		case XmDIALOG_SELECTION_LABEL:
			return SELECTION_LABEL(fbw);

		case XmDIALOG_TEXT:
			return TEXTF(fbw);

		case XmDIALOG_SEPARATOR:
			return SEP(fbw);

		case XmDIALOG_DIR_LIST:
			return LIST1(fbw);

		case XmDIALOG_DIR_LIST_LABEL:
			return DIRECTORIES_LABEL(fbw);

		case XmDIALOG_ADVANCED_BUTTON:
			return ADVANCED_BUTTON(fbw);

		case XmDIALOG_STANDARD_BUTTON:
			return STANDARD_BUTTON(fbw);
	}

	return NULL;
}










