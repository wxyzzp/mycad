(Defun c:Conference()

(command "zoom" "0.1x")

(setq ref (List 0 0) );setq
(setq len1 250)
(setq len1 200)
(setq cen1 (list 0 -1500) );setq
;(setq cen2 (list 0 -1500 250) );setq
;(setq rad (distance cen1 ref) );setq
;(command "Surftab1" "50")
;(command "Surftab2" "50")

(command "array" "All" "" "p" cen1 "25" "" "y")
(command "zoom" "e")      
(command "hide")

(terpri)(princ "This script was written primarily to check the performance of ICAD in generating surfaces")

);defun 
