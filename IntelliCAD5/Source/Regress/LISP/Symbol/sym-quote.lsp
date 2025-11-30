;;;
;;; Lisp Acceptance Test
;;;	Symbol functions
;;;		quote
;;;
;;; 17 Jun 98	MichaelHu
;;;
;;; quote is mostly tested in the list tests.
;;;
	(logmsg "")
	(load (strcat testscrdir "error"))

;;; ---------------------------------------------
;;; IntelliCAD help tests.
	(defun icadhlp1 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc '(1 2 3 4))
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun icadhlp2 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (quote (1 2 3 4)))
		(logall errloc rc)
		(setq *error* prverr)
	)

;;; ---------------------------------------------
;;; AutoCAD help tests.
	(defun acadhlp1 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc '"expr")
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun acadhlp2 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (quote "a"))
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun acadhlp3 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (quote "cat"))
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun acadhlp4 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (quote ("a" "b")))
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun acadhlp5 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc '"a")
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun acadhlp6 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc '"cat")
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun acadhlp7 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc '("a" "b"))
		(logall errloc rc)
		(setq *error* prverr)
	)

