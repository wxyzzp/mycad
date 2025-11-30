	(defun maxp1real ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc 9.9e308 rc (boundp 'rc))
		(logall errloc rc)
		(setq *error* prverr)
	)

