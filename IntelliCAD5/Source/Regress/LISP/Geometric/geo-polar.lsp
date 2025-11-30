;;;
;;; Lisp Acceptance Test
;;;	Geometric functions
;;;		polar
;;;
;;; 17 Jun 98	MichaelHu
;;;
	(logmsg "")
	(load (strcat testscrdir "error"))
;;; ---------------------------------------------
;;; boundary tests.
	;;; ------------------------------------------
	;;; no arguments.
	(defun noargs ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (polar))
		(logall errloc rc)
		(setq *error* prverr)
	)
	;;; --------------------------------------------
	;;; numerics.
	;;; --------------------------------------------
	;;; zero-length line.
	(defun zero ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (polar '(0 0) 0 0))
		(logall errloc rc)
		(setq *error* prverr)
	)

;;; ---------------------------------------------
;;; IntelliCAD help tests.
	(defun icadhlp1 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (polar '(1 2) 1.570796 2))
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun icadhlp2 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (polar '(3.13397 1) 0.523598775 1))
		(logall errloc rc)
		(setq *error* prverr)
	)

;;; ---------------------------------------------
;;; AutoCAD help tests.
	(defun acadhlp1 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (polar '(1 1 3.5) 0.785398 1.414214))
		(logall errloc rc)
		(setq *error* prverr)
	)

