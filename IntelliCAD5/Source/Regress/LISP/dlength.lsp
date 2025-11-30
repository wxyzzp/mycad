;;Tip1767:  DLENGTH.LSP       DEVELOPED LENGTH       (C)2002, John May

;;;---------------------------------------------------------------
;;;DESCRIPTION:
;;;    Developed length:
;;;    Give the thickness, Inside Radius, how many Degrees in the bend, and the answer placement
;;;    code will put the formula in text with the answer.
;;;
;;;---------------------------------------------------------------
;;;
;;; Created by:  	John May
;;;         	ColumbusJack Corp.
;;;                     1000 Front street
;;;		Jmay@columbusjack.com
;;;
;;;--------------------------------------------------------------

(defun
   C:DLENGTH (/)
  (setq THK (getreal "\nWhat is the thickness?  "))
  (setq STHK (rtos THK))
  (setq RDS (getreal "\nWhat is the Inside Radius?  "))
  (setq SRDS (rtos RDS))
  (setq DAG (getreal "\nHow many Degrees? "))
  (setq SDAG (itoa (fix DAG)))
  (setq CPT (getpoint "\nPick answer placement. "))
  (setq OUTC1 (* 0.64 THK))
  (setq OUTC2 (* 1.57 RDS))
  (setq OUTC3 (/ DAG 90))
  (setq OUTC4 (+ OUTC1 OUTC2))
  (setq OUTC5 (* OUTC4 OUTC3))
  (setq ANS (rtos OUTC5))
  (setq TXTH (* (getvar "dimtxt") (getvar "dimscale")))
  (setq
    BLNTXT
     (strcat
       " " ANS " = [(0.64 x " STHK ") x (1.57 x " SRDS ")] x " SDAG
       "/90  "
      ) ;_ end of strcat
  ) ;_ end of setq
  (command "text" "m" CPT TXTH 0 BLNTXT)
  (princ)
  (gc)
) ;_ end of defun

