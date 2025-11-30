	(defun maxp1real ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (cond 
					(9.9e308 (logmsg errloc "t")) 
					(t (logmsg errloc "default"))
				))
		(setq *error* prverr)
	)

