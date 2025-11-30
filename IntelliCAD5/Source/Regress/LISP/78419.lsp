;_This_LISP_is_created_by_Srinivas(Cybage_Software)_on_3/31/2003
(defun LeaMi (leSX leSY leEX leEY MiCenX MiCenY)
					;(command "zoom" "w" "-3,-3" "3,3")
  (command "leader"
	   (list leSX leSY)
	   (list leEX leEY)
	   ""
	   "OO"
	   ""
  )
  (command "move"
	   (polar (list leSX leSY)
		  (atan (/ (- leEY leSY) (- leEX leSX)))
		  (+ (distance (list leSX leSY) (list leEX leEY)) 0.09)
	   )
	   (list leSX leSY)
	   ""
	   "0,0"
	   "0,0"
  )
  (setq i 0)
  (setq co 1)
  (while (< i 6.2832)
    (command
      "'zoom"
      "c"
      (list leSX leSY)
      ".5"
      "mirror"

					;(princ (+ (distance (list leSX leSY) (list leEX leEY)) 0.09))

      (polar (list leSX leSY)
	     (atan (/ (- leEY leSY) (- leEX leSX)))
	     (+ (distance (list leSX leSY) (list leEX leEY)) 0.27)
      )
      (list leSX leSY)

      ""
      (list MiCenX MiCenY)
      (polar (list MiCenX MiCenY) i 1.0)
      "n"
    )
    (command "zoom" "extents")
    (command "chprop" "p" "" "color" co "")
    (if	(< co 254)
      (setq co (1+ co))
      (setq co 1)
    )

    (setq le (entget (entlast)))
    (setq
      sl (assoc 211 le)
    )


    (setq x1 (cadr sl))
    (setq y1 (caddr sl))
    (setq j (/ (atan y1 x1) 2.0))
    (if	(< j 0.0)
      (setq j (+ 6.2832 j))
    )

    (setq ii i)
    (if	(> i 3.1416)
      (setq ii (- i 3.1416))
    )

    (if	(> (- ii j) 0.0001)
      (progn
	(princ "\n Angle : \n")
	(princ i)
	(princ "\n X : ")
	(princ x1)
	(princ " Y : ")
	(princ y1)
	(princ " \n ")
	(alert "Error")
      )
      (progn
	(princ "\n Angle : \n")
	(princ i)
	(princ "\n X : ")
	(princ x1)
	(princ " Y : ")
	(princ y1)
	(princ " \n ")

      )
    )

    (setq i (+ i 0.05))
  )
)

(LeaMi 2 2 4 2 1 1)
(command "erase" "all" "")
(LeaMi 2 2 4 2 3.5 3.5)
(command "erase" "all" "")
(LeaMi 1 1 3 1 -0.5 -0.5)
(command "erase" "all" "")
(LeaMi -2 -2 4 -2 3.5 3.5)
					;(command "erase" "all" "")
					;(LeaMi -2 -3 4 -1 3.5 3.5)
