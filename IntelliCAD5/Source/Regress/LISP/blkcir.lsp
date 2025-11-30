(defun c:bcirl()
(command "viewres" "y" "20000")
(setq ss1 (ssget "X" '((0 . "CIRCLE")))n 0)
(repeat (fix (sslength ss1))
(setq ent (entget (ssname ss1 n)))
(setq rad (cdr (assoc 40 ent)))
(setq cpt (cdr (assoc 10 ent)))
(if (< rad 0.7)
(progn
(command "zoom" "c" cpt "2")
(setq ent (subst (cons 62 2) (assoc 62 ent) ent))
(entmod ent)
)
(progn
(command "zoom" "c" cpt "3")
(setq ent (subst (cons 62 4) (assoc 62 ent) ent))
(entmod ent)
)
);if
(setq n (1+ n))
);repeat
);defun
