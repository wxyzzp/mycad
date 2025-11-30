(defun c:78305()

(setq x 0)     
(setq y 0)
(setq charCount 0)

(while (< x 16)     
	
	(while (< y 16)
	
		(setq mt (chr charCount))
		(setq mt (strcat "\\" mt ";" (rtos charCount)))
		
		(command "-mtext" (list (* x 1.8) y) "w" "3" mt "")
		
		(command "chprop" "l" "" "color" charCount "")
		(setq y (+ y 1))
		(setq charCount (+ charCount 1))
	)
	
	(setq x (+ x 1))
	(setq y 0)

)

)
