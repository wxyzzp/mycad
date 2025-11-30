(defun C:TEST ()
(setq ss (ssget))
(prompt "\nPick two points to describe an axis of rotation:")
(setq p1 (getpoint "Base point:"))
(setq p2 (getpoint p1 "Second point:"))
(setq vec (mapcar '- P2 P1))
(setq vec (mapcar '(lambda (num) (* num (/ 1.0 (distance vec '(0 0 0))))) vec))
(setq ang (getangle p1 "Rotation:"))(print ANG)
(setq 1mcos (- 1 (cos ang)) u (car vec) v (cadr vec) w (last vec))
;creates a matrix that represents rotation about an arbitrary axis
(setq mx (list 
  (list (+ (cos ang) (* u u 1mcos)) (+ (* -1 w (sin ang)) (* u v 1mcos)) (+ (* v (sin ang)) (* u w 1mcos)) 0)
  (list (+ (* w (sin ang)) (* u v 1mcos)) (+ (cos ang) (* v v 1mcos)) (+ (* -1 u (sin ang)) (* v w 1mcos)) 0)
  (list (+ (* -1 v (sin ang)) (* u w 1mcos)) (+ (* u (sin ang)) (* v w 1mcos)) (+ (cos ang) (* w w 1mcos)) 0)
  (list 0 0 0 1)
  ))
(if (= (substr (getvar "acadver") 3) "i");IntelliCAD
  (progn
    (if (not xformss) (xload "SDS/lispxforms"))
    (xformss ss mx)
    )
  (xformss_acad ss mx)
  )
mx
)

(defun xformss_acad (ss mx)
  (setq COUNT 0)
  (repeat (sslength ss)
    (setq entname (ssname ss COUNT))
    (vla-transformby (vlax-ename->vla-object entname)  (vlax-tmatrix mx))
    (setq COUNT (1+ COUNT))
    )
  )
  
