;  A2C.LSP V1.0 (Beta) CWE
;  CREATE A CRICLE WHERE AN ARC USED TO BE USING ARC'S CENTER, RADIUS, LAYER
;-----------------------------------------------------------------------------
; FUNCTIONS
; ~~~~~~~~~
; C:A2C
;
; GLOBALS
; ~~~~~~~
;
; REVISIONS
; ~~~~~~~~~
;
; DEPENDANCIES
; ~~~~~~~~~~~~
;
;
; COMMENTS
; ~~~~~~~~
;
(terpri)
(princ "\n    ***************************************************************")
(princ "\n    * Loading A2C.LSP.                                            *")
(princ "\n    *            Command is <A2C>                                 *")
(princ "\n    *                      Please wait...                         *")
(princ "\n    ***************************************************************")
(terpri)
;---------------------------------------------------------------------------

(defun c:a2c (/ e c r lyr tc)
;(defun c:a2c ()
  (setq tc (getvar "cmdecho"))
  (setvar "cmdecho" 0)
  (prompt "\nSelect arc to convert to circle: ")
  (setq e (entget (car (entsel))))
  (if (= (cdr (assoc 0 e)) "ARC")
    (progn
	    (setq c (trans (cdr (assoc 10 e)) 0 1))
	    (setq r (cdr (assoc 40 e)))
	    (setq lyr (cdr (assoc 8 e)))
	    (command ".erase" (cdr (assoc -1 e)) "")
	    (command ".circle" c r)
	    (command ".chprop" "L" "" "layer" lyr "")
    );progn
  );if arc
  (setvar "cmdecho" tc)
  (princ)
);c:a2c



;the foregoing courtesy of:

;**********************************************************************
;***  Cliff W. Estes                             cestes@seanet.com  ***
;***  BaseLine Technology                         ph (206)882-7317  ***
;***  15834 NE 67th Place                        fax (206)882-7327  *** 
;***  Redmond, WA  98052            http://www.basline.com/basline  *** 
;***   Home of BaseLine II, THE interactive hull fairing system     ***
;**********************************************************************
;   FREE BaseLine II Demo available at http://www.basline.com/basline
;   Shareware ScrBldr v1.0 available at http://www.basline.com/basline

(princ)
