	(defun maxp1real ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (listp 0 9.9e308))
		(logbool errloc rc)
		(setq *error* prverr)
	)

