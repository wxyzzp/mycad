(Defun c:Theatre()

(setq dist 550) 
(setq lev 1.0)
(setq base (list 0 0 0) );setq
(setq disp (List 0 dist (* dist lev) ) );setq
(command "zoom" "0.1x")
(command "3Darray" "All" "" "R" "" "10" "" "450")

(repeat 3
(command "copy" "all" "" base disp)
(setq lev (+ lev 1) );setq
(setq disp (List 0 (* dist lev) (* dist lev)) );setq
);repeat

(command "zoom" "e")      
(command "hide")

(terpri)(princ "This script was written primarily to check the performance of ICAD in generating surfaces")

);defun
