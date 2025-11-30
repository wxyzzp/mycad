;;;
;;; Lisp Acceptance Test
;;;	Geometric functions
;;;		angle
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
		(setq rc (angle))
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
		(setq rc (angle '(0 0) '(0 0)))
		(logall errloc rc)
		(setq *error* prverr)
	)

;;; ---------------------------------------------
;;; IntelliCAD help tests.
	(defun icadhlp1 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (angle '(1 1) '(3 3)))
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun icadhlp2 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (angle '(2.0 3.3 1.3) '(5.2 8.9 6.0)))
		(logall errloc rc)
		(setq *error* prverr)
	)

;;; ---------------------------------------------
;;; AutoCAD help tests.
	(defun acadhlp1 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (angle '(1.0 1.0) '(1.0 4.0)))
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun acadhlp2 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (angle '(5.0 1.33) '(2.4 1.33)))
		(logall errloc rc)
		(setq *error* prverr)
	)

