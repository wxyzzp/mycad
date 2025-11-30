(defun c:77892_ovl()
  (setq	dimvars	'((140 dimtxt)
		  (147 dimgap)
		  )
  )

  (setq	entdata
	 (cdr
	   (assoc "ACAD"
		  (cdr
		    (assoc -3
			   (entget (entlast) '("ACAD"))
		    )
		  )
	   )
	 )
  )
  (setq ovlist nil)
  (setq var1 nil)
  (foreach listid entdata
    (if	(or (= (car listid) 1070)
	    (= (car listid) 1040)
	    (= (car listid) 1000)
	)
      (if (= var1 nil)
	(progn
	  (setq var1 (cdr listid))
					;(princ var1)
	  (setq var1 (cdr (assoc var1 dimvars)))
	)
	(progn
	  (setq ovlist (cons (cons var1 (cdr listid)) ovlist))
					;(princ var1)
	  (setq var1 nil)
	)
      )
    )
  )
  ;(princ ovlist)
(setq gp (cdr (assoc '(dimgap) ovlist)))
(setq tx (cdr (assoc '(dimtxt) ovlist)))
)
