	(defun maxp1zeroreal ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (cons '9.9e308 (list 0)))
		(log errloc rc)
		(setq *error* prverr)
	)

