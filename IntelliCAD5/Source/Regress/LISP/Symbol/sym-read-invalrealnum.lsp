	(defun invalrealnum ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (read ".5"))
		(logall errloc rc)
		(setq *error* prverr)
	)

