	(defun invalrealnum ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (atom .5))
		(logall errloc rc)
		(setq *error* prverr)
	)

