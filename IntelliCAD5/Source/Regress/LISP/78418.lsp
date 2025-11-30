(defun LeaMi (leSX leSY leEX leEY MiCenX MiCenY)
  (command "leader"
	   (list leSX leSY)
	   (list leEX leEY)
	   ""
	   "OO"
	   ""
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

      "l"				;(princ (+ (distance (list leSX leSY) (list leEX leEY)) 0.09))


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


    (setq i (+ i 0.1))
  )
)
