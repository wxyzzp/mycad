	(defun onemaxp1real ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (eval (apply '+ '(1 9.9e308))))
		(logall errloc rc)
		(setq *error* prverr)
	)

