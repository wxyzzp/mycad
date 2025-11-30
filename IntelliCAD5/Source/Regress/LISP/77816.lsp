(defun c:frame ()
(setq sc1 nil yo1 nil ta1 nil yo nil ta nil zx1 nil zx2 nil zx3 nil zx4 nil)
(setq sc1 1000)
(setq yo1 3640)
(setq ta1 2570)   
     (setq yo (* sc1 yo1))
     (setq ta (* sc1 ta1))


    (setq zx1 (list 0 0))
    (setq zx2 (list ta yo))
    (setq zx3 (list yo ta))
    (setq zx4 (list 0 yo))

    (command "color" 2)

  (command "pline" zx1 zx2 zx3 zx4 "c")

  (command "zoom" "a")

)
