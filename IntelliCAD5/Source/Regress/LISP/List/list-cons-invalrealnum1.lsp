	(defun invalrealnum1 ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (cons '0 (list .5)))
		(log errloc rc)
		(setq *error* prverr)
	)

