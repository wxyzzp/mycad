(defun c:thick_text()

    (command "-style" "ET" "Times.shx" "1" "" "" "" "") 
    (setq txt (List "A" "B" "C" "D" "E" "F" "G" "H" "I" "J" "K" "L" "M" 
                    "N" "O" "P" "Q" "R" "S" "T" "U" "V" "W" "X" "Y" "Z") );setq          
    (command "viewpoint" (List -1 -1 1) )
    (command "thickness" "1")
    (command "viewres" "y" "20000")
    (setvar "highlight" 0) 

    (setq no 0) 
;   (command "text" (List 0 0) "" (nth no txt))    
;   (command "zoom" "e")  

    (While 
       (/= no 26)   
         
    (command "text" (List 0 0) "" (nth no txt))    
    (command "zoom" "e")  
    (command "dview" "all" "" "z" "2" "")  
    (command "dview" "all" "" "z" "2.5" "")  
    (command "dview" "all" "" "z" "3" "")  
    (command "hide") 
   
    (setq no (+ no 1) );setq 
    (command "erase" "all" "")
    (command "regen")

     );While
      
    (command "thickness" "0")
    (setvar "highlight" 1) 
           
);defun


 
        
