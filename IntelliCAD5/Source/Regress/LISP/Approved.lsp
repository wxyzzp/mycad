(defun c:approved()

  (command "zoom" "e")
  (setq cen (getvar "viewctr") );setq
  (setq hei ( / (getvar "viewsize") 7.0) );setq 
  (setq St_hei (rtos hei) );setq 
  (command "-Style" "Approved" "C:\Windows\Systems\Fonts\Arial.ttf" St_hei "" "" "" "")
  (command "layer" "n" "Approved" "c" "1" "Approved" "s" "Approved" "")
  (command "TEXT" "J" "C" cen "30" "Approved") 

);defun
   
 
