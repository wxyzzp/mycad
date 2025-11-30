	(defun zeromaxp1real ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (eval (apply '+ '(0 9.9e308))))
		(logall errloc rc)
		(setq *error* prverr)
	)

