;_This_LISP_is_created_by_Srinivas_on_4/14/2003_testing_functionality_of_cond_function
;_Pass_the_parameter_to_the_function_as_"acad.exe"_to_run_LISP_in_ACAD_and_"icad.exe"_to_run_in_ICAD
(defun checkCond (exeName)
  (setq fpath (findfile exeName))
 ;_______
  (if (/= (cond ((findfile exeName))) fpath)
    (alert "Err")
  )
 ;_______
  (if (/= (cond (T (findfile exeName))) fpath)
    (alert "Err")
  )
 ;_______
  (cond (0 (princ "Hello") (findfile exeName)))
 ;_______
  (if (/= (cond	(0 (princ "Hello"))
		(T (findfile exeName))
	  )
	  "Hello"
      )
    (alert "Err")
  )
 ;_______
  (if (/= (cond	(T (princ "Hello"))
		(findfile exeName)
	  )
	  "Hello"
      )
    (alert "Err")
  )
 ;_______
  (cond	(T (princ "Hello"))
	(T (findfile exeName))
  )
  (defun FILENAME () T)
  (cond ((FILENAME) (findfile exeName)))
;_______
(if (/= (cond (0 '("Hello") (findfile exeName))) fpath) (alert "Err"))
;_______
(if (/= (cond (1 (findfile exeName))) fpath) (alert "Err"))
)
