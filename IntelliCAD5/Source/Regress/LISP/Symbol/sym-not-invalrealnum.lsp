	(defun invalrealnum ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc .5 rc (not rc))
		(logall errloc rc)
		(setq *error* prverr)
	)

