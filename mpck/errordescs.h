
error_info errordescs[]={
	{ERR_NONE,		"NO",  "none"},
	{ERR_UBYTES, 		"UNB", "unidentified bytes"},
	{ERR_INCONSISTENT,	"INC", "inconsistent frame headers"},
	{ERR_INVALID,		"INV", "invalid header values"},
	{ERR_FRAMECRC,		"CRC", "CRC error"},
	{ERR_LONGFNAME,		"LFN", "filename too long"},
	{ERR_BADFNAME,		"BFN", "filename contains strange characters"},
	{ERR_NOFRAMES,		"NOF", "no frames found"},
	{ERR_LFRAMES,		"NEF", "not enough frames found"},
	{ERR_EMPTY,		"EMP", "empty file"}
};

#define errordescs_siz 10 
