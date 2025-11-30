;_Created_by_Srinivas_Chavan_for_Cybage_on_3/17/2003
(defun c:inters_check ()
  (setq MAX_SIZE 1.0)
  (setq x1 (- MAX_SIZE))
  (setq INCRE_SIZE 1.0)
  (setq INFINITE 99999999)
  (setq	x2 x1
	x3 x1
	x4 x1
	y1 x1
	y2 x1
	y3 x1
	y4 x1
  )

  (while (< x1 MAX_SIZE)
    (progn
      (while (< y1 MAX_SIZE)
	(progn
	  (while (< x2 MAX_SIZE)
	    (progn
	      (while (< y2 MAX_SIZE)
		(progn
		  (while (< x3 MAX_SIZE)
		    (progn
		      (while (< y3 MAX_SIZE)
			(progn
			  (while (< x4 MAX_SIZE)
			    (progn
			      (while (< y4 MAX_SIZE)
				(progn
				  (setq	xdiff1 (- x2 x1)
					xdiff2 (- x4 x3)
				  )
				  (if (= xdiff1 0.0)
				    (setq m1 INFINITE)
				    (setq m1 (/ (- y2 y1) xdiff1))
				  )
				  (if (= xdiff2 0.0)
				    (setq m2 INFINITE)
				    (setq m2 (/ (- y4 y3) xdiff2))
				  )
				  (if (= m1 m2)
				    (progn
				      (setq inters_res
					     (inters
					       (list x1 y1)
					       (list x2 y2)
					       (list x3 y3)
					       (list x4 y4)
					     )
				      )
				      (if (/= inters_res nil)
					(progn
					  (princ "\n\n x1:")
					  (princ x1)
					  (princ "\n y1:")
					  (princ y1)
					  (princ "\n x2:")
					  (princ x2)
					  (princ "\n y2:")
					  (princ y2)
					  (princ "\n x3:")
					  (princ x3)
					  (princ "\n y3:")
					  (princ y3)
					  (princ "\n x4:")
					  (princ x4)
					  (princ "\n y4:")
					  (princ y4)
					  (princ "\n m1:")
					  (princ m1)
					  (princ "\n m2:")
					  (princ m2)
					  (princ "\n inters result:"
					  )
					  (princ inters_res)
					  (alert "Check this fix for failure")z
					)
				      )

				    )
				  )
				)
				(setq y4     (+ y4 INCRE_SIZE)
				      xdiff1 0.0
				      xdiff2 0.0
				      m1     0.0
				      m2     0.0
				)
			      )
			    )
			    (setq x4 (+ x4 INCRE_SIZE)
				  y4 (- MAX_SIZE)
			    )
			  )
			)
			(setq y3 (+ y3 INCRE_SIZE)
			      x4 (- MAX_SIZE)
			)
		      )
		    )
		    (setq x3 (+ x3 INCRE_SIZE)
			  y3 (- MAX_SIZE)
		    )
		  )
		)
		(setq y2 (+ y2 INCRE_SIZE)
		      x3 (- MAX_SIZE)
		)
	      )
	    )
	    (setq x2 (+ x2 INCRE_SIZE)
		  y2 (- MAX_SIZE)
	    )
	  )
	)
	(setq y1 (+ y1 INCRE_SIZE)
	      x2 (- MAX_SIZE)
	)
      )
    )
    (setq x1 (+ x1 INCRE_SIZE)
	  y1 (- MAX_SIZE)
    )
  )
)

