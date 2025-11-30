; lisptest.lsp  		Copyright 1997, Visio Corporation
; 
; 8 February, 1996		Linda Eskin
;
; This is a series of tests that call functions in OURBASE.C

(defun testgetargs (argument)
	(xload "ourbase")
	(ob_lispgetargs argument)
)
