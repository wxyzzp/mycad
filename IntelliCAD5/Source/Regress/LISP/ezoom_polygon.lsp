;; Degree to radian conversion
(defun dtr (deg) (/ pi 180) );defun

(defun c:ezoom_polygon()
  ;; Layer creations
  (setq num 1) 
     
  (setvar "CMDECHO" 0) 
  (command "Viewres" "Y" "20000")
;;
  (repeat 255
    (command "layer" "n" num "c" num num "")
    (setq num (+ 1 num) );setq
    (terpri) (princ "Creating layer definitions..")

  ); repeat

;; Circle creations    

 (command "layer"  "s" "0" "")

    (setq nos 6)    
  
    (setq num_A 1)
    (setq num_SA (itoa num_A) );setq     
    (princ num_SA)
     

 (While (<= num_A 255)

    (setq cen (List 0 0) 
          dist (float num_A)  
          po1 (polar cen (dtr 90) (* dist 2.0))
          );setq        

    (command "zoom" "e") 
    (setvar "Orthomode" 1)   
    ;(command "polygon" "6" cen po1)
    (command "polygon" nos cen po1)

    (command "zoom" "e") 
    (setq ent (entlast))  
    (command "change" ent "" "P" "LA" num_A "")
    (command "regen")   
      
    (setq num_A (+ 1 num_A) );setq
    (setq num_SA (itoa num_A) );setq     
    (terpri) (princ num_SA)
     
  ); While
 
    (setq num_B (- num_A 1) );setq
    (setq num_SB (itoa num_B) );setq     
    (terpri) (princ num_SB)

 (While (/= num_B 1)
    (command "erase" (entlast) "")
    (command "zoom" "e")     
    (setq num_B (- num_B 1) );setq
    (setq num_SB (itoa num_B) );setq     
    (terpri) (princ num_SB)

 );while  

    (terpri) (princ "Execution complete")
       

);defun

