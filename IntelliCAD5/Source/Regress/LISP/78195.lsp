(defun drawucs()
	(command "color" "Red")
	(command "line" (list 0 0 0) (list 1 0 0) "")
	(command "color" "Green")
	(command "line" (list 0 0 0) (list 0 1 0) "")
	(command "color" "Blue")
	(command "line" (list 0 0 0) (list 0 0 1) "")
	(command "color" "Bylayer")
	(command "zoom" "extents")
)

(defun c:78195()
	(command "ucs" "entity" (entsel))
	(drawucs)

)

