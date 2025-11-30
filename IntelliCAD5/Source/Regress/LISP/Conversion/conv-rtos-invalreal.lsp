	(defun invalidrealnum ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (rtos .5))
		(logall errloc rc)
		(setq *error* prverr)
	)

