(defun c:testReva ( / )
	(command "pline" (list 7 10) (list 3 10) (list 3 7) (list 5 7) (list 6 4) "")
	(setq pl (entlast))
	(command "line" (list 8 9) (list 8 3) "")
	(setq ln (entlast))
	; this next line should work, but it doesn't:
	(command "revsurf" pl ln 0 360)
)
(defun c:testRevb ( / )
	(command "pline" (list 7 10) (list 3 10) (list 3 7) (list 5 7) (list 6 4) "")
	(setq pl (entlast))
	(command "line" (list 8 9) (list 8 3) "")
	(setq ln (entlast))
	; you have to do this, instead:
	(command "revsurf" (list 7 10) (list 8 9) 0 360)
)

(defun c:testRulea ( / )
	(command "pline" (list 7 10) (list 3 10) (list 3 7) (list 5 7) (list 6 4) "")
	(setq pl (entlast))
	(command "line" (list 8 9) (list 8 3) "")
	(setq ln (entlast))
	; this next line should work, but it doesn't:
	(command "rulesurf" pl ln)
)
(defun c:testRuleb ( / )
	(command "pline" (list 7 10) (list 3 10) (list 3 7) (list 5 7) (list 6 4) "")
	(setq pl (entlast))
	(command "line" (list 8 9) (list 8 3) "")
	(setq ln (entlast))
	; you have to do this, instead:
	(command "rulesurf" (list 7 10) (list 8 9))
)

