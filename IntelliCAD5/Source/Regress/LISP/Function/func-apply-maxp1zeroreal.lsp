	(defun maxp1zeroreal ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (apply '+ '(9.9e308 0)))
		(logall errloc rc)
		(setq *error* prverr)
	)

