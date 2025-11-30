(prompt "\nType testbeam to run.....")
(defun c:testbeam ()
;______________________
	(setq oldsnap (getvar "osmode"))
	(setq oldblipmode (getvar "blipmode"))
	(setvar "osmode" 0)(setvar "blipmode" 0)
;________________________
	(initget (+ 1 2 4))(setq lb (getdist "\nLength of Beam : "))
	(initget (+ 1 2 4))(setq hb (getdist "\nHeight of Beam : "))
	(initget (+ 1 2 4))(setq wt (getdist "\nFlange Thickness : "))
	(initget (+ 1 2 4))(setq ep (getdist "\nEnd Plate Thickness : "))
	(initget (+ 1 2 4))(setq nl (getdist "\nLength of Notch : "))
	(initget (+ 1 2 4))(setq nd (getdist "\nDepth of Notch : "))
;____________________________
;Get Insertion Point
	(setvar "osmode" 32)
(while(setq ip (getpoint "\nInsertion Point : "))
	(setvar "osmode" 0)
;____________________________
;Start of Polar Calculations
	(setq p2 (polar ip (dtr 180.0) (- (/ lb 2) nl)))
	(setq p3 (polar p2 (dtr 270.0) wt))
	(setq p4 (polar p2 (dtr 270.0) nd))
	(setq p5 (polar p4 (dtr 180.0) nl))
	(setq p6 (polar p5 (dtr 180.0) ep))
	(setq p7 (polar p6 (dtr 270.0) (- hb nd)))
	(setq p8 (polar p7 (dtr 0.0) ep))
	(setq p9 (polar p8 (dtr 90.0) wt))
	(setq p10 (polar p9 (dtr 0.0) lb))
	(setq p11 (polar p8 (dtr 0.0) lb))
	(setq p12 (polar p11 (dtr 0.0) ep))
	(setq p13 (polar p12 (dtr 90.0) (- hb nd)))
	(setq p14 (polar p13 (dtr 180.0) ep))
	(setq p15 (polar p14 (dtr 180.0) nl))
	(setq p16 (polar p15 (dtr 90.0) (- nd wt)))
	(setq p17 (polar p16 (dtr 90.0) wt))
;End of Polar Calculations
;___________________________________
;Start of Command Function
(command "Line" ip p2 p4 p6 p7 p12 p13 p15 p17 "c""Line" p3 p16 ""
"Line" p9 p10 """Line" p5 p8 """Line" p11 p14 "") 
;End of Command Function
;___________________________________
	(setvar "osmode" 32)

);end of while loop
;___________________________________
;Reset System Variable
(setvar "osmode" oldsnap)(setvar "blipmode" oldblipmode)
;Reset blipmode
;__________________________________
	(princ)
) ;end of defun
;____________________________________
;This function converts Degrees to Radians.
(defun dtr (x)(* pi (/ x 180.0));divide the angle by 180 then;multiply the result by the constant PI
) ;end of function
;____________________________________
(princ) ;load cleanly
;_______________________________________