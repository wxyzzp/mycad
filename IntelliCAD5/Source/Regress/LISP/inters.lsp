;=====================================================================
;;$Workfile: inters.lsp $
; Test cases for (inters).
; Copyright (C) 1998 Visio Corporation. All rights reserved.
;
;
; ABSTRACT
;	This file contains test cases for (inters).  Compare results
;	against the expected results given in [square brackets].
;
;	If the symbol VERBOSE is defined, all results will be written
;	to the command line.  If this symbol is not defined, only
;	results which do not match the expected results will be displayed.
;    
;
; RELATED FILES
;	none.
;
;=====================================================================
; REVISION HISTORY
;
;;$Log: inters.lsp,v $
;;Revision 1.1  2002/04/21 11:10:36  vittals
;;no message
;;
; 
; 1     11/05/98 1:00p Michaelhu
; Initial check-in.
; 
;
;=====================================================================
;=====================================================================
; yes-no
;
; Abstract -    Prints "yes" if the argument is T, "no" otherwise.
;
; Parameters
; 	arg			The test value.
;
; Return Value
; 	"yes"		If the test value is T.
;	"no"		If the test value is not T.
;
;Revisions:
;=====================================================================
(defun yes-no ( arg / )
	(if arg "yes" "no")
)
;=====================================================================
; format-point
;
; Abstract -    Formats two numbers into the format '(x,y)'.
;
; Parameters
; 	pt			A list representing a 2D point.
;
; Return Value
; 	ans			A string containing the formatted point.
;
;Revisions:
;=====================================================================
(defun format-point ( pt / )
	(strcat "(" (itoa (car pt)) "," (itoa (cadr pt)) ")")
)
;=====================================================================
; format-line
;
; Abstract -    Formats two points into the format '(x,y),(x,y)'.
;
; Parameters
; 	pt1			A list representing a 2D point.
; 	pt2			A list representing a 2D point.
;
; Return Value
; 	ans			A string containing the formatted line.
;
;Revisions:
;=====================================================================
(defun format-line ( pt1 pt2 / )
	(strcat (format-point pt1) "," (format-point pt2))
)
;=====================================================================
; format-result
;
; Abstract -    Formats a test result into the format
;	'(pt1),(pt2) type [expected]: actual'.
;
; Parameters
; 	pt1s		The start point of the first line.
; 	pt1e		The end point of the first line.
; 	pt2s		The start point of the second line.
; 	pt2e		The end point of the second line.
;	typ			The type; T for non-infinite, nil for infinite
;	ex			The expected result.
;	ans			The actual result.
;
; Return Value
; 	ans			A string containing the formatted line.
;
;Revisions:
;=====================================================================
(defun format-result ( pt1s pt1e pt2s pt2e typ ex ans / strtyp)
	(if typ
		(setq strtyp "non-infinite")
		(setq strtyp "infinite")
	)
	(strcat
		(format-line pt1s pt1e)
		" and "
		(format-line pt2s pt2e)
		" " strtyp " ["
		(yes-no ex)
		"]: "
		(yes-no ans)
	)
)
;=====================================================================
; not-nil
;
; Abstract -    Determines whether two lines intersect.
;
; Parameters
; 	tstval		The value to test for nilness
;
; Return Value
; 	T			If testval is something other than nil.
;	nil			If testval is nil.
;
;Revisions:
;=====================================================================
(defun not-nil ( tstval / )
	(/= nil tstval)
)
;=====================================================================
; test-inters
;
; Abstract -    Tests whether two lines intersect.  Checks for
;	an intersection both between the two lines and between two
;	infinite lines passing through the two lines' end points.
;
; Parameters
; 	pt1s		The start point of the first line.
; 	pt1e		The end point of the first line.
; 	pt2s		The start point of the second line.
; 	pt2e		The end point of the second line.
;	ex1			The expected result for the non-infinite test.
;	ex2			The expected result for the infinite test.
;
; Return Value
; 	none.
;
;Revisions:
;=====================================================================
(defun test-inters (pt1s pt1e pt2s pt2e ex1 ex2 / ans)
	(princ "\n")
	(setq ans (not-nil (inters pt1s pt1e pt2s pt2e)))
	;(if (or (/= ans ex1) (/= nil VERBOSE))
		(princ (strcat "\n" (format-result pt1s pt1e pt2s pt2e t ex1 ans)))
	;)
	(setq ans (not-nil (inters pt1s pt1e pt2s pt2e nil)))
	;(if (or (/= ans ex2) (/= nil VERBOSE))
		(princ (strcat "\n" (format-result pt1s pt1e pt2s pt2e nil ex2 ans)))
	;)
)
;=====================================================================
; main
;
; Abstract -    The main test routine.  Calls (test-inters) for a
;	series of lines that:
;		- intersect,
;		- would intersect if extended,
;		- share a single endpoint,
;		- are exactly equal,
;		- lie on the same line but do not overlay each other,
;		- lie on the same line and partially overlay each other,
;		- lie on the same line and share an endpoint,
;		- are parallel.
;
; Parameters
; 	none.
;
; Return Value
; 	none.
;
;Revisions:
;=====================================================================
(defun main ( / )
	;--------------------------------------------------
	; intersect.
	(test-inters '(1 1) '(9 9) '(4 1) '(4 5) t t)
	;--------------------------------------------------
	; would intersect if extended.
	(test-inters '(1 1) '(9 9) '(4 1) '(4 2) nil t)
	;--------------------------------------------------
	; share a single endpoint.
	(test-inters '(1 1) '(9 9) '(1 1) '(4 2) t t)
	;--------------------------------------------------
	; are exactly equal.
	(test-inters '(1 1) '(9 9) '(1 1) '(9 9) nil nil)
	;--------------------------------------------------
	; line on the same line but do not overlay each
	;	other.
	(test-inters '(1 1) '(9 9) '(10 10) '(14 14) nil nil)
	;--------------------------------------------------
	; line on the same line and partially overlay each
	;	other.
	(test-inters '(1 1) '(9 9) '(4 4) '(10 10) nil nil)
	;--------------------------------------------------
	; line on the same line and share an endpoint.
	(test-inters '(1 1) '(9 9) '(9 9) '(15 15) nil nil)
	;--------------------------------------------------
	; are parallel.
	(test-inters '(1 1) '(9 9) '(1 2) '(9 10) nil nil)
	(princ)
)


