(defun silent_err ( errmsg / )
	(logmsg (strcat "***Error (" errloc "): " errmsg))
	(princ)
)

