(defun c:78310()
	(if (= (assoc '42. (entget (entlast))) nil)
			;(princ "---------****Error****---------")
			(getpoint "------*****Error*****-------")
			;above line stops script in case of errror
	)
)
