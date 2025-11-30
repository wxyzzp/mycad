;===================================================================================
; FREEHAND.lsp   (Command function for AutoCAD) 
; (c) Copyright 2000 TimeSavers for CAD
; from: www.timesaversforcad.com
;-----------------------------------------------------------------------------------
; Description:
;
; Modifies lines and polylines to appear as if drawn freehand.
; Two things occur:
;   1. Corners are given slight overlap.
;   2. Lines are given slight "wiggle" to them.
; User can specify degree of both overlap & wiggle [1 small - 5 large, 0-none]
; Accomplished by first lengthening each line, then replacing line
; with LWPOLYLINE(R14) or POLYLINE(<R14) and adding slightly offset vertices.
;
; Globals: fh_scal(freehand scale), fh_oscl (overlap scale), fh_wscl (wiggle scale)
;===================================================================================
(alert "\nChanges made by FREEHAND cannot \nbe restored once you leave drawing.")

(defun c:freehand (/ *fh:err* o_m c_e fh_s fh_os fh_ws x ss ssp e ed llc p1 p2 d el ne
                     vertno ang vertd offd offang vlst elst lp dsf)
  ;-ERROR HANDLING
  (defun *fh:err* (m)
    (or (member m '("Function cancelled" "quit / exit abort" "console break"))
        (prompt (strcat "\n< " m " >\n"))
    )
    (setvar "osmode" o_m)(setvar "cmdecho" c_e)
    (setq *error* *e*)(princ)
  )

  ;-GET VALUES/SETUP
  (setq *e* (cond (*e*)(*error*))
        *error* *fh:err*
        c_e (getvar "cmdecho")
        o_m (getvar "osmode")
        dsf (getvar "dimscale") ;TSC (scale)
  )
  (setvar "cmdecho" 0)
  (command "undo" "e" "undo" "g")
  (setvar "osmode" 0)
  (or (= (type fh_scl) 'REAL)(setq fh_scl 1.0))
  (or (= (type fh_oscl) 'INT)(setq fh_oscl 3))
  (or (= (type fh_wscl) 'INT)(setq fh_wscl 3))

  ;-GET FREEHAND SCALE FACTOR
  (if (setq fh_s (getreal (strcat "\nFreehand Scale <" (rtos fh_scl 2) ">: ")))
    (setq fh_scl fh_s)
  )

  ;-GET AMOUNT OF OVERLAP
  (initget "0 1 2 3 4 5")
  (if (setq fh_os (getint (strcat "\nLine OVERLAP amount [1 small-5 large, 0 none] <"(itoa fh_oscl)">: ")))
    (setq fh_oscl fh_os)
  )
  (cond ((= fh_oscl 0)(setq fh_os nil))
        ((= fh_oscl 1)(setq fh_os 0.2))
        ((= fh_oscl 2)(setq fh_os 0.7))
        ((= fh_oscl 3)(setq fh_os 1.2))
        ((= fh_oscl 4)(setq fh_os 1.7))
        ((= fh_oscl 5)(setq fh_os 2.2))
  )
  (initget "0 1 2 3 4 5")

  ;-GET AMOUNT OF WIGGLE
  (if (setq fh_ws (getint (strcat "\nLine WIGGLE amount  [1 small-5 large, 0 none] <"(itoa fh_wscl)">: ")))
    (setq fh_wscl fh_ws)
  )
  (cond ((= fh_wscl 0)(setq fh_ws nil))
        ((= fh_wscl 1)(setq fh_ws 0.02))
        ((= fh_wscl 2)(setq fh_ws 0.07))
        ((= fh_wscl 3)(setq fh_ws 0.12))
        ((= fh_wscl 4)(setq fh_ws 0.17))
        ((= fh_wscl 5)(setq fh_ws 0.22))
  )

  ;-SELECT OBJECTS
  (if (setq ss (ssget '((-4 . "<OR")(0 . "LINE")(0 . "LWPOLYLINE")(0 . "POLYLINE")(-4 . "OR>"))))
    (progn
      (prompt "\nFREEHAND in progress...")
      (if (setq ssp (ssget "P" '((-4 . "<NOT")(0 . "LINE")(-4 . "NOT>"))))
        (progn
          (setq el (entlast) x 0)
          (while (setq ne el el (entnext el)))
          (while (< x (sslength ssp))
            (ssdel (setq e (ssname ssp x)) ss)
            (command ".explode" e)
            (setq x (1+ x))
          )
          (while (setq ne (entnext ne))(ssadd ne ss))
        )
      )
      (setq x 0)
      (while (< x (sslength ss))
        (setq e (ssname ss x) ed (entget e))
        ;-GET NEEDED INFO FROM LINE
        (setq llc (list (assoc 8 ed)) ;start Layer/Ltype/Color list
              p1 (cdr (assoc 10 ed))
              p2 (cdr (assoc 11 ed))
              ang (angle p1 p2)
              d (distance p1 p2)
        )
        (if (assoc 6 ed)(setq llc (cons (assoc 6 ed) llc)))
        (if (assoc 62 ed)(setq llc (cons (assoc 62 ed) llc)))
        ;-CREATE OVERLAP
        (if (and fh_os (> d 0))
          (progn
            ;*-overlap distance, adjust # after "dsf" - larger=less
            (setq ovlpd (+ (* (/ dsf 160) fh_scl)(* fh_os fh_scl))
                  p1 (polar p1 ang (- ovlpd))
                  p2 (polar p2 ang ovlpd)
            )
            (setq ed (subst (cons 10 p1)(assoc 10 ed) ed)
                  ed (subst (cons 11 p2)(assoc 11 ed) ed)
            )
            (entmod ed)
            (setq d (distance p1 p2))
          )
        )
        ;-CREATE WIGGLE
        (if (and fh_ws (> d 0))
          (progn
            (setq vlst nil
                  lp p1
                  ;*-number of vertices, adjust # after "dsf" -larger=more
                  vertno (fix (/ d (+ (* (/ dsf 16) fh_scl)(* 3 fh_scl)))) ;;;(+ (* (/ dsf 16) fh_scl) 3)))
                  vertd (if (> vertno 0)(/ d (1+ vertno)))
                  ;*-offset distance for vertices, adjust # after "dsf" -larger=less
                  offd (+ (* (/ dsf 480) fh_scl)(* fh_ws fh_scl))
                  offang (+ ang (/ pi 2))
            )
            ;-MAKE LIST OF VERTICES FOR NEW PLINE
            (repeat vertno
              (setq lp (polar lp ang vertd)
                    vlst (cons (polar lp offang offd) vlst)
                    offang (+ offang pi)
              )
            )
            ;-DELETE LINE
            (entdel e)(setq e nil)
            (if (= (substr (getvar "acadver") 1 2) "14")
              (progn
                (setq elst (list (cons 10 p2)'(40 . 0.0)'(41 . 0.0)'(42 . 0.0)))
                (foreach v vlst
                  (setq elst (append (list (cons 10 v)'(40 . 0.0)'(41 . 0.0)'(42 . 0.0)) elst))
                )
                (setq elst (append (list (cons 10 p1)'(40 . 0.0)'(41 . 0.0)'(42 . 0.0)) elst))
                (entmake (append '((0 . "LWPOLYLINE")(100 . "AcDbEntity"))
                                 llc
                                 (list '(100 . "AcDbPolyline")
                                       (cons 90 (/ (length elst) 4))
                                       '(70 . 0)
                                 )
                                 elst
                ))
              )
              (progn
                ;-REPLACE WITH PLINE
                (entmake (append '((0 . "POLYLINE")(70 . 128)(66 . 1)) llc))
                (entmake (append (list '(0 . "VERTEX")(cons 10 p2)) llc))
                (foreach n vlst (entmake (append (list '(0 . "VERTEX")(cons 10 n)) llc)))
                (entmake (append (list '(0 . "VERTEX")(cons 10 p1)) llc))
                (entmake (cons '(0 . "SEQEND") llc))
              )
            )
          )
        )
        (setq x (1+ x))
      )
      (command "undo" "e")
      (prompt "\nFreehand complete. ")
    )
  )

  ;-RETURN VALUES
  (setvar "osmode" o_m)(setvar "cmdecho" c_e)
  (setq *error* *e*)
  (princ)
)
;-END FILE
