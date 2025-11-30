	(defun invalrealnum ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (nth .5 '("a" "b" "c" "d" "e" "f" "g" "h" "i" "j" "k")))
		(logbool errloc rc)
		(setq *error* prverr)
	)

