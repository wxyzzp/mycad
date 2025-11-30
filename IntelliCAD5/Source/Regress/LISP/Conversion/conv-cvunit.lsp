;;;
;;; Lisp Acceptance Test
;;;	Conversion functions
;;;		cvunit
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
		(setq rc (cvunit))
		(logall errloc rc)
		(setq *error* prverr)
	)

;;; ---------------------------------------------
;;; IntelliCAD help tests.
	(defun icadhlp1 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (cvunit 1 "day" "second"))
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun icadhlp2 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (cvunit 2 "meter" "yard"))
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun icadhlp3 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (cvunit '(2 3) "mile" "feet"))
		(logall errloc rc)
		(setq *error* prverr)
	)

;;; ---------------------------------------------
;;; AutoCAD help tests.
	(defun acadhlp1 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (cvunit 1 "minute" "second"))
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun acadhlp2 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (cvunit 1 "gallon" "furlong"))
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun acadhlp3 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (cvunit 1.0 "inch" "cm"))
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun acadhlp4 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (cvunit 1.0 "acre" "sq yard"))
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun acadhlp5 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (cvunit '(1.0 2.5) "ft" "in"))
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun acadhlp6 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (cvunit '(1 2 3) "ft" "in"))
		(logall errloc rc)
		(setq *error* prverr)
	)

