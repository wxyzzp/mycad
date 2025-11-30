	(defun invalidrealnum ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (atof ".5"))
		(logall errloc rc)
		(setq *error* prverr)
	)

