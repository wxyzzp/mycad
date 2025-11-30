(defun c:ccol()
(command "viewres" "y" "20000")
(setq ss1 (ssget "X" '((0 . "CIRCLE")))n 0)
(repeat (fix (sslength ss1))
(setq ent (entget (ssname ss1 n)))
(setq rad (cdr (assoc 40 ent)))
(setq cpt (cdr (assoc 10 ent)))
(command "zoom" "c" cpt "4")
(if (< rad 0.7)
(progn
(setq ent (subst (cons 62 1) (assoc 62 ent) ent))
(entmod ent)
)
(progn
(setq ent (subst (cons 62 3) (assoc 62 ent) ent))
(entmod ent)
)
);if
(setq n (1+ n))
);repeat
);defun
