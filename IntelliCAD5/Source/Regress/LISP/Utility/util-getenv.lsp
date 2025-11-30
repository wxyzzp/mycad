;;;
;;; Lisp Acceptance Test
;;;	getenv functions
;;;		getenv
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
		(setq rc (getenv))
		(logall errloc rc)
		(setq *error* prverr)
	)

;;; ---------------------------------------------
;;; IntelliCAD help tests.
	(defun icadhlp1 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (getenv "PATH"))
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun icadhlp2 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (getenv "junk"))
		(logall errloc rc)
		(setq *error* prverr)
	)

;;; ---------------------------------------------
;;; AutoCAD help tests.
	(defun acadhlp1 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (getenv "ACAD"))
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun acadhlp2 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (getenv "NOSUCH"))
		(logall errloc rc)
		(setq *error* prverr)
	)

