;;;
;;; Lisp Acceptance Test
;;;	Geometric functions
;;;		distance
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
		(setq rc (distance))
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
		(setq rc (distance '(0 0) '(0 0)))
		(logall errloc rc)
		(setq *error* prverr)
	)

;;; ---------------------------------------------
;;; IntelliCAD help tests.
	(defun icadhlp1 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (distance '(2 3) '(5 1)))
		(logall errloc rc)
		(setq *error* prverr)
	)

;;; ---------------------------------------------
;;; AutoCAD help tests.
	(defun acadhlp1 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (distance '(1.0 2.5 3.0) '(7.7 2.5 3.0)))
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun acadhlp2 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (distance '(1.0 2.0 0.5) '(3.0 4.0 0.5)))
		(logall errloc rc)
		(setq *error* prverr)
	)

