	(defun maxp1real ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (setq x 9.9e308))
		(while x
			(logstr errloc "hi from while") 
			(setq x nil)
		)
		(setq *error* prverr)
	)

