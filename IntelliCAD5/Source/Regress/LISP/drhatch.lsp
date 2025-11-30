(defun c:drf()
(setq sshatch (ssget "x" '((0 . "HATCH"))) n 0)
(repeat (sslength sshatch)
(setq ent (ssname sshatch n))
(command "Draworder" ent "" "b")
(setq n (1+ n))
);repeat
);defun
