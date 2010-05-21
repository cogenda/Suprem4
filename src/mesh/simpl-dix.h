/*
 * simpl-dix.h
 *	SIMPL-DIX global definitions.
 *
 * VERSION:
 *	Release 1, 10-15-1987
 *
 * Copyright (C) 1987  U. C. Berkeley SAMPLE Group
 */


/*	simpl-dix.h		Version 1.2		*/
/*	Last Modification:	10/11/89 05:56:19		*/

#ifndef	TRUE
#define	TRUE			1
#endif

#ifndef	FALSE
#define	FALSE			0
#endif

/*
 * Definitions for status flags.
 */

#define	NOT_SET			0
#define	SET			1
#define	RESET			2
#define	MODIFIED		3
#define	SAVED			4


/*
 * Definitions for file control flags.
 */

#define	READ			0
#define	WRITE			1

/*
 * Definitions for returned value.
 */

#define	ERROR			-1
#define	FAIL			0
#define	SUCCESS			1
#define	NOT_APPLY		2

#define	NO			0
#define	YES			1

#define	NOT_FOUND		-1


#define	Color			XColor
#define	CursorPtr		*Cursor




/*
 * Definitions for exit code.
 */

#define	ERROR_USAGE		-1
#define	ERROR_DISPLAY		-2
#define	ERROR_SYSTEM		-3
#define	ERROR_SIMPL		-4


/*
 * Definitions for size allocation.
 */

#define	BUFFER_SIZE		80
#define	CHAR_SIZE		80
#define	FIELD_SIZE		40
#define	FILE_SIZE		20
#define	FILENAME_SIZE		100
#define	MESSAGE_SIZE		180
#define	NAME_SIZE		5


/*
 * Character constants.
 */

#define	EOS			'\0'

#define	BS			010
#define	CR			015
#define	CNTLU			025
#define	CNTLX			030
#define	ESC			033
#define	DEL			0177


/*
 * Limiting constants.
 */

#define	INT_EXTREME		2100000000
#define	FLOAT_EXTREME		1e38


/*
 * Sleep time for prompting.
 */

#define	LONG_SLEEP		2
#define	SHORT_SLEEP		1


/*
 * Macro procedures.
 */

char *CheckPtr();

#define	Alloc(type)\
	((type *) CheckPtr((char *)malloc(sizeof(type))))

#define	AllocN(n, type)\
	((type *) CheckPtr((char *)malloc((unsigned) (n * sizeof(type)))))


#define	SwapInt(Int1, Int2)\
	{int IntTmp; IntTmp = Int1; Int1 = Int2; Int2 = IntTmp;}

#define	SwapFloat(Float1, Float2)\
	{float FloatTmp; FloatTmp = Float1; Float1 = Float2; Float2 = FloatTmp;}

#define	SwapPtr(Ptr1, Ptr2)\
	{char *PtrTmp; PtrTmp = Ptr1; Ptr1 = Ptr2; Ptr2 = PtrTmp;}


#define	loop		for(;;)
#define	not(A)		(A == TRUE ? FALSE : TRUE)


/*
 * Basic data types.
 */
#define Vertex XPoint

#define VertexRelative 0x0001
#define VertexDontDraw 0x0002
#define VertexCurved 0x0004
#define VertexStartClosed 0x0008
#define VertexEndClosed 0x0010
#define VertexDrawLastPoint 0x0020

typedef struct {
	float top, bottom;
	float left, right;
} floatBound;

typedef struct float_point {
	float x;
	float y;
} floatPoint;

typedef struct float_path {
	struct float_point point;
	struct float_path *next;
} floatPath;

typedef struct {
	int top, bottom;
	int left, right;
} intBound;

typedef struct int_point {
	int x;
	int y;
} intPoint;

typedef struct int_path {
	struct int_point point;
	struct int_path *next;
} intPath;


/*
 * System dependent information.
 */

#ifdef	BSD

#define	FD_MASK(p)	p

#endif


#ifdef	ULTRIX

#define	FD_MASK(p)	(p == (fd_set *) 0 ? NULL : (p)->fds_bits)

#ifndef	FD_SETSIZE
#define	FD_SETSIZE	32
#endif

#ifndef	FD_SET
#define	FD_SET(n, p)	(((p)->fds_bits[0]) |= (1 << n))
#endif

#ifndef	FD_ZERO
#define	FD_ZERO(p)	bzero((char *)(p), sizeof(*(p)))
#endif

#endif


#ifndef	FD_SETSIZE
#define	FD_SETSIZE	32
#endif

#ifndef	FD_SET
#define	FD_SET(n, p)	(((p)->fds_bits[0]) |= (1 << n))
#endif

#ifndef	FD_ZERO
#define	FD_ZERO(p)	bzero((char *)(p), sizeof(*(p)))
#endif
