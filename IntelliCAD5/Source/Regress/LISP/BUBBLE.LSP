(prompt "\nType bubble 1 to run.....")
(defun C:BUBBLE	(/ la t1 pt1 pt2 pt3 lp txt)
  (initerr)
  (setvar "CMDECHO" 0)
  (setvar "BLIPMODE" 0)
  (setvar "OSMODE" OLDSNAP)
  (command "LAYER" "M" "0" "")
;;;*------------------------------------------------------
  (setq	t1  (getvar "USERR1")
	pt1 (upoint 1 "" "Pick Start Point" nil nil)
  ) ;_ end of setq
  (setq	pt2 (upoint 1 "" "To point" nil pt1)
	pt3 (polar pt1 (angle pt1 pt2) (* t1 2.5))
  )					;setq
  (setvar "OSMODE" 0)
  (command "PLINE" pt1 "W" "0" (* t1 0.8) pt3 "W" "0" "0" pt2)
;;;*----------------------------------------------------------
  (setvar "OSMODE" OLDSNAP)
  (while (setq pt2 (getpoint "\nTo point: " pt2))
    (command pt2)
  )					;end while
  (command "")
  (setvar "OSMODE" 0)
  (command "CHANGE" "L" "" "P" "C" "9" "")
;;;*----------------------------------------------------------
  (setq lp (getvar "LASTPOINT"))
  (command "INSERT" "*BUBBLE1" lp t1 "0" "TRIM" "L" "" lp "") ;command
  (setq txt (getstring "\nEnter Item Number: "))
  (command "TEXT" "J" "M" lp "" txt "LAYER" "M" la "")
;;;*---------------------------------------------------------
  (reset)
  (princ)
)					;*defun
(defun initerr ()
  (setq oldlayer (getvar "clayer"))
  (setq oldsnap (getvar "osmode"))
  (setq oldpick (getvar "pickbox"))
  (setq temperr *error*)
  (setq *error* trap)
  (princ)
)					;defun
;;;*===========================================================
(defun trap (errmsg)
  (command nil nil nil)
  (if (not (member errmsg '("console break" "Function Cancelled"))
      ) ;_ end of not
    (princ (strcat "\nError: " errmsg))
  )					;                 
  (setvar "clayer" oldlayer)
  (setvar "blipmode" 1)
  (setvar "menuecho" 0)
  (setvar "highlight" 1)
  (setvar "osmode" oldsnap)
  (setvar "pickbox" oldpick)
  (princ "\nError Resetting Enviroment ")
  (terpri)
  (setq *error* temperr)
  (princ)
)					;defun
;;;*===========================================================
(defun reset ()
  (setq *error* temperr)
  (setvar "clayer" oldlayer)
  (setvar "blipmode" 1)
  (setvar "menuecho" 0)
  (setvar "highlight" 1)
  (setvar "osmode" oldsnap)
  (setvar "pickbox" oldpick)
  (princ)
)					;defun
;;;*======================================================
(defun upoint (bit kwd msg def bpt / inp)
  (if def				;check for a default
    (setq pts (strcat
		(rtos (car def))
		","
		(rtos (cadr def))	;formats X,Y 2D pt as string
		(if			;formats 3D ,Z if supplied and FLATLAND off
		  (and (caddr def) (= 0 (getvar "FLATLAND")))
		   (strcat "," (rtos (caddr def)))
		   ""
		) ;_ end of if
	      )				;if&strcat
	  msg (strcat "\n" msg "<" pts ">: ") ;string them with default
	  bit (* 2 (fix (/ bit 2)))	;a default and no null bit code conflict so
    )					;this reduces bit by 1 if odd, to allow null
    (if	(= " " (substr msg (strlen msg) 1))
					;no def, if last char is space
      (setq msg (strcat "\n" (substr msg 1 (1- (strlen msg))) ": "))
					;then strip space
      (setq msg (strcat "\n" msg ": "))	;else msg OK
    ) ;_ end of if
  )					;if,if
  (initget bit kwd)
  (setq	inp
	 (if bpt			;check for base point
	   (getpoint msg bpt)		;and use it
	   (getpoint msg)		;but not if nil
	 ) ;_ end of if
  )					;setq&if
  (if inp
    inp
    def
  )					;evaluate results and return proper value
)					;defun
(PRINC)


