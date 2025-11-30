	(defun maxp1real ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (if 9.9e308
					(logstr errloc "yes")
					(logstr errloc "no")
				))
		(setq *error* prverr)
	)

