(defun c:moo()
(setq ss1 (ssget "x"))
(command "move"ss1 "" "10,10" "0,0")
)
