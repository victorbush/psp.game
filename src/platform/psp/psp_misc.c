/*=========================================================
INCLUDES
=========================================================*/

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "common.h"

/*=========================================================
TYPES
=========================================================*/

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

int fopen_s
	(
	FILE**      _Stream,
	char const* _FileName,
	char const* _Mode
	)
{
	*_Stream = fopen(_FileName, _Mode);
	return !_Stream;	
}

int fscanf_s
	(
	FILE*       const _Stream,
	char const* const _Format,
	...
	)
{
	return fscanf(_Stream, _Format);
}

double math_acos(double x) { return acos(x); }
double math_asin(double x) { return asin(x); }
double math_ceil(double x) { return ceil(x); }
double math_cos(double x) { return cos(x); }
double math_exp(double x) { return exp(x); }
double math_fmod(double x, double y) { return fmod(x, y); }
double math_log(double x) { return log(x); }
double math_log10(double x) { return log10(x); }
double math_modf(double x, double* y) { return modf(x, y); }
double math_tan(double x) { return tan(x); }

int sscanf_s
	(
	char const* const _Buffer,
	char const* const _Format,
	...
	)
{
	return sscanf(_Buffer, _Format);
}

int strcpy_s
	(
	char*       _Destination,
	size_t     _SizeInBytes,
	char const* _Source
	)
{
	strcpy(_Destination, _Source);
	return 0;
}

int strncpy_s
	(
	char*       _Destination,
	size_t     _SizeInBytes,
	char const* _Source,
	size_t     _MaxCount
	)
{
	strncpy(_Destination, _Source, _MaxCount);
	return 0;
}