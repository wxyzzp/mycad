;_This_LISP_is_created_by_Srinivas_Chavan
(defun c:checkleader ()
  (setq le (entget (entlast)))
  (setq coordoffirst (cadddr (assoc 10 le)))
  (foreach co le
    (progn (if (= (car co) 10)
	     (if (/= coordoffirst (cadddr co))
	       (alert "Err")
	     )
	   )
    )
  )
)