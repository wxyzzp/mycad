;; This utility lisp routine can be used to quickly switch
;; from using DWGdirect as the code path for opening and 
;; saving drawings (with support for AutoCAD 2004 reading),
;; to using the old OpenDWG code path. Note that it is a
;; toggle routine. If DWGdirect is not active, when you
;; execute this routine it will activate it. If you again
;; execute this routine it will deactivate it.

;; You could save this file with the name acad.lsp and place
;; it where your ICAD.EXE program is to always have access
;; to it, or add this routine to your already existing icad.lsp.

;; Another way of doing this so that IntelliCAD always uses
;; DWGdirect for opening and saving files is to set the system
;; environment variable 'USEDWGDIRECT' via the control panel.
;; You can set it to anything you like. Here I use the string
;; "DWGdirectReader". 
;; Then IntelliCAD will default to using DWGdirect all the time.
;;

;; Once the DWGdirect code path is turned on perminently, this
;; routine becomes obsolete, and can be deleted.

(setvar "cmdecho" 0)

(defun c:dd()
	(setq tmp (getenv "USEDWGDIRECT"))
	(if (= (getenv "USEDWGDIRECT") nil) 
			(setenv "USEDWGDIRECT" "DWGdirectReader")
			(setenv "USEDWGDIRECT" "")
	)
	(if (= (getenv "USEDWGDIRECT") nil)
	    (print "DWGdirect reader/writer off")
	    (print "DWGdirect reader/writer on")
	)
	(princ)
)

(setvar "cmdecho" 1)

