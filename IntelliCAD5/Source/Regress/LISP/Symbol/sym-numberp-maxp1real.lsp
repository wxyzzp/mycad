	(defun maxp1real ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (numberp 9.9e308))
		(logall errloc rc)
		(setq *error* prverr)
	)

