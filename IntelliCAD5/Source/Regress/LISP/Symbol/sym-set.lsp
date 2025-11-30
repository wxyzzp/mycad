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
		(set 'rc 56)
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun icadhlp2 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(set 'rc "Hello")
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun icadhlp3 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(set 'rc '("a" "b" "c" "d"))
		(logall errloc rc)
		(setq *error* prverr)
	)

;;; ---------------------------------------------
;;; AutoCAD help tests.
	(defun acadhlp1 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(set 'rc 5.0)
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun acadhlp2 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq x 5.0)
		(set (quote rc) 'x)
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun acadhlp3 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(set 'x 5.0)
		(set 'rc 'x)
		(set rc 640)
		(logall errloc rc)
		(logall errloc x)
		(setq *error* prverr)
	)

