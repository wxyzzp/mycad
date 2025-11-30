;; Degree to radian conversion
(defun dtr (deg) (/ pi 180) );defun

(defun c:ezoom_ellipse()
  
  (setvar "CMDECHO" 0) 
  (command "Viewres" "Y" "20000")
;; Layer creations

  (setq num 1) 
  (repeat 255
    (command "layer" "n" num "c" num num "")
    (setq num (+ 1 num) );setq
    (terpri) (princ "Creating layer definitions..")

  ); repeat


;; Ellipse creations    

  (command "layer"  "s" "0" "")
  
  (setq num_A 1)
  (setq num_SA (itoa num_A) );setq     
  (princ num_SA)

 (While (<= num_A 255)
    
    (setq cen (List 0 0) 
          po1 (polar cen (dtr 90) (* num_A 1.0))
          );setq          

    (command "zoom" "e") 
          
    (command "Ellipse" cen po1 "r" "45")

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
):while  

    (terpri) (princ "Execution complete")
       

);defun

