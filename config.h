/* See LICENSE file for copyright and license details. */

/* appearance */
static unsigned int borderpx        = 2;        /* border pixel of windows */
static unsigned int snap            = 32;       /* snap pixel */
static const int swallowfloating    = 0;	/* 1 means swallow floating windows */
static const unsigned int gappih    = 5;        /* horiz inner gap between windows */
static const unsigned int gappiv    = 5;        /* vert inner gap between windows */
static const unsigned int gappoh    = 0;        /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 0;        /* vert outer gap between windows and screen edge */
static const int smartgaps          = 1;        /* 1 means no outer gap when there is only one window */
static int showbar                  = 1;        /* 0 means no bar */
static int topbar                   = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "ypn envypn", "Siji" };
static const char dmenufont[]       = "ypn envypn";
static char normbgcolor[]           = "#222222";
static char normbordercolor[]       = "#444444";
static char normfgcolor[]           = "#bbbbbb";
static char selfgcolor[]            = "#eeeeee";
static char selbordercolor[]        = "#005577";
static char selbgcolor[]            = "#005577";
static char *colors[][3] = {
       /*               fg           bg           border   */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
       [SchemeSel]  = { selfgcolor,  normbgcolor, selbordercolor  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ "Gimp",     NULL,     NULL,           0,         1,          0,           0,        -1 },
	{ "krita",    NULL,     NULL,           0,         1,          0,           0,        -1 },
	{ "firefox",  NULL,     NULL,           1 << 9,    0,          0,          -1,        -1 },
	{ "chromium", NULL,     NULL,           1 << 1,    0,          0,          -1,        -1 },
	{ "discord",  NULL,     NULL,           1 << 3,    0,          0,          -1,        -1 },
	{ "st",       NULL,     NULL,           0,         0,          1,           0,        -1 },
	{ NULL,       NULL,     "Event Tester", 0,         0,          0,           1,        -1 }, /* xev */
};

/* layout(s) */
static float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ KeyPress,   MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ KeyPress,   MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ KeyPress,   MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ KeyPress,   MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, NULL };
static const char *termcmd[]  = { "st", NULL };
static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = { "st", "-t", scratchpadname, "-g", "120x34", NULL };

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
		{ "normbgcolor",        STRING,  &normbgcolor },
		{ "normbordercolor",    STRING,  &normbordercolor },
		{ "normfgcolor",        STRING,  &normfgcolor },
		{ "selbgcolor",         STRING,  &selbgcolor },
		{ "selbordercolor",     STRING,  &selbordercolor },
		{ "selfgcolor",         STRING,  &selfgcolor },
		{ "borderpx",          	INTEGER, &borderpx },
		{ "snap",          	INTEGER, &snap },
		{ "showbar",          	INTEGER, &showbar },
		{ "topbar",          	INTEGER, &topbar },
		{ "nmaster",          	INTEGER, &nmaster },
		{ "resizehints",       	INTEGER, &resizehints },
		{ "mfact",       	FLOAT,   &mfact },
};

#include "movestack.c"
#include <X11/XF86keysym.h>
static Key keys[] = {
	/* type		modifier                     key               function        argument */
	{  KeyPress,	MODKEY,                      XK_p,             spawn,          {.v = dmenucmd } },
	{  KeyPress,	MODKEY|ShiftMask,            XK_Return,        spawn,          {.v = termcmd } },
	{  KeyPress,	MODKEY,                      XK_grave,         togglescratch,  {.v = scratchpadcmd } },
	{  KeyPress,	MODKEY,                      XK_b,             togglebar,      {0} },
	{  KeyPress,	MODKEY,                      XK_j,             focusstack,     {.i = +1 } },
	{  KeyPress,	MODKEY,                      XK_k,             focusstack,     {.i = -1 } },
	{  KeyPress,	MODKEY,                      XK_i,             incnmaster,     {.i = +1 } },
	{  KeyPress,	MODKEY,                      XK_d,             incnmaster,     {.i = -1 } },
	{  KeyPress,	MODKEY,                      XK_h,             setmfact,       {.f = -0.05} },
	{  KeyPress,	MODKEY,                      XK_l,             setmfact,       {.f = +0.05} },
	{  KeyPress,	MODKEY|ShiftMask,	     XK_j,             movestack,      {.i = +1 } },
	{  KeyPress,	MODKEY|ShiftMask,	     XK_k,             movestack,      {.i = -1 } },
	{  KeyPress,	MODKEY|Mod1Mask,             XK_h,             incrgaps,       {.i = +1 } },
	{  KeyPress,	MODKEY|Mod1Mask,             XK_l,             incrgaps,       {.i = -1 } },
	{  KeyPress,	MODKEY|Mod1Mask|ShiftMask,   XK_h,             incrogaps,      {.i = +1 } },
	{  KeyPress,	MODKEY|Mod1Mask|ShiftMask,   XK_l,             incrogaps,      {.i = -1 } },
	{  KeyPress,	MODKEY|Mod1Mask|ControlMask, XK_h,             incrigaps,      {.i = +1 } },
	{  KeyPress,	MODKEY|Mod1Mask|ControlMask, XK_l,             incrigaps,      {.i = -1 } },
	{  KeyPress,	MODKEY|Mod1Mask,             XK_0,             togglegaps,     {0} },
	{  KeyPress,	MODKEY|Mod1Mask|ShiftMask,   XK_0,             defaultgaps,    {0} },
	{  KeyPress,	MODKEY,                      XK_y,             incrihgaps,     {.i = +1 } },
	{  KeyPress,	MODKEY,                      XK_o,             incrihgaps,     {.i = -1 } },
	{  KeyPress,	MODKEY|ControlMask,          XK_y,             incrivgaps,     {.i = +1 } },
	{  KeyPress,	MODKEY|ControlMask,          XK_o,             incrivgaps,     {.i = -1 } },
	{  KeyPress,	MODKEY|Mod1Mask,             XK_y,             incrohgaps,     {.i = +1 } },
	{  KeyPress,	MODKEY|Mod1Mask,             XK_o,             incrohgaps,     {.i = -1 } },
	{  KeyPress,	MODKEY|ShiftMask,            XK_y,             incrovgaps,     {.i = +1 } },
	{  KeyPress,	MODKEY|ShiftMask,            XK_o,             incrovgaps,     {.i = -1 } },
	{  KeyPress,	MODKEY,                      XK_Return,        zoom,           {0} },
	{  KeyPress,	MODKEY,                      XK_Tab,           view,           {0} },
	{  KeyPress,	MODKEY|ShiftMask,            XK_c,             killclient,     {0} },
	{  KeyPress,	MODKEY,                      XK_t,             setlayout,      {.v = &layouts[0]} },
	{  KeyPress,	MODKEY,                      XK_f,             setlayout,      {.v = &layouts[1]} },
	{  KeyPress,	MODKEY,                      XK_m,             setlayout,      {.v = &layouts[2]} },
	{  KeyPress,	MODKEY,                      XK_space,         setlayout,      {0} },
	{  KeyPress,	MODKEY|ShiftMask,            XK_space,         togglefloating, {0} },
	{  KeyPress,	MODKEY,                      XK_s,             togglesticky,   {0} },
	{  KeyPress,	MODKEY,                      XK_0,             view,           {.ui = ~0 } },
	{  KeyPress,	MODKEY|ShiftMask,            XK_0,             tag,            {.ui = ~0 } },
	{  KeyPress,	MODKEY,                      XK_comma,         focusmon,       {.i = -1 } },
	{  KeyPress,	MODKEY,                      XK_period,        focusmon,       {.i = +1 } },
	{  KeyPress,	MODKEY|ShiftMask,            XK_comma,         tagmon,         {.i = -1 } },
	{  KeyPress,	MODKEY|ShiftMask,            XK_period,        tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                          0)
	TAGKEYS(                        XK_2,                          1)
	TAGKEYS(                        XK_3,                          2)
	TAGKEYS(                        XK_4,                          3)
	TAGKEYS(                        XK_5,                          4)
	TAGKEYS(                        XK_6,                          5)
	TAGKEYS(                        XK_7,                          6)
	TAGKEYS(                        XK_8,                          7)
	TAGKEYS(                        XK_9,                          8)
	{  KeyPress,    MODKEY|ShiftMask,            XK_q,             quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
