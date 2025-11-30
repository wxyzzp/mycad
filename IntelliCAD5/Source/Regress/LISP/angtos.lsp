;=====================================================================
;;$Workfile: angtos.lsp $
; (angtos) tests.
; Copyright (C) 1998 Visio Corporation. All rights reserved.
;
;
; ABSTRACT
;	This routine tests (angtos) with a variety of angles, all units,
;	and precisions of 0 and 4.
;
;	Only errors are displayed to the screen.
;    
;
; RELATED FILES
;	None.
;
;=====================================================================
; REVISION HISTORY
;
;;$Log: angtos.lsp,v $
;;Revision 1.1  2002/04/21 11:10:36  vittals
;;no message
;;
; 
; 1     1/14/99 11:43a Michaelhu
; Initial check-in.
; 
;
;=====================================================================
;=====================================================================
; DTR
;
; Abstract -    Converts an angle from degrees to radians.
;
; Parameters
; 	a			The angle in degrees.
;
; Return Value
; 	rad			The angle in radians.
;
;Revisions:
;=====================================================================
(defun DTR (a)
  (* pi (/ a 180.0))
)
;=====================================================================
; do_test
;
; Abstract -    This routine runs the test.
;
; Parameters
; 	val			The value to test.
;	mode		The unit to use.
;	prec		The precision to use.
; 	ans			The expected answer.
;
; Return Value
; 	None.
;
;Revisions:
;=====================================================================
(defun do_test
	(
	val
	mode
	prec
	ans
	/
	)
	(if (/= ans (angtos val mode prec))
		(progn 
			(princ "\n(angtos ")(princ val)(princ " ")(princ mode)
				(princ " ")(princ prec)(princ ") [")(princ ans)
				(princ "] = ")
			(princ (angtos val mode prec))
		)
	)
)
;=====================================================================
; Main
;
; Abstract -    Runs the tests.  Only errors will be displayed.
;
; Parameters
; 	None.
;
; Return Value
; 	None.
;
;Revisions:
;=====================================================================
(defun c:main ( / test_val)
	;--------------------------------------------------
	; 0 deg.
	(setq test_val (dtr 0))
	(do_test test_val 0 0 "0")
	(do_test test_val 0 4 "0.0000")
	(do_test test_val 1 0 "0d")
	(do_test test_val 1 4 "0d0'0\"")
	(do_test test_val 2 0 "0g")
	(do_test test_val 2 4 "0.0000g")
	(do_test test_val 3 0 "0r")
	(do_test test_val 3 4 "0.0000r")
	(do_test test_val 4 0 "N 90d E")
	(do_test test_val 4 4 "E")
	;--------------------------------------------------
	; 90 deg.
	(setq test_val (dtr 90))
	(do_test test_val 0 0 "90")
	(do_test test_val 0 4 "90.0000")
	(do_test test_val 1 0 "90d")
	(do_test test_val 1 4 "90d0'0\"")
	(do_test test_val 2 0 "100g")
	(do_test test_val 2 4 "100.0000g")
	(do_test test_val 3 0 "2r")
	(do_test test_val 3 4 "1.5708r")
	(do_test test_val 4 0 "N 0d W")
	(do_test test_val 4 4 "N")
	;--------------------------------------------------
	; 180 deg.
	(setq test_val (dtr 180))
	(do_test test_val 0 0 "180")
	(do_test test_val 0 4 "180.0000")
	(do_test test_val 1 0 "180d")
	(do_test test_val 1 4 "180d0'0\"")
	(do_test test_val 2 0 "200g")
	(do_test test_val 2 4 "200.0000g")
	(do_test test_val 3 0 "3r")
	(do_test test_val 3 4 "3.1416r")
	(do_test test_val 4 0 "S 90d W")
	(do_test test_val 4 4 "W")
	;--------------------------------------------------
	; 270 deg.
	(setq test_val (dtr 270))
	(do_test test_val 0 0 "270")
	(do_test test_val 0 4 "270.0000")
	(do_test test_val 1 0 "270d")
	(do_test test_val 1 4 "270d0'0\"")
	(do_test test_val 2 0 "300g")
	(do_test test_val 2 4 "300.0000g")
	(do_test test_val 3 0 "5r")
	(do_test test_val 3 4 "4.7124r")
	(do_test test_val 4 0 "S 0d E")
	(do_test test_val 4 4 "S")
	;--------------------------------------------------
	; 360 deg.
	(setq test_val (dtr 360))
	(do_test test_val 0 0 "0")
	(do_test test_val 0 4 "0.0000")
	(do_test test_val 1 0 "0d")
	(do_test test_val 1 4 "0d0'0\"")
	(do_test test_val 2 0 "0g")
	(do_test test_val 2 4 "0.0000g")
	(do_test test_val 3 0 "0r")
	(do_test test_val 3 4 "0.0000r")
	(do_test test_val 4 0 "N 90d E")
	(do_test test_val 4 4 "E")
	;--------------------------------------------------
	; 45 deg.
	(setq test_val (dtr 45))
	(do_test test_val 0 0 "45")
	(do_test test_val 0 4 "45.0000")
	(do_test test_val 1 0 "45d")
	(do_test test_val 1 4 "45d0'0\"")
	(do_test test_val 2 0 "50g")
	(do_test test_val 2 4 "50.0000g")
	(do_test test_val 3 0 "1r")
	(do_test test_val 3 4 "0.7854r")
	(do_test test_val 4 0 "N 45d E")
	(do_test test_val 4 4 "N 45d0'0\" E")
	;--------------------------------------------------
	; -45 deg.
	(setq test_val (dtr -45))
	(do_test test_val 0 0 "315")
	(do_test test_val 0 4 "315.0000")
	(do_test test_val 1 0 "315d")
	(do_test test_val 1 4 "315d0'0\"")
	(do_test test_val 2 0 "350g")
	(do_test test_val 2 4 "350.0000g")
	(do_test test_val 3 0 "5r")
	(do_test test_val 3 4 "5.4978r")
	(do_test test_val 4 0 "S 45d E")
	(do_test test_val 4 4 "S 45d0'0\" E")
	;--------------------------------------------------
	; 42.5 deg.
	(setq test_val (dtr 42.5))
	(do_test test_val 0 0 "43")
	(do_test test_val 0 4 "42.5000")
	(do_test test_val 1 0 "42d")
	(do_test test_val 1 4 "42d30'0\"")
	(do_test test_val 2 0 "47g")
	(do_test test_val 2 4 "47.2222g")
	(do_test test_val 3 0 "1r")
	(do_test test_val 3 4 "0.7418r")
	(do_test test_val 4 0 "N 48d E")
	(do_test test_val 4 4 "N 47d30'0\" E")
	;--------------------------------------------------
	; 270 - 42.5 deg.
	(setq test_val (dtr (- 270.0 42.5)))
	(do_test test_val 0 0 "228")
	(do_test test_val 0 4 "227.5000")
	(do_test test_val 1 0 "228d")
	(do_test test_val 1 4 "227d30'0\"")
	(do_test test_val 2 0 "253g")
	(do_test test_val 2 4 "252.7778g")
	(do_test test_val 3 0 "4r")
	(do_test test_val 3 4 "3.9706r")
	(do_test test_val 4 0 "S 43d W")
	(do_test test_val 4 4 "S 42d30'0\" W")
	;--------------------------------------------------
	; all done.
	(princ "\n*** Test Complete ***")(princ)
)

