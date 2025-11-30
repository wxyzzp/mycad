	(defun invalrealnum ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (if .5
					(logstr errloc "yes")
					(logstr errloc "no")
				))
		(setq *error* prverr)
	)

