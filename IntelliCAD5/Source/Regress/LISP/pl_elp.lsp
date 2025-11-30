;;Tip1765:  PL_ELP.LSP        POLYLINE ELLIPSE       (C)2002, Bill Zondlo  $50 Bonus Winner

(defun
   C:PL_ELP
   (/ AN1 ANG ANS DEG DS1 DS2 INC NUM PST PSTT PT1 PT2 PT3 SPT STT)
 ;-----------;
  (setvar "blipmode" 1)
  (setvar "cmdecho" 0) ;---------;
  (setq
    PT1
     (getpoint "\nSelect Center Point: ") ;select center point.
  ) ;_ end of setq
  (initget 1)
  (setq
    PT2
     (getpoint PT1 "\nFirst Axis Point: ") ;select first end point.
  ) ;_ end of setq
  (setvar "blipmode" 0)
  (initget 1)
  (setq
    PT3
     (getpoint PT1 "\nSecond Axis Point: ") ;select second end point.
  ) ;---------;
  (setq
    ANG  (angle PT1 PT2) ;angle of long axis.
    DS1  (distance PT1 PT2) ;long axis length.
    DS2  (distance PT1 PT3) ;short axis length.
    INC  0 ;start point of full ellipse
    NUM  360 ;number of increments.
    DEG  (/ (* pi 2.0) 360.0) ;one degree, in radians.
    PST  (+ (car PT1) (* (- DS1) (cos INC)))
 ;formula as found in DXF group codes
    PSTT (+ (cadr PT1) (* (- DS2) (sin INC)))
 ;to determine points for an ellipse.
    SPT  (list PST PSTT) ;first xy point.
    AN1  (angle PT1 SPT) ;angle from start point.
    SPT  (polar PT1 (+ AN1 ANG) (distance PT1 SPT)) ;add angle of axis.
    INC  (+ INC DEG) ;increase angle for next point.      
  ) ;---------;
  (command "_.pline" SPT) ;start poly
 ;---------;
  (repeat NUM ;draw ellipse
    (setq
      PST  (+ (car PT1) (* (- DS1) (cos INC))) ;  
      PSTT (+ (cadr PT1) (* (- DS2) (sin INC))) ;
      SPT  (list PST PSTT) ;
      AN1  (angle PT1 SPT) ;
      SPT  (polar PT1 (+ AN1 ANG) (distance PT1 SPT)) ;
      INC  (+ INC DEG) ;next angle
    ) ;_ end of setq
    (command SPT) ;continue pline
  ) ;end repeat
  (command "c") ;close pline
 ;--------;
  (setvar "cmdecho" 1)
) ;end defun
