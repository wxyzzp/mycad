	(defun maxp1onereal ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (eval (apply '+ '(9.9e308 1))))
		(logall errloc rc)
		(setq *error* prverr)
	)

