;draws multiple sine curves using different powers
(defun c:SineCurves()
(setq i 1)
(while (<= i 10)
	(command "color" i)
	(sine i)
	(setq i (+ i 1))
);while ends
);defun ends


;deg to rad
(defun dtr(deg)
(setq deg (/ (* deg pi) 180.0))
)


;function to create sine curve
(defun sine(i)
;(setq n (getint "\nEnter the power of the sine graph :")
(setq 	n i
	th 10
	pt (list 0 0 )
)
(command "limits" (list -5 -5) (list 5 5)
	"zoom" "a"
)
(while (<= th 360)
(setq ptlast pt
	rth (dtr th)
	y (expt (sin rth) n)
	th (+ th 10)
	pt (list rth y)
)
(command "line" ptlast pt "")
)
)

