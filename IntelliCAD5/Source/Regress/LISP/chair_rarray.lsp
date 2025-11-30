(Defun c:chair_rarray()
(command "zoom" "0.125x")
(command "3Darray" "All" "" "R" "2" "10" "10" "450" "450")
(setq cen (getvar "viewctr") );setq
(command "zoom" "e")      
(command "hide")
(command "zoom" "c" cen "5x")
(command "hide")
(command "zoom" "e")
(terpri)(princ "This script was written primarily to check the performance of ICAD in generating surfaces")
);defun
