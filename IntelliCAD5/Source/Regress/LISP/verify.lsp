(defun c:verify()

  (command "zoom" "e")
  (setq cen (getvar "viewctr") );setq
  (setq hei ( / (getvar "viewsize") 7.0) );setq 
  (setq St_hei (rtos hei) );setq 
  (command "-Style" "VERIFIED" "C:\Windows\Systems\Fonts\Arial.ttf" St_hei "" "" "" "")
  (command "layer" "n" "VERIFIED" "c" "1" "VERIFIED" "s" "VERIFIED" "")
  (command "TEXT" "J" "C" cen "30" "VERIFIED") 

);defun
   
  
