	(defun invalrealnum ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (setq cnt 0 x .5))
		(repeat x
			(setq cnt (1+ cnt))
			(logflt errloc cnt) 
		)
		(setq *error* prverr)
	)

