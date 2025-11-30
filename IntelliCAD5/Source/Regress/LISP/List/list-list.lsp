;;;
;;; Lisp Acceptance Test
;;;	List functions
;;;		list
;;;
;;; 17 Jun 98	MichaelHu
;;;
	(logmsg "")
	(load (strcat testscrdir "error"))


;;; ---------------------------------------------
;;; list is mostly tested in cons.


;;; ---------------------------------------------
;;; IntelliCAD help tests.
	(defun icadhlp1 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (list 1 2))
		(loglist errloc rc)
		(setq *error* prverr)
	)
	(defun icadhlp2 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (list "string1" "string2"))
		(loglist errloc rc)
		(setq *error* prverr)
	)
	(defun icadhlp3 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (list (car (list 1 2)) (car (list "string1" "string2")) ))
		(loglist errloc rc)
		(setq *error* prverr)
	)
	(defun icadhlp4 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc '(1 2 "buckle my shoe"))
		(loglist errloc rc)
		(setq *error* prverr)
	)

;;; ---------------------------------------------
;;; AutoCAD help tests.
	(defun acadhlp1 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (list '"a" '"b" '"c"))
		(loglist errloc rc)
		(setq *error* prverr)
	)
	(defun acadhlp2 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (list '"a" '("b" "c") '"d"))
		(loglist errloc rc)
		(setq *error* prverr)
	)
	(defun acadhlp3 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (list 3.9 6.7))
		(loglist errloc rc)
		(setq *error* prverr)
	)

