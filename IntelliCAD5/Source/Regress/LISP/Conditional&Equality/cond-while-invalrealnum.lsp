	(defun invalrealnum ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (setq x .5))
		(while x
			(logstr errloc "hi from while") 
			(setq x nil)
		)
		(setq *error* prverr)
	)

