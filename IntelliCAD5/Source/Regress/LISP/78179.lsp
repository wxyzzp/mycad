 (defun c:78179()
	(setq e (entsel))
	(command "dimtedit" e "angle" "45"
		"dimtedit" e "restore"
	)
)
