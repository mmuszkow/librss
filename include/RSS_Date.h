#ifndef __RSS_DATE_H__
#define __RSS_DATE_H__

static const RSS_char days_names[7][4] = 
{
	RSS_text("Mon"), 
	RSS_text("Tue"),
	RSS_text("Wed"),
	RSS_text("Thu"),
	RSS_text("Fri"),
	RSS_text("Sat"),
	RSS_text("Sun")
};

static const RSS_char months_names[12][4] = 
{
	RSS_text("Jan"),
	RSS_text("Feb"),
	RSS_text("Mar"),
	RSS_text("Apr"),
	RSS_text("May"),
	RSS_text("Jun"),
	RSS_text("Jul"),
	RSS_text("Aug"),
	RSS_text("Sep"),
	RSS_text("Oct"),
	RSS_text("Nov"),
	RSS_text("Dec")
};

typedef struct timezone_diff
{
	RSS_char	name[4];
	int			diff;
} timezone_diff;

#define	RSS_TIMEZONES_LEN 35

static const timezone_diff timezones[RSS_TIMEZONES_LEN] =
{
	{ RSS_text("A"), 1 },
    { RSS_text("B"), 2 },
    { RSS_text("C"), 3 },
    { RSS_text("D"), 4 },
    { RSS_text("E"), 5 },
    { RSS_text("F"), 6 },
    { RSS_text("G"), 7 },
    { RSS_text("H"), 8 },
    { RSS_text("I"), 9 },
    { RSS_text("K"), 10 },
    { RSS_text("L"), 11 },
    { RSS_text("M"), 12 },
    { RSS_text("N"), -1 },
    { RSS_text("O"), -2 },
    { RSS_text("P"), -3 },
    { RSS_text("Q"), -4 },
    { RSS_text("R"), -5 },
    { RSS_text("S"), -6 },
    { RSS_text("T"), -7 },
    { RSS_text("U"), -8 },
    { RSS_text("V"), -9 },
    { RSS_text("W"), -10 },
    { RSS_text("X"), -11 },
    { RSS_text("Y"), -12 },
	{ RSS_text("Z"), 0 },
	{ RSS_text("UT"), 0 },
    { RSS_text("GMT"), 0 },
    { RSS_text("EST"), 5 },
    { RSS_text("EDT"), 4 },
    { RSS_text("CST"), 6 },
    { RSS_text("CDT"), 5 },
    { RSS_text("MST"), 7 },
    { RSS_text("MDT"), 6 },
	{ RSS_text("PST"), 8 },
	{ RSS_text("PDT"), 7 }
};

#endif
