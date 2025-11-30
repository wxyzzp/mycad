;;;
;;; Lisp Acceptance Test
;;;	Symbol functions
;;;		lambda
;;;
;;; 17 Jun 98	MichaelHu
;;;
	(logmsg "")
	(load (strcat testscrdir "error"))

;;; ---------------------------------------------
;;; IntelliCAD help tests.
	(defun icadhlp1 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc 
			(apply
				'(lambda (a b c) 
					(+ a (* b c))
				)
				'(5 2 3)
			)
		)
		(logall errloc rc)
		(setq *error* prverr)
	)

;;; ---------------------------------------------
;;; AutoCAD help tests.
	(defun acadhlp1 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc 
			(apply
				'(lambda (x y z) 
					(* x (- y z))
				)
				'(5 20 14)
			)
		)
		(logall errloc rc)
		(setq *error* prverr)
	)
	(defun acadhlp2 ( / prverr cntr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc 
			(mapcar
				'(lambda (x) 
					(* x 5)
				)
				'(2 4 -6 10.2)
			)
		)
		(logall errloc rc)
		(setq *error* prverr)
	)

