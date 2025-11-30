	(defun invalrealnum2 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (apply '+ '(.5 0)))
		(logall errloc rc)
		(setq *error* prverr)
	)

