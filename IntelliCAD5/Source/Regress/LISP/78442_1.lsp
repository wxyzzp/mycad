 ;_Created_by_Srinivas_Chavan_on_13_Mar_2003_
 ;_Some_Suggestions_by_Uday
(defun checkInters (ptInters ptExp)
					;  (princ ptExp)
  (if (= ptExp nil)

    (progn
					;     (princ "if")
					;    (princ ptInters)
      (if (/= ptInters nil)
	(alert "Please check this bug fix")
	(princ "OK")
      )
    )
    (progn
					;   (princ "else")
      (if
	(not
	  (and
	    (equal (car ptInters) (car ptExp) 0.0000001)
	    (equal (cadr ptInters) (cadr ptExp) 0.0000001)
	    (equal (caddr ptInters) (caddr ptExp) 0.0000001)
	  )
	)
	 (alert "Please check this bug fix")
	 (princ "OK")
      )
    )
  )
)
					;(setq ptInters )
					;(setq ptExp )
					;(princ ptInters)
					;(princ ptExp)
;__1
(checkInters
  (inters
    '(2.0 3.0)
    '(2.0 -4.0)
    '(4.0 1.0)
    '(0.0 1.0)
  )
  (list 2.0 1.0)
)
;__2
(checkInters
  (inters
    '(2.0 3.0)
    '(2.0 -4.0)
    '(4.0 1.0)
    '(3.0 1.0)
  )
  nil
)
;__3
(checkInters
  (inters
    '(2.0 3.0) '(2.0 -4.0) '(4.0 1.0) '(3.0 1.0) nil)
  '(2.0 1.0)
)
;__4
(checkInters
  (inters
    '(2.0 3.0) '(2.0 -4.0) '(4.0 1.0) '(3.0 1.0) T)
  nil
)
;__5
(checkInters
  (inters
    '(1.0 1.0) '(3.0 1.0) '(1.0 0.0) '(3.0 0.0)	nil)
  nil
)
;__6
(checkInters
  (inters
    '(2.0 3.0)
    '(2.0 -4.0)
    '(4.0 1.0)
    '(0.0 1.0)
  )
  (list 2.0 1.0)
)
;__7
(checkInters
  (inters
    '(-2.0 -2.0)
    '(-2.0 1.0)
    '(-1.0 -2.0)
    '(-1.0 -2.0)
  )
  nil
)
;__8
(checkInters
  (inters '(1.0 1.0 1.0)
	  '(3.0 3.0 1.0)
	  '(3.0 1.0 1.0)
	  '(1.0 3.0 1.0)
  )
  '(2.0 2.0 1.0)
)
;__9
(checkInters
  (inters '(0.0 0.0 0.0)
	  '(2.0 2.0 2.0)
	  '(0.0 2.0 0.0)
	  '(2.0 0.0 2.0)
  )
  '(1.0 1.0 1.0)
)
;__10
(checkInters
  (inters
    '(0.0 0.0 0.0)
    '(2.0 2.0 2.0)
    '(0.0 0.0 2.0)
    '(2.0 2.0 4.0)
  )
  nil
)
;__11
					;Parallel vertical lines
(checkInters
  (inters
    '(-1.0 -1.0)
    '(-1.0 0.0)
    '(-2.0 0.0)
    '(-2.0 1.0)
  )
  nil
)
;__12
					;Parallel horizontal lines
(checkInters
  (inters
    '(-1.0 -1.0)
    '(0.0 -1.0)
    '(0.0 -2.0)
    '(1.0 -2.0)
  )
  nil
)
;__13
					;Parallel z-dirn lines
(checkInters
  (inters
    '(0.0 0.0 0.0)
    '(0.0 0.0 0.0)
    '(0.0 2.0 1.0)
    '(0.0 2.0 1.0)
  )
  nil
)
