	(defun maxp1real ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (nth 9.9e308 '("a" "b" "c" "d" "e" "f" "g" "h" "i" "j" "k")))
		(logbool errloc rc)
		(setq *error* prverr)
	)

