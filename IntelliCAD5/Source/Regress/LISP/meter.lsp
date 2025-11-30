(defun dtr (deg) (/ pi deg) );defun

;; TIMER ashish !!
(defun c:meter() 
       (setq p1 (List 0 0) 
             p2 (List 2 2) 
             p3 (List 2 0) 
             p4 (List 4 2) 	
             p5 (List 4 0) 
             p6 (List 6 2) 	
             pt1 (List 1 1) 
             pt2 (List 3 1)  
             pt3 (List 5 1)  
             z   "0"  
             n    0
             num  (itoa n) 
             str   (strcat z num)   
             str1  (strcat z num)
             str2  (strcat z num)
             str3  (strcat z num)

        );setq

(setvar "CMDECHO" "0")
(command "rectangle" p1 p6 )
(command "pline" p3 p2 "")
(command "pline" p4 p5 "")
(command "zoom" "e")
(command "textsize" "1")
(command "color" "41")

(command "text" "J" "MC" pt1 "" "" str1 )
(setq txt1 (entlast) );setq

(command "text" "J" "MC" pt2 "" "" str2 )
(setq txt2 (entlast) );setq

(command "text" "J" "MC" pt3 "" "" str3 )
(setq txt3 (entlast) );setq

(setq str3_n 1)
(setq str2_n 1)
(setq str1_n 0)

(WHILE 
    (/= str1_n 10)

           (setq    num1  (itoa str1_n) 
                     str1  (strcat z num1)
                    );setq

           (command "erase" txt1 "")
           (command "text" "J" "MC" pt1 "" "" str1 )
           (setq txt1 (entlast) );setq
           (setq str1_n (+ 1 str1_n) );setq  

      (If 
          (= str2_n 60)
          (setq str2_n 0) 
         ;(setq str1_n (+ 1 str1_n) );setq  
      );If            
     
      
(While 
  (/= str2_n 60)

  (Cond 
    ( 
       (= str3_n 10)
       (command "erase" txt2 "") 
       (command "regen") 
      
     (If 
         (< str2_n 10) 
         (progn
 
           (setq    num2  (itoa str2_n) 
                    str2  (strcat z num2)
           );setq
          );progn
         
          (progn
          (setq    str2 (itoa str2_n) );setq 
          
          );progn
       );IF  

        (command "text" "J" "MC" pt2 "" "" str2 )
        (setq txt2 (entlast) );setq
        (setq str2_n (+ 1 str2_n) );setq

     (If 
        (= str3_n 10)
        (setq str3_n 0) 
        (setq str2_n (+ 1 str2_n) );setq 
    );If            

;      (If 
;          (= str2_n 60)
;          (setq str2_n 0) 
;          ;(setq str1_n 1)  
;       );If            
      
      ) 
      ):COND    


(While 
  (/= str3_n 10)

  (command "erase" txt3 "") 
  (command "regen") 

  (setq    num3  (itoa str3_n) 
           str3  (strcat z num3)
  );setq

  (command "text" "J" "MC" pt3 "" "" str3 )
  (setq txt3 (entlast) );setq
  (setq str3_n (+ 1 str3_n) );setq
 
;  (If 
;     (= str3_n 10)
;     (setq str3_n 0) 
;  );If            


);While 3

);While 2

);Main WHILE

(command "erase" txt3 "")
(command "text" "J" "MC" pt3 "" "" str )

(command "erase" txt2 "")
(command "text" "J" "MC" pt2 "" "" str )

(command "erase" txt1 "")
(command "text" "J" "MC" pt1 "" "" str )

(command "regen")
(command "color" "Bylayer")

 
);defun    
