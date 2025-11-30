	(defun invalrealnum ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (cond 
					(.5 (logmsg errloc "t")) 
					(t (logmsg errloc "default"))
				))
		(setq *error* prverr)
	)

