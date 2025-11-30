	(defun onemaxp1real ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (cons '1 (list 9.9e308)))
		(log errloc rc)
		(setq *error* prverr)
	)

