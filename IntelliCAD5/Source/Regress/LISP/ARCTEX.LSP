;=====================================================================
;;$Workfile: ARCTEX.LSP $
; This routine creates text along an arc.
; Copyright (C) 1998 Visio Corporation. All rights reserved.
;
;
; ABSTRACT
;====  ARCTEXT.LSP  ====  rev: 12/13/87  ====================================
;      As the name implies, this routine will let you write text on an arc.
;      All your input from each run will become default values for
;      subsequent runs, so you can undo (type "U") and rerun (type "AT"),
;      changing values for character height, radius, or included angle 
;      until spacing looks good.  Any text style should work, but make sure
;      it is loaded with a height of 0.
;
;      This routine is offered as "shareware".  Feel free to use it and copy 
;      it for others.  Please do not delete this header.  If you find that it
;      has been particularly useful and would like to encourage the author in
;      similar endeavors, please send $19.00 to .........  Rich Perlman
;                                                          2409 Amity Avenue
;                                                          Gastonia, NC 28054
;                       For custom programming, call ...  (704) 867-1314
;----------------------------------------------------------------------------
;	modified by MichaelHu to remove all interactive aspects.
;    
;
; RELATED FILES
;	\\audrey\qa\automate\benchmarks\lisp\*.
;
;=====================================================================
; REVISION HISTORY
;
;;$Log: ARCTEX.LSP,v $
;;Revision 1.1  2002/03/11 07:06:04  vittals
;;
;;VS_3102002_1.txt
;;
; 
; 2     11/18/98 8:29a Michaelhu
; Modifications to make all routines run on both IntelliCAD and AutoCAD.
; 
; 1     11/11/98 9:31a Michaelhu
; Initial check-in.
; 
;
;=====================================================================
(defun at (st ht cp ra fd)
    (setvar "cmdecho" 0) 
	(if (= nil st) (setq st ""))
	(if (= nil ht) (setq ht (getvar "TEXTSIZE")))
	(if (= nil cp) (setq cp (list 0.0 0.0 0.0)))
	(if (= nil ra) (setq ra 0.0))
	(if (= nil fd) (setq fd 180.0))
    (setq sl (strlen st)
          fa (/ (* fd pi) -180.0)
          ai (/ fa (- sl 1))
          a0 (/ (- pi fa) 2.0)
          nn 0
          ss1 (ssadd)
    )
    (while (< nn sl)
        (setq ap (+ a0 (* nn ai))
              pt (polar cp ap ra)
              tr (- (/ (* 180.0 ap) pi) 90.0)
              nn (+ nn 1 )
        )
        (command "._text" "_m" pt ht tr (substr st nn 1))
        (setq ss1 (ssadd (entlast) ss1))
    )
    (setq st st)
)
;----  GETSTRING WITH DEFAULT PROMPT  ---------------------------------------
(defun gtst (prmpt currval dflt / fullstrg)
    (if (equal currval nil) (setq currval dflt))
    (setq response (getstring t (prompt (strcat prmpt " <"  currval ">: "))))
    (if (equal response "") (setq currval currval) response)
)
;----  GETREAL WITH DEFAULT PROMPT  -----------------------------------------
(defun gtrl (prmpt currval dflt / response)
    (if (equal currval nil) (setq currval dflt))
    (setq response (getstring (prompt 
                                 (strcat prmpt " <" (rtos currval) ">: "))))
    (if (equal response "") (setq currval currval) (atof response))
)
;----  GETPOINT WITH DEFAULT PROMPT  ----------------------------------------
(defun gtpt (prmpt currval dflt / response)
    (if (equal currval nil) (setq currval dflt))
    (setq response (getpoint (strcat prmpt " <" (rtos (car currval)) 
                                           "," (rtos (cadr currval)) ">: ")))
    (if (equal response nil) (setq currval currval) response)
)
(princ "\n\n ArcText loaded.  Usage:")
(princ "\n(at \"text\" text_ht '(x y z of center point) arc_radius included_angle_in_degrees)")
(princ)

