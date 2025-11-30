(defun SNAPTOGGLE (SNAPSTATE) ;Pass the snap state desired 1=On 0=Off
(setq snap (getvar "osmode"))
;(princ snap)
(if (= 1 SNAPSTATE) ;Turn it on
;(princ "Turn it on")
(if (> 16383 snap) ;button is pushed
() ;Leave it pushed else -
(setq snap (- snap 16384)) ;push button
)
;Turn it off
;(princ "Turn it off")
(if (> 16383 snap) ;button is pushed
(setq snap (+ snap 16384)) ;UNpush the button else -
() ;Leave it unpushed
))
(setvar "osmode" snap)
(princ)
)
(defun C:SnapOn ()
(SNAPTOGGLE 1)
)
(defun C:SnapOff ()
(SNAPTOGGLE 0)
)