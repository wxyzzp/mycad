	(defun invalrealnum2 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (cons '.5 (list 0)))
		(log errloc rc)
		(setq *error* prverr)
	)

