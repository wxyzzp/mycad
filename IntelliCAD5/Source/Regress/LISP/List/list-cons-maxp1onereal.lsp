	(defun maxp1onereal ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (cons '9.9e308 (list 1)))
		(log errloc rc)
		(setq *error* prverr)
	)

