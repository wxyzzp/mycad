	(defun invalrealnum ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc .5 rc (boundp 'rc))
		(logall errloc rc)
		(setq *error* prverr)
	)

