	(defun invalidrealnum ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (distof ".5"))
		(logall errloc rc)
		(setq *error* prverr)
	)

