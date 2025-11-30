;;;
;;; Lisp Acceptance Test
;;;	Geometric functions
;;;		inters
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
		(setq rc (inters))
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
		(setq rc (inters '(0 0) '(0 0) '(0 0) '(0 0)))
		(logall errloc rc)
		(setq *error* prverr)
	)

;;; ---------------------------------------------
;;; IntelliCAD help tests.
	(defun icadhlp1 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (inters '(2 3) '(2 -4) '(4 1) '(0 1)))
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun icadhlp2 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (inters '(2 3) '(2 -4) '(4 1) '(3 1)))
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun icadhlp3 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (inters '(2 3) '(2 -4) '(4 1) '(3 1) nil))
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun icadhlp4 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (inters '(2 3) '(2 -4) '(4 1) '(3 1) t))
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun icadhlp5 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (inters '(1 1) '(3 1) '(1 0) '(3 0)))
		(logall errloc rc)
		(setq *error* prverr)
	)

;;; ---------------------------------------------
;;; AutoCAD help tests.
	(defun acadhlp1 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (inters '(1.0 1.0) '(9.0 9.0) '(4.0 1.0) '(4.0 2.0)))
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun acadhlp2 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (inters '(1.0 1.0) '(9.0 9.0) '(4.0 1.0) '(4.0 2.0) t))
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun acadhlp3 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (inters '(1.0 1.0) '(9.0 9.0) '(4.0 1.0) '(4.0 2.0) nil))
		(logall errloc rc)
		(setq *error* prverr)
	)

