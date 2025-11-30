	(defun invalrealnum1 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (apply '+ '(0 .5)))
		(logall errloc rc)
		(setq *error* prverr)
	)

