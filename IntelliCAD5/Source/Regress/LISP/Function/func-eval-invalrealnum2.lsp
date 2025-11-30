	(defun invalrealnum2 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (eval (apply '+ '(.5 0))))
		(logall errloc rc)
		(setq *error* prverr)
	)

