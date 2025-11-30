;draws multiple cosine curves using different powers
(defun c:CosineCurves()
(setq i 1)
(while (<= i 10)
	(command "color" i)
	(cosine i)
	(setq i (+ i 1))
);while ends
);defun ends


;deg to rad
(defun dtr(deg)
(setq deg (/ (* deg pi) 180.0))
)


;function to create cosine curve
(defun cosine(i)
;(setq n (getint "\nEnter the power of the cosine graph :")
(setq 	n i
	th 10
	pt (list 0 3 )
)
(command "limits" (list -5 -5) (list 5 5)
	"zoom" "a"
)
(while (<= th 360)
(setq ptlast pt
	rth (dtr th)
	y (expt (cos rth) n)
	y (+ y 2)
	th (+ th 10)
	pt (list rth y)
)
(command "line" ptlast pt "")
)
)

