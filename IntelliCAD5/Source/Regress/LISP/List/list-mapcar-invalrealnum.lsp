	(defun invalrealnum ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (mapcar '1+ '( 0 .5)))
		(logbool errloc rc)
		(setq *error* prverr)
	)

