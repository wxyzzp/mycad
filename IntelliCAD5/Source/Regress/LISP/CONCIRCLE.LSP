(defun c:concircles()
(setq pt (getpoint "\nEnter Center point :")
      rad (getdist pt "\nRadius for the first concentric circle :")
      nom (getint "\nEnter number of circles :")
)
(repeat (abs nom)
(if (minusp nom)
(progn
(setq ab (/ rad (abs nom)))
(command "circle" pt rad)
(setq rad (- rad ab))
);progn
(progn
(setq ab (/ rad nom))
(command "circle" pt rad)
(setq rad (+ rad ab))
);progn
);if
);repeat
);defun
