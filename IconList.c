/* $Id: IconList.c,v 1.6 1996/02/20 01:19:52 cwikla Exp $ */
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

/*
** IconList widget 
*/

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <Xm/XmP.h>
#include <Xm/DrawP.h>
#include <Xm/PrimitiveP.h>
#include <Xm/ScrolledW.h>
#include <Xm/Frame.h>
#include <Xm/ScrollBar.h>

#include "DumbClip.h"
#include "IconListP.h"

#if XmVersion >= 2000
extern void _XmSelectColorDefault(Widget,int,XrmValue *);
#endif

static void classInitialize();
static void initialize(XmIconListWidget _request, XmIconListWidget _new, String *_args, Cardinal *_numArgs);
static void expose(XmIconListWidget _ilw, XEvent *_event, Region _region);
static void resize(XmIconListWidget _ilw);
static void getGCsAndFonts(XmIconListWidget _ilw);

static XmNavigability widgetNavigable(Widget _w);
static void focusChange(Widget _w, XmFocusChange _fc);

static void destroy(XmIconListWidget _ilw);
static void drawElementBackground(XmIconListWidget _ilw, int _nelement, GC _gc);
static void drawIconListElement(XmIconListWidget _ilw, int _row, GC _backgroundGC, GC _highlightGC);
static void drawIconList(XmIconListWidget _ilw, XRectangle *_xrect);

static int yToElementRow(XmIconListWidget _ilw, Position _y);

static void arm(Widget _w, XEvent *_event, String *_args, Cardinal *_numArgs);
static void activate(Widget _w, XEvent *_event, String *_args, Cardinal *_numArgs);
static void armAndActivate(Widget _w, XEvent *_event, String *_args, Cardinal *_numArgs);

static void up(Widget _w, XEvent *_event, String *_args, Cardinal *_numArgs);
static void down(Widget _w, XEvent *_event, String *_args, Cardinal *_numArgs);

static void buttonMotionEH(Widget _w, XtPointer _data, XEvent *_event, Boolean *_cont);

static int getFirstVisibleElement(XmIconListWidget _ilw);
static Boolean rowFullyVisible(XmIconListWidget _ilw, int _row, Boolean *_aboveTop);
static void moveFocusElement(XmIconListWidget _ilw, int _whence, XEvent *_event);
static void setFocusElement(XmIconListWidget _ilw, int _row, Boolean _toTop);
static void scrollToRow(XmIconListWidget _ilw, int _row, Boolean _jump, Boolean _toTop);
static void syncScrollBar(XmIconListWidget _ilw);
static void highlightElement(XmIconListWidget _ilw, int _row, GC _gc);
static void callCallbacks(XmIconListWidget _ilw, int _reason, XEvent *_event);
static XtGeometryResult queryGeometry(XmIconListWidget _ilw, XtWidgetGeometry *_request, XtWidgetGeometry *_pref);

#define SPACE 5

#define CORE(a) ((a)->core)
#define PRIM(a) ((a)->primitive)
#define ICON_LIST(a) ((a)->iconList)

#define WIDTH(a) ((a)->core.width)
#define HEIGHT(a) ((a)->core.height)
#define BORDER(a) ((a)->core.border_width)
#define BWIDTH(a) ((a)->core.width + 2 * (a)->core.border_width)
#define BHEIGHT(a) ((a)->core.height + 2 * (a)->core.border_width)

#define MOVE(a,b,c) _XmMoveObject((Widget)(a), (b), (c))
#define RESIZE(a,b,c,d) _XmResizeObject((Widget)(a), (b), (c), (d))

#define HIGHLIGHT_GC(a) ((a)->primitive.highlight_GC)

#define BG(a) ((a)->core.background_pixel)
#define DEPTH(a) ((a)->core.depth)

#define COPY_GC(a) ((a)->iconList.copyGC)
#define THE_GC(a) ((a)->iconList.gc)
#define ERASE_GC(a) ((a)->iconList.eraseGC)
#define IGC(a) ((a)->iconList.insensitiveGC)
#define SELECT_GC(a) ((a)->iconList.selectGC)
#define SELECT(a) ((a)->iconList.selectColor)
#define FG(a) ((a)->primitive.foreground)
#define FL(a) ((a)->iconList.fontList)
#define THE_FONT(a) ((a)->iconList.font)
#define ELEMENTS(a) ((a)->iconList.elements)
#define FOCUS_EL(a) ((a)->iconList.focusElement)
#define NUM_ELEMENTS(a) ((a)->iconList.numberOfElements)
#define FULL_EL_HEIGHT(a) ((a)->iconList.fullElementHeight)
#define HT(a) ((a)->iconList.highlightThickness)
#define VIS_COUNT(a) ((a)->iconList.visibleItemCount)
#define MARG_W(a)  ((a)->iconList.marginWidth)
#define MARG_H(a) ((a)->iconList.marginHeight)
#define TEXT_HEIGHT(a) ((a)->iconList.textHeight)
#define TEXT_OFFSET(a) ((a)->iconList.textOffset)
#define ICON_HEIGHT(a) ((a)->iconList.iconHeight)
#define ICON_WIDTH(a) ((a)->iconList.iconWidth)
#define USING_BITMAPS(a) ((a)->iconList.usingBitmaps)
#define LAST_CLICK_TIME(a) ((a)->iconList.lastClickTime)
#define LAST_CLICK_ROW(a) ((a)->iconList.lastClickRow)
#define CLICK_COUNT(a) ((a)->iconList.clickCount)
#define CLIP(a) ((a)->iconList.clipWidget)
#define VBAR(a) ((a)->iconList.verticalScrollBarWidget)
#define ST(a) Prim_ShadowThickness(a)
#define START_ROW(a) ((a)->iconList.startRow)
#define END_ROW(a) ((a)->iconList.endRow)
#define SW(a) ((a)->iconList.scrolledWindowWidget)
#define HAS_FOCUS(a) ((a)->iconList.hasFocus)
#define CALL_CALLBACKS(a) ((a)->iconList.callCallbacks)

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

static char iconListTranslations[] = 
"<Btn1Down>,<Btn1Up>:Activate()\n\
<Btn1Down>:Arm()\n\
~s ~m ~a <Key>Return:ArmAndActivate()\n\
~s ~m ~a <Key>space:ArmAndActivate() \n\
<Key>osfActivate:ArmAndActivate()\n\
<Key>osfSelect:ArmAndActivate()\n\
<Key>osfHelp: PrimitiveHelp()\n\
c <Key>osfUp:IconListTraverseUp(PAGE)\n\
c <Key>osfDown: IconListTraverseDown(PAGE)\n\
<Key>osfUp:IconListTraverseUp()\n\
<Key>osfDown:IconListTraverseDown()\n\
<Key>osfPageUp: IconListTraverseUp(PAGE)\n\
<Key>osfPageDown: IconListTraverseDown(PAGE)\n\
<EnterWindow>:PrimitiveEnter()\n\
<LeaveWindow>:PrimitiveLeave()\n\
<FocusOut>:PrimitiveFocusOut()\n\
<FocusIn>:PrimitiveFocusIn()\n\
s ~m ~a <Key>Tab:PrimitivePrevTabGroup()\n\
~m ~a <Key>Tab:PrimitiveNextTabGroup()\n\
<Unmap>: PrimitiveUnmap()";

static XtActionsRec actions[] =
{
	{"Arm", (XtActionProc)arm},
	{"Activate", (XtActionProc)activate},
	{"ArmAndActivate", (XtActionProc)armAndActivate},
	{"IconListTraverseUp", (XtActionProc)up},
	{"IconListTraverseDown", (XtActionProc)down},
};

#define TheOffset(field) XtOffset(XmIconListWidget, iconList.field) 
static XtResource resources[] =
{
	{XmNuserData, XmCUserData, XtRPointer, sizeof(XtPointer),
		TheOffset(userData), XtRImmediate, (XtPointer)0},
	{XmNfontList, XmCFontList, XmRFontList, sizeof(XmFontList),
		TheOffset(fontList), XmRImmediate, (XtPointer)NULL},
	{XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension, sizeof(Dimension),
		TheOffset(highlightThickness), XmRImmediate, (XtPointer) 2},
	{XmNmarginHeight, XmCMarginHeight, XmRVerticalDimension, sizeof(Dimension),
		TheOffset(marginHeight), XmRImmediate, (XtPointer)2},
	{XmNmarginWidth, XmCMarginWidth, XmRHorizontalDimension, sizeof(Dimension),
		TheOffset(marginWidth), XmRImmediate, (XtPointer)2},
	{XmNhighlightOnEnter, XmCHighlightOnEnter, XtRBoolean, sizeof(Boolean),
		TheOffset(highlightOnEnter), XtRImmediate, (XtPointer)TRUE},
	{XmNactivateCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
		TheOffset(activateCallback), XtRImmediate, (XtPointer)NULL},
	{XmNvalueChangedCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
		TheOffset(valueChangedCallback), XtRImmediate, (XtPointer)NULL},
	{XmNvisibleItemCount, XmCVisibleItemCount, XmRInt, sizeof(int),
		TheOffset(visibleItemCount), XtRImmediate, (XtPointer)7},
	{XmNiconWidth, XmCIconDimension, XmRDimension, sizeof(Dimension),
		TheOffset(iconWidth), XtRImmediate, (XtPointer)16},
	{XmNiconHeight, XmCIconDimension, XmRDimension, sizeof(Dimension),
		TheOffset(iconHeight), XtRImmediate, (XtPointer)16},
	{XmNvisibleItemCount, XmCVisibleItemCount, XtRInt, sizeof(int),
		TheOffset(visibleItemCount), XtRImmediate, (XtPointer)10},
	{XmNusingBitmaps, XmCUsingBitmaps, XtRBoolean, sizeof(Boolean),
		TheOffset(usingBitmaps), XtRImmediate, (XtPointer)FALSE},
	{XmNclipWidget, XmCClipWidget, XtRWidget, sizeof(Widget),
		TheOffset(clipWidget), XtRImmediate, (XtPointer)NULL},
	{XmNhorizontalScrollBar, XmCHorizontalScrollBar, XmRWidget, sizeof(Widget),
		TheOffset(horizontalScrollBarWidget), XtRImmediate, (XtPointer)NULL},
	{XmNverticalScrollBar, XmCVerticalScrollBar, XmRWidget, sizeof(Widget),
		TheOffset(verticalScrollBarWidget), XtRImmediate, (XtPointer)NULL},
	{XmNselectColor, XmCSelectColor, XtRPixel, sizeof(Widget),
		TheOffset(selectColor), XmRCallProc, (XtPointer)_XmSelectColorDefault},
};
#undef TheOffset

static XmBaseClassExtRec baseClassExtRec = {
	NULL,
	NULLQUARK,
	XmBaseClassExtVersion,
	sizeof(XmBaseClassExtRec),
	NULL,				 /* InitializePrehook	*/
	NULL,				 /* SetValuesPrehook */
	NULL,				 /* InitializePosthook	 */
	NULL,				 /* SetValuesPosthook	*/
	XmInheritClass,				 /* secondaryObjectClass */
	XmInheritSecObjectCreate,		 /* secondaryCreate	*/
	XmInheritGetSecResData,			 /* getSecRes data	 */
	{ 0 },					/* fastSubclass flags	 */
	XmInheritGetValuesPrehook,	 /* getValuesPrehook */
	XmInheritGetValuesPosthook,				 /* getValuesPosthook	*/
	NULL,								 /* classPartInitPrehook */
	NULL,								 /* classPartInitPosthook*/
	NULL,								 /* ext_resources		*/
	NULL,								 /* compiled_ext_resources*/
	0,									/* num_ext_resources	*/
	FALSE,								/* use_sub_resources	*/
	widgetNavigable,					/* widgetNavigable		*/
	focusChange				/* focusChange			*/
};

XmIconListClassRec xmIconListClassRec = 
{
	{
		(WidgetClass)&xmPrimitiveClassRec, /* superclass */
		(String)"XmIconList",				/* classname */
		(Cardinal)sizeof(XmIconListRec),		 /* widget size */
		classInitialize,					/* class init */
		(XtWidgetClassProc)NULL,		/* class part init */
		FALSE,								 /* class inited */
		(XtInitProc)initialize,				/* initialize */
		(XtArgsProc)NULL,					/* initialize hook */
		(XtRealizeProc)XtInheritRealize,			 /* realize */
		actions,								/* actions */
		XtNumber(actions),						/* number of actions */
		resources,							 /* resources */
		XtNumber(resources),			 /* number of resources */
		NULLQUARK,							 /* xrm class */
		TRUE,									/* compress motions */
		XtExposeCompressMultiple |
			XtExposeGraphicsExposeMerged,	/* compress exposures */
		TRUE,									/* compress enter/leave */
		TRUE,									/* visibility interest */
		(XtWidgetProc)destroy,				 /* destroy */
		(XtWidgetProc)resize,				/* resize */
		(XtExposeProc)expose,				 /* expose */
		NULL,									/* set values */
		NULL,									/* set values hook */
		XtInheritSetValuesAlmost,			/* set values almost */
		NULL,								/* get values hook */
		(XtAcceptFocusProc)NULL, /* AcceptFocus,/* accept focus */
		XtVersion,				/* version */
		NULL,					/* callback private */
		iconListTranslations, /* XtInheritTranslations, /* translations */
		(XtGeometryHandler)queryGeometry, /* XtInheritQueryGeometry,		/* query geometry */
		NULL,				 /* display accelerator */
		(XtPointer)&baseClassExtRec,			/* extension */
	},
	{   /* XmPrimitive Part */
		XmInheritBorderHighlight,
		XmInheritBorderUnhighlight,
		NULL, /* XtInheritTranslations, */
		NULL,
		NULL,
		0,
		(XtPointer)NULL,	 /* extension */
	},
	{ /* XmIconList Part */
		0,
	},
};

WidgetClass xmIconListWidgetClass = (WidgetClass)&xmIconListClassRec;

static void classInitialize()
{
	baseClassExtRec.record_type = XmQmotif;
}

static void initialize(XmIconListWidget _request, XmIconListWidget _new, String *_args, Cardinal *_numArgs)
{
	getGCsAndFonts(_new);

	/* XmAddTabGroup((Widget)_new); */

	CLIP(_new) = NULL;
	VBAR(_new) = NULL;
	SW(_new) = NULL;
	HAS_FOCUS(_new) = FALSE;

	FOCUS_EL(_new) = -1;
	START_ROW(_new) = 0;
	END_ROW(_new) = 0;

	CALL_CALLBACKS(_new) = TRUE;

	NUM_ELEMENTS(_new) = 0;
	ELEMENTS(_new) = NULL;
	LAST_CLICK_TIME(_new) = XtLastTimestampProcessed(XtDisplay(_request));
	CLICK_COUNT(_new) = 0;
	LAST_CLICK_ROW(_new) = -1;

	TEXT_HEIGHT(_new) = THE_FONT(_new)->ascent + THE_FONT(_new)->descent;

/*
** Full spaced out element height.
*/

	FULL_EL_HEIGHT(_new) = MAX(ICON_HEIGHT(_new),  TEXT_HEIGHT(_new)) + 2 * HT(_new);

	TEXT_OFFSET(_new) = (FULL_EL_HEIGHT(_new) - TEXT_HEIGHT(_new))/2;

	HEIGHT(_new) = FULL_EL_HEIGHT(_new) * VIS_COUNT(_new);

	WIDTH(_new) = ICON_WIDTH(_new) + SPACE + (20 * THE_FONT(_new)->max_bounds.width) + MARG_W(_new) + 2 * HT(_new);

	XtAddEventHandler((Widget)_new, ButtonMotionMask, FALSE, (XtEventHandler)buttonMotionEH, (XtPointer)_new);
}

static void getGCsAndFonts(XmIconListWidget _ilw)
{
	XGCValues	   values;
	XtGCMask		valueMask;
	XFontStruct	 *fs = (XFontStruct *) NULL;

	if (FL(_ilw) == NULL)
		FL(_ilw) = _XmGetDefaultFontList( (Widget) _ilw, XmLABEL_FONTLIST);

	FL(_ilw) = XmFontListCopy(FL(_ilw));

	valueMask = GCForeground | GCBackground;

	_XmFontListGetDefaultFont(FL(_ilw), &fs);
	values.foreground = FG(_ilw);
	values.background = BG(_ilw);
	values.graphics_exposures = False;

	COPY_GC(_ilw) = XtAllocateGC((Widget)_ilw, _ilw->core.depth,
			valueMask, &values, ~0L, 0L);

	valueMask |= GCFont | GCGraphicsExposures;

	if (fs == (XFontStruct *)NULL)
		valueMask &= ~GCFont;
	else
		values.font = fs->fid;

	THE_GC(_ilw) = XtAllocateGC((Widget)_ilw, _ilw->core.depth, valueMask, &values,
		GCClipMask | GCClipYOrigin, 0L);

	values.function = GXcopy;

	values.foreground = BG(_ilw);
	values.subwindow_mode = IncludeInferiors;
	ERASE_GC(_ilw) = XtGetGC((Widget)_ilw, valueMask, &values);

#if 0
	values.foreground = FG(_ilw);

	valueMask |= GCFillStyle | GCTile;
	values.fill_style = FillTiled;
	values.subwindow_mode = ClipByChildren;
	values.tile = XmGetPixmapByDepth (XtScreen((Widget)(_ilw)), "50_foreground",
			FG(_ilw), BG(_ilw), DEPTH(_ilw));

	IGC(_ilw) = XtGetGC((Widget) _ilw, valueMask, &values);
#endif

	if (fs == (XFontStruct *)NULL)
	{
		XGetGCValues(XtDisplay(_ilw), THE_GC(_ilw), GCFont, &values);
		fs = XQueryFont(XtDisplay(_ilw), values.font);
	}

	if (SELECT(_ilw) == FG(_ilw))
		SELECT(_ilw) = BG(_ilw);

	valueMask = GCForeground;
	values.foreground = SELECT(_ilw);

	SELECT_GC(_ilw) = XtGetGC((Widget)_ilw, valueMask, &values);

	THE_FONT(_ilw) = fs;
}

static void callCallbacks(XmIconListWidget _ilw, int _reason, XEvent *_event)
{
	XmIconListCallbackStruct ilcs;

	if (!CALL_CALLBACKS(_ilw) || (FOCUS_EL(_ilw) == -1) || !HAS_FOCUS(_ilw))
		return;

	ilcs.row = FOCUS_EL(_ilw);
	ilcs.element = ELEMENTS(_ilw)+FOCUS_EL(_ilw);	
	ilcs.reason = _reason;
	ilcs.event = _event;

	switch(_reason)
	{
		case XmCR_VALUE_CHANGED:
			if (XtHasCallbacks((Widget)_ilw, XmNvalueChangedCallback))
				XtCallCallbacks((Widget)_ilw, XmNvalueChangedCallback, &ilcs);
		break;

		case XmCR_ACTIVATE:
			if (XtHasCallbacks((Widget)_ilw, XmNactivateCallback))
				XtCallCallbacks((Widget)_ilw, XmNactivateCallback, &ilcs);
		break;
	}
}

#define MODE(a,b) ((a)->request_mode & (b))

static XtGeometryResult queryGeometry(XmIconListWidget _ilw, XtWidgetGeometry *_request, XtWidgetGeometry *_pref)
{
	if (CLIP(_ilw))
		_pref->height = HEIGHT(CLIP(_ilw));
	else
		_pref->height = HEIGHT(_ilw);

   _pref->border_width = BORDER(_ilw);
   _pref->width = WIDTH(_ilw);

	return XtGeometryYes;
}

static void expose(XmIconListWidget _ilw, XEvent *_event, Region _region)
{
	XRectangle xrect;

	XClipBox(_region, &xrect);
	if (NUM_ELEMENTS(_ilw) <= 0)
		return;

	drawIconList(_ilw, &xrect);
	/* drawElementBackground(_ilw, FOCUS_EL(_ilw), SELECT_GC(_ilw));  */
}

static void resize(XmIconListWidget _ilw)
{
/*
	XRectangle xrect;

	if (!XtIsRealized((Widget)_ilw))
		return;

	xrect.x = 0;
	xrect.y = 0;
	xrect.width = WIDTH(_ilw);
	xrect.height = HEIGHT(_ilw);

	if (NUM_ELEMENTS(_ilw) <= 0)
		return;
*/
}

static int getFirstVisibleElement(XmIconListWidget _ilw)
{
	int offset;

	if (CORE(_ilw).y < 0)
		offset = (-CORE(_ilw).y)/FULL_EL_HEIGHT(_ilw);
	else
		offset = 0;

	return offset;
}

static Boolean rowFullyVisible(XmIconListWidget _ilw, int _row, Boolean *_aboveTop)
{
	Boolean visible;

	visible = FALSE;
	if (_row < START_ROW(_ilw))
		*_aboveTop = TRUE;
	else
	if (_row > END_ROW(_ilw))
		*_aboveTop = FALSE;
	else
		visible = TRUE;

	return visible;
}

static void syncScrollBar(XmIconListWidget _ilw)
{
	Arg warg[7];
	int diff, n, size;

	if (VBAR(_ilw))
	{
		if (NUM_ELEMENTS(_ilw))
		{
			diff = END_ROW(_ilw) - START_ROW(_ilw) + 1;
			size = NUM_ELEMENTS(_ilw);
		}
		else
		{
			diff = 1;
			size = 1;
		}

		n = 0;
		XtSetArg(warg[n], XmNminimum, 0); n++;
		XtSetArg(warg[n], XmNmaximum, size); n++;
		XtSetArg(warg[n], XmNincrement, 1); n++;
		if (NUM_ELEMENTS(_ilw))
		{
			if (FOCUS_EL(_ilw) == -1)
				XtSetArg(warg[n], XmNvalue, 0);
			else
				XtSetArg(warg[n], XmNvalue, FOCUS_EL(_ilw)); 
			n++;
		}
		else
		{
			XtSetArg(warg[n], XmNvalue, 0); n++;
		}
		if (NUM_ELEMENTS(_ilw) > diff)
		{
			
			XtSetArg(warg[n], XmNsliderSize, diff); n++;
		}
		else
		{
			XtSetArg(warg[n], XmNsliderSize, size); n++;
		}
		XtSetValues(VBAR(_ilw), warg, n);
	}
}

static void syncToClip(XmIconListWidget _ilw)
{
	int offset;

	if (CLIP(_ilw) == NULL)
		return;

	offset = FULL_EL_HEIGHT(_ilw) - 1;

	START_ROW(_ilw) = (-CORE(_ilw).y)/FULL_EL_HEIGHT(_ilw);
	END_ROW(_ilw) = START_ROW(_ilw) + HEIGHT(CLIP(_ilw))/FULL_EL_HEIGHT(_ilw) - 1;
	if (END_ROW(_ilw) >= NUM_ELEMENTS(_ilw))
		END_ROW(_ilw) = NUM_ELEMENTS(_ilw)-1;

#if 0
	if (VBAR(_ilw))
	{
		if ( (START_ROW(_ilw) == 0) && (END_ROW(_ilw) == (NUM_ELEMENTS(_ilw)-1)))
			XtUnmanageChild(VBAR(_ilw));
		else
			XtManageChild(VBAR(_ilw));
	}
#endif


}

static void scrollBarSetValue(XmIconListWidget _ilw, Widget _sb, int _value, Boolean _erase)
{
	Position x, y;
	Boolean aboveTop;
	Boolean visible, nextVisible;
	int nextStart, nextEnd, offset;

	if (_sb != VBAR(_ilw))
		return;

	y = -(_value * FULL_EL_HEIGHT(_ilw));
	x = CORE(_ilw).x;

	if (CLIP(_ilw))
	{
		visible = rowFullyVisible(_ilw, FOCUS_EL(_ilw), &aboveTop);

		offset = FULL_EL_HEIGHT(_ilw) - 1;

		nextStart = y/FULL_EL_HEIGHT(_ilw);

		if (nextStart == START_ROW(_ilw))
			return;

		nextEnd = nextStart + HEIGHT(CLIP(_ilw))/FULL_EL_HEIGHT(_ilw) - 1;
		if (nextEnd >= NUM_ELEMENTS(_ilw))
			nextEnd = NUM_ELEMENTS(_ilw)-1;

		nextVisible = ( (FOCUS_EL(_ilw) >= START_ROW(_ilw)) && (FOCUS_EL(_ilw) <= END_ROW(_ilw)));

		if (_erase && !visible && !nextVisible)
			highlightElement(_ilw, FOCUS_EL(_ilw), ERASE_GC(_ilw));
	}

	MOVE(_ilw, x, y);
	syncToClip(_ilw);

/*
	if (nextVisible)
		highlightElement(_ilw, FOCUS_EL(_ilw), HIGHLIGHT_GC(_ilw));
*/

/* 	callCallbacks(_ilw, XmCR_VALUE_CHANGED); */

/*
	if (CLIP(_ilw) && !(visible && nextVisible))
		highlightElement(_ilw, FOCUS_EL(_ilw), HIGHLIGHT_GC(_ilw));
*/
}

static void clipCB(Widget _clip, XtPointer _data, XtPointer _call)
{
	XmIconListWidget ilw;
	Dimension height;
	XmDumbClipCallbackStruct *dccs = (XmDumbClipCallbackStruct *)_call;

	ilw = (XmIconListWidget)_data;

	if (dccs->reason == XmCR_RESIZE)
	{
		height = MAX(HEIGHT(CLIP(ilw)), NUM_ELEMENTS(ilw)*FULL_EL_HEIGHT(ilw));
		height -= 2 * BORDER(ilw);
		RESIZE(ilw, WIDTH(CLIP(ilw))-2*BORDER(ilw), height, BORDER(ilw));
		syncToClip(ilw);
	}
	syncScrollBar(ilw);
}

static void scrollBarCB(Widget _sb, XtPointer _data, XtPointer _call)
{
	XmScrollBarCallbackStruct *sbcs = (XmScrollBarCallbackStruct *)_call;
	XmIconListWidget ilw = (XmIconListWidget)_data;

	scrollBarSetValue(ilw, _sb, sbcs->value, sbcs->reason == XmCR_DRAG);
}

static void scrollToRow(XmIconListWidget _ilw, int _row, Boolean _jump, Boolean _toTop)
{
	int rowsPerClip;
	int value, ss, inc, pi;
	int newRow;
	Boolean fromTop;

	if (CLIP(_ilw) == NULL)
		return;

	if (!_toTop && rowFullyVisible(_ilw, _row, &fromTop))
		return;

	rowsPerClip = HEIGHT(CLIP(_ilw))/FULL_EL_HEIGHT(_ilw);

	if (_toTop)
	{
		if (_row >= rowsPerClip)
		{
			if (((NUM_ELEMENTS(_ilw) - _row) - 1) < rowsPerClip)
				newRow = NUM_ELEMENTS(_ilw) - rowsPerClip;
			else
				newRow = _row;
		}
		else
		{
			if (NUM_ELEMENTS(_ilw) > rowsPerClip)
				newRow = _row; /* 0; */
			else
				newRow = 0;
		}
	}
	else
	if (fromTop)
		newRow = _row;
	else
	{
		newRow = _row - (rowsPerClip-1);
		if (newRow < 0)
			newRow = 0;
	}

	XmScrollBarGetValues(VBAR(_ilw), &value, &ss, &inc, &pi);
	XmScrollBarSetValues(VBAR(_ilw), newRow, ss, inc, pi, TRUE);

	/* scrollBarSetValue(_ilw, VBAR(_ilw), newRow, FALSE); */
}

static int yToElementRow(XmIconListWidget _ilw, Position _y)
{
	int retElement;

	retElement = _y/FULL_EL_HEIGHT(_ilw);

	if (retElement > (NUM_ELEMENTS(_ilw)-1))
		return -1;
	else
		return retElement;
}

static void activate(Widget _w, XEvent *_event, String *_args, Cardinal *_numArgs)
{
	int thisRow;
	XmIconListWidget ilw;
	int multiTime;
	Boolean activate;

	ilw = (XmIconListWidget)_w;
	if (!XmIsIconList(_w))
		return;

	if (ELEMENTS(ilw) == NULL)
		return;

	thisRow = yToElementRow(ilw, _event->xbutton.y);
	if ( (thisRow < 0) || (thisRow > NUM_ELEMENTS(ilw)))
	{
		LAST_CLICK_ROW(ilw) = -1;
		CLICK_COUNT(ilw) = 0;
		return;
	}

	activate = FALSE;
	if (_event->type == ButtonRelease)
	{
		if (CLICK_COUNT(ilw) == 0)
		{
			LAST_CLICK_ROW(ilw) = thisRow;
			CLICK_COUNT(ilw)++;
		}
		else
		if (thisRow == LAST_CLICK_ROW(ilw))
		{
			multiTime = XtGetMultiClickTime(XtDisplay(_w));
			if ( (_event->xbutton.time - LAST_CLICK_TIME(ilw)) <= multiTime)
			{
				CLICK_COUNT(ilw) = 0;
				activate = TRUE;
			}
			else
				CLICK_COUNT(ilw) = 1;
		}
		LAST_CLICK_TIME(ilw) = _event->xbutton.time;
		LAST_CLICK_ROW(ilw) = thisRow;
	}
	else
		activate = TRUE;

	if (!activate)
		return;

	scrollToRow(ilw, FOCUS_EL(ilw), FALSE, FALSE);

	callCallbacks(ilw, XmCR_ACTIVATE, _event);
	/* printf("Activate\n"); */
	
}

static void arm(Widget _w, XEvent *_event, String *_args, Cardinal *_numArgs)
{
	XmIconListWidget ilw = (XmIconListWidget)_w;
	int thisElement;

	if (!XmIsIconList(ilw) || (ELEMENTS(ilw) == NULL))
		return;

	XmProcessTraversal(_w, XmTRAVERSE_CURRENT);

	if (_event->type == ButtonPress)
	{
		thisElement = yToElementRow(ilw, _event->xbutton.y);
		setFocusElement(ilw, thisElement, FALSE);
	}
}

static void setFocusElement(XmIconListWidget _ilw, int _row, Boolean _toTop)
{
	Boolean above;
	if ( _row == -1)
		return;

	if ( (_row != FOCUS_EL(_ilw)) || _toTop || !rowFullyVisible(_ilw, _row, &above))
	{
   		if (FOCUS_EL(_ilw) != -1)
	   		drawIconListElement(_ilw, FOCUS_EL(_ilw), ERASE_GC(_ilw), ERASE_GC(_ilw));
   		FOCUS_EL(_ilw) =_row;
		scrollToRow(_ilw, _row, FALSE, _toTop);
   		drawIconListElement(_ilw, FOCUS_EL(_ilw), SELECT_GC(_ilw), 
						HAS_FOCUS(_ilw) ? HIGHLIGHT_GC(_ilw) : ERASE_GC(_ilw));

	}
	callCallbacks(_ilw, XmCR_VALUE_CHANGED, NULL);
}

static void armAndActivate(Widget _w, XEvent *_event, String *_args, Cardinal *_numArgs)
{
	arm(_w, _event, _args, _numArgs);
	activate(_w, _event, _args, _numArgs);
}

static void moveFocusElement(XmIconListWidget _ilw, int _whence, XEvent *_event)
{
	int next;

	if (_whence == 0)
		return;

	if (FOCUS_EL(_ilw) != -1)
		next = _whence + FOCUS_EL(_ilw);
	else
		next = _whence;

	if (next < 0)
		next = 0;
	else
	if (next >= NUM_ELEMENTS(_ilw))
		next = NUM_ELEMENTS(_ilw)-1;

	if (FOCUS_EL(_ilw) == next)
		return;

	if (FOCUS_EL(_ilw) != -1)
		drawIconListElement(_ilw, FOCUS_EL(_ilw), ERASE_GC(_ilw), ERASE_GC(_ilw));

	FOCUS_EL(_ilw) = next;
	scrollToRow(_ilw, FOCUS_EL(_ilw), TRUE, FALSE);
	drawIconListElement(_ilw, FOCUS_EL(_ilw), SELECT_GC(_ilw), HIGHLIGHT_GC(_ilw));

	callCallbacks(_ilw, XmCR_VALUE_CHANGED, _event);
}

static void up(Widget _w, XEvent *_event, String *_args, Cardinal *_numArgs)
{
	XmIconListWidget ilw;
	int move;

	ilw = (XmIconListWidget)_w;

	if (!XmIsIconList(ilw) || (ELEMENTS(ilw) == NULL))
		return;

	if (FOCUS_EL(ilw) == 0)
		return;

	move = -1;
	if ((*_numArgs) == 1)
	{
		if (strcmp(_args[0], "PAGE") == 0)
			move = -VIS_COUNT(ilw);
	}

	if ((Widget)ilw != _w)
	{
		Time ltime = XtLastTimestampProcessed(XtDisplay(ilw));
	/* 	XmProcessTraversal((Widget)ilw, XmTRAVERSE_PREV_TAB_GROUP); */
		XmProcessTraversal((Widget)ilw, XmTRAVERSE_CURRENT);
		XtCallAcceptFocus((Widget)ilw, &ltime);
	}

	moveFocusElement(ilw, move, _event);
}

static void down(Widget _w, XEvent *_event, String *_args, Cardinal *_numArgs)
{
	XmIconListWidget ilw;
	int move;

	ilw = (XmIconListWidget)_w;

	if (!XmIsIconList(ilw) || (ELEMENTS(ilw) == NULL))
		return;

	if ( (FOCUS_EL(ilw)+1) == NUM_ELEMENTS(ilw))
		return;

	move = 1;
	if ((*_numArgs) == 1)
	{
		if (strcmp(_args[0], "PAGE") == 0)
			move = VIS_COUNT(ilw);
	}

	if ((Widget)ilw != _w)
	{
		XmProcessTraversal((Widget)ilw, XmTRAVERSE_CURRENT);
		XmProcessTraversal((Widget)ilw, XmTRAVERSE_CURRENT);
		/* XtSetKeyboardFocus((Widget)ilw, (Widget)ilw); */
	}

	moveFocusElement(ilw, move, _event);
}

static void buttonMotionEH(Widget _w, XtPointer _data, XEvent *_event, Boolean *_cont)
{
	int row;
	XmIconListWidget ilw = (XmIconListWidget)_w;

	if (!XmIsIconList(ilw) || (ELEMENTS(ilw) == NULL))
		return;

	row = yToElementRow(ilw, _event->xmotion.y);
	if ((row < 0) || (row == FOCUS_EL(ilw)))
		return;

	setFocusElement(ilw, row, FALSE);

}

static XmNavigability widgetNavigable(Widget _w)
{
	XmIconListWidget ilw = (XmIconListWidget)_w;

	if( XtIsSensitive(_w) && PRIM(ilw).traversal_on)
		return XmTAB_NAVIGABLE;

	return XmNOT_NAVIGABLE;
}



static void focusChange(Widget _w, XmFocusChange _fc)
{  
	XmIconListWidget ilw = (XmIconListWidget)_w;

	/* Enter/Leave is called only in pointer mode,
	 * Focus in/out only called in explicit mode.
	 */
	switch(	_fc	)
	{
		case XmENTER:
			if(!ICON_LIST(ilw).highlightOnEnter)
				break;
		/* 	Drop through. */
		case XmFOCUS_IN:
			HAS_FOCUS(ilw) = TRUE;
			if ( (FOCUS_EL(ilw) == -1) && NUM_ELEMENTS(ilw))
				FOCUS_EL(ilw) = 0;
			else
			if (NUM_ELEMENTS(ilw) == 0)
				FOCUS_EL(ilw) = -1;
			drawIconListElement(ilw, FOCUS_EL(ilw), SELECT_GC(ilw), HIGHLIGHT_GC(ilw));
			break;
		case XmLEAVE:
			if (!ICON_LIST(ilw).highlightOnEnter)
				break;
			/* Drop through. */
		case XmFOCUS_OUT:
			HAS_FOCUS(ilw) = FALSE;
			drawIconListElement(ilw, FOCUS_EL(ilw), SELECT_GC(ilw), ERASE_GC(ilw));
			break;
	}
	return;
}


static void destroy(XmIconListWidget _ilw)
{
}

#define drawShadow(wid,tgc,bgc,x,y,w,h) \
	_XmDrawShadows(XtDisplay(wid), XtWindow (wid), \
			tgc, bgc, \
			x, y, w, h, \
			ST(wid), XmSHADOW_OUT);

static void drawElementBackground(XmIconListWidget _ilw, int _nelement, GC _gc)
{
	Position y;
	Position x;

	if (_nelement < START_ROW(_ilw))
		return;

	y = _nelement * FULL_EL_HEIGHT(_ilw) + HT(_ilw);
	x = HT(_ilw);

	XFillRectangle(XtDisplay(_ilw), XtWindow(_ilw), _gc,
				x, y, WIDTH(_ilw)-2*HT(_ilw), FULL_EL_HEIGHT(_ilw)-2*HT(_ilw));
}

static void highlightElement(XmIconListWidget _ilw, int _row, GC _gc)
{
	Position y;
	Position x;
	Boolean aboveTop;

	if (!XtIsRealized((Widget)_ilw))
		return;

	y = _row * FULL_EL_HEIGHT(_ilw);
	x = 0;

	if ( (_row != FOCUS_EL(_ilw)) || (rowFullyVisible(_ilw, FOCUS_EL(_ilw), &aboveTop)))
		_XmDrawSimpleHighlight( XtDisplay(_ilw), XtWindow(_ilw),
				_gc, x, y,
				WIDTH(_ilw), FULL_EL_HEIGHT(_ilw),
				HT(_ilw));
	else
	if (_row != -1)
		_XmDrawSimpleHighlight( XtDisplay(_ilw), XtWindow(_ilw),
				_gc, x, START_ROW(_ilw)*FULL_EL_HEIGHT(_ilw),
				WIDTH(_ilw), ((END_ROW(_ilw)-START_ROW(_ilw))+1)*FULL_EL_HEIGHT(_ilw),
				HT(_ilw));
	else
		_XmDrawSimpleHighlight( XtDisplay(_ilw), XtWindow(_ilw),
				_gc, 0, 0, WIDTH(CLIP(_ilw))-2*BORDER(_ilw), HEIGHT(CLIP(_ilw))-2*BORDER(_ilw),
				HT(_ilw));
}

static void drawText(XmIconListWidget _ilw, GC _gc, Position _x, Position _y, const char *_string, int _len)
{
	XDrawString(XtDisplay(_ilw), XtWindow(_ilw), _gc, _x, _y + THE_FONT(_ilw)->ascent, (char *)_string, _len);
}


static void drawIconListElement(XmIconListWidget _ilw, int _row, GC _backgroundGC, GC _highlightGC)
{
	Position startY, xpos, yOffset;
	IconListElement *thisElement;
	XGCValues gcValues;

	if (!XtIsRealized((Widget)_ilw))
		return;

	if (_row == -1)
	{
		highlightElement(_ilw, _row, _highlightGC);
		return;
	}

	thisElement = ELEMENTS(_ilw)+_row;

	startY = _row*FULL_EL_HEIGHT(_ilw);
	yOffset = (FULL_EL_HEIGHT(_ilw) - thisElement->iconPixmap.height)/2;
	xpos = HT(_ilw);

	drawElementBackground(_ilw, _row, _backgroundGC);
	highlightElement(_ilw, _row, _highlightGC);

	if (thisElement->iconPixmap.isBitmap)
		gcValues.clip_mask = thisElement->iconPixmap.pixmap;
	else
		gcValues.clip_mask = thisElement->iconPixmap.mask;

	gcValues.clip_y_origin = startY + yOffset; 
	gcValues.clip_x_origin = xpos;

	XChangeGC(XtDisplay(_ilw), COPY_GC(_ilw), 
		GCClipMask | GCClipXOrigin | GCClipYOrigin, &gcValues);

	if (!thisElement->iconPixmap.isBitmap)
		XCopyArea(XtDisplay(_ilw), thisElement->iconPixmap.pixmap, XtWindow(_ilw), 
			COPY_GC(_ilw), 0, 0, thisElement->iconPixmap.width, 
			thisElement->iconPixmap.height,
			xpos, startY+yOffset);
	else
		XCopyPlane(XtDisplay(_ilw), thisElement->iconPixmap.pixmap, XtWindow(_ilw), 
			COPY_GC(_ilw), 0, 0, thisElement->iconPixmap.width, thisElement->iconPixmap.height,
			xpos, startY+yOffset, 1);

	xpos += MARG_W(_ilw) + thisElement->iconPixmap.width + SPACE;

	startY += TEXT_OFFSET(_ilw);
	drawText(_ilw, THE_GC(_ilw), xpos, startY, thisElement->string, strlen(thisElement->string));

}

static void drawIconList(XmIconListWidget _ilw, XRectangle *_xrect)
{
	int beginElement, endElement;
	int i;

	if (!XtIsRealized((Widget)_ilw))
		return;

	beginElement = _xrect->y/FULL_EL_HEIGHT(_ilw);
	endElement = beginElement + (_xrect->height+(FULL_EL_HEIGHT(_ilw)-1))/FULL_EL_HEIGHT(_ilw);

	if (beginElement >= NUM_ELEMENTS(_ilw))
	{
		beginElement = -1;
		endElement = -2;
	}
	else
	if (endElement > (NUM_ELEMENTS(_ilw)-1))
		endElement = NUM_ELEMENTS(_ilw) - 1;

	for(i=beginElement;i<=endElement;i++)
	{
		/* printf("drawing %d\n", i); */
		if ( (i == FOCUS_EL(_ilw))) /*  && HAS_FOCUS(_ilw)) */
			drawIconListElement(_ilw, i, SELECT_GC(_ilw), HAS_FOCUS(_ilw) ? HIGHLIGHT_GC(_ilw) : ERASE_GC(_ilw));
		else
			drawIconListElement(_ilw, i, ERASE_GC(_ilw), ERASE_GC(_ilw));
	}

	if (CLIP(_ilw) && HAS_FOCUS(_ilw))
		highlightElement(_ilw, FOCUS_EL(_ilw), HIGHLIGHT_GC(_ilw));
}

void XmIconListSetItems(Widget _iconList, IconListElement *_elements, int _count)
{
	XmIconListWidget ilw;

	ilw = (XmIconListWidget)_iconList;

	ELEMENTS(ilw) = _elements;
	NUM_ELEMENTS(ilw) = _count;
	FOCUS_EL(ilw) = -1;

	if (XtIsRealized((Widget)ilw))
		XClearArea(XtDisplay(ilw), XtWindow(ilw), 0, 0, WIDTH(ilw), HEIGHT(ilw), TRUE);

	if (CLIP(ilw))
	{
		RESIZE(ilw, WIDTH(CLIP(ilw)), MAX(FULL_EL_HEIGHT(ilw) * _count, HEIGHT(CLIP(ilw))), 0);
		MOVE(ilw, 0, 0);
		syncToClip(ilw);
	}

	/* purify_watch(&HEIGHT(ilw), "w"); */

	syncScrollBar(ilw);
}



Widget XmCreateScrolledIconList(Widget _parent, const char *_name, ArgList _args, Cardinal _numArgs)
{
	Widget sw, bb, clip;
	Widget vBar;
	XmIconListWidget ilw;
	Arg warg[7];
	Cardinal n;

	n = 0;
	XtSetArg(warg[n], XmNscrollingPolicy, XmAPPLICATION_DEFINED); n++;
	sw = XtCreateWidget(_name, xmScrolledWindowWidgetClass, _parent, warg, n);

	n = 0;
	XtSetArg(warg[n], XmNorientation, XmVERTICAL); n++;
	XtSetArg(warg[n], XmNminimum, 0); n++;
	XtSetArg(warg[n], XmNmaximum, 100); n++;
	vBar = XtCreateManagedWidget("_vscroll", xmScrollBarWidgetClass, sw, warg, n);

	n = 0;
	XtSetArg(warg[n], XmNshadowType, XmSHADOW_IN); n++;
	bb = XtCreateWidget("_border", xmFrameWidgetClass, sw, warg, n);

	n = 0;
	XtSetArg(warg[n], XmNshadowThickness, 0); n++;
	XtSetArg(warg[n], XmNmarginWidth, 0); n++;
	XtSetArg(warg[n], XmNmarginHeight, 0); n++;
	XtSetArg(warg[n], XmNchildType, XmFRAME_WORKAREA_CHILD); n++;
	clip = XtCreateManagedWidget("_clip", xmDumbClipWidgetClass, bb, warg, n);

	ilw = (XmIconListWidget)XtCreateManagedWidget("_iconList", xmIconListWidgetClass, clip, 
				_args, _numArgs);

	n = 0;
	XtSetArg(warg[n], XtNborderWidth, 0); n++;
	XtSetArg(warg[n], XtNbackground, CORE(ilw).background_pixel); n++;
	XtSetArg(warg[n], XmNx, 0); n++;
	XtSetArg(warg[n], XmNy, 0); n++;
	XtSetArg(warg[n], XmNwidth, WIDTH(ilw)+2*BORDER(ilw)); n++;
	XtSetArg(warg[n], XmNheight, (FULL_EL_HEIGHT(ilw) * VIS_COUNT(ilw))+2*BORDER(ilw)); n++;
	XtSetValues(clip, warg, n);

	n = 0;
	XtSetArg(warg[n], XmNbackground, CORE(ilw).background_pixel); n++;
	XtSetArg(warg[n], XmNshadowThickness, ST(ilw)); n++;
	XtSetValues(bb, warg, n);

	XtManageChild(bb);

	XmScrolledWindowSetAreas(sw, NULL, vBar, (Widget)bb);

	VBAR(ilw) = vBar;
	CLIP(ilw) = clip;
	SW(ilw) = sw;

	XtAddCallback(clip, XmNresizeCallback, (XtCallbackProc)clipCB, (XtPointer)ilw);

	XtAddCallback(VBAR(ilw), XmNvalueChangedCallback, (XtCallbackProc)scrollBarCB, (XtPointer)ilw);
	XtAddCallback(VBAR(ilw), XmNdragCallback, (XtCallbackProc)scrollBarCB, (XtPointer)ilw);

	XtManageChild(sw);

	return (Widget)ilw;
}

Widget XmIconListGetScrolledWindow(Widget _w)
{
	XmIconListWidget ilw;

	ilw = (XmIconListWidget)_w;
	return SW(ilw);
}

void XmIconListScrollToRow(Widget _w, int _row, Boolean _toTop, Boolean _makeFocus, Boolean _callCallbacks)
{
	XmIconListWidget ilw;
	Boolean above;

	ilw = (XmIconListWidget)_w;

	if ( (_row < 0) || (_row >= NUM_ELEMENTS(ilw)))
		return;

	if (_makeFocus && HAS_FOCUS(ilw) && FOCUS_EL(ilw) != -1 && 
		!rowFullyVisible(ilw, FOCUS_EL(ilw), &above))
		highlightElement(ilw, FOCUS_EL(ilw), ERASE_GC(ilw));

	CALL_CALLBACKS(ilw) = _callCallbacks;

	if (_makeFocus)
		setFocusElement(ilw, _row, _toTop);

	CALL_CALLBACKS(ilw) = TRUE;

/*	
	if (_makeFocus && HAS_FOCUS(ilw) && (FOCUS_EL(ilw) != -1))
		highlightElement(ilw, FOCUS_EL(ilw), HIGHLIGHT_GC(ilw));
*/
}
