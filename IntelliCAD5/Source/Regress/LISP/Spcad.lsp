(setvar "CMDECHO" 0)
(prompt "\nType Spcad to run.....")
(defun mlayer (name color ltype)
     (if (tblsearch "LAYER" name)      ;if exists
          (command "LAYER" "S" name "");then set
          (progn                       ;else
               (setq regen (getvar "REGENMODE"))(setvar "REGENMODE" 0)
               (command "LAYER" "M" name)          ;make it
               (if color (command "C" color name)) ;set color if specified             
               (if ltype (command "lt" ltype name));set linetype if specified
               (command "")                        ;end of layer command
               (setvar "REGENMODE" regen)
          );end of progn
     );end of if
     (princ)
);end of defun
;   (mlayer "A-COL" "WHITE" "CONTINUOUS")
;   This draws a bike frame and wheels based on users spec.s
;   using inches
;   the dtr funtion converts degrees to radiuns
;   the rtd funtion converts radians to degrees
;
(defun dtr (a)(* pi (/ a 180.0))
)
;
(defun rtd (a)(/ (* a 180.0) pi)
)
(defun efram1 () ;bot brkt
     (setq uni1 (getvar "LUNITS"))
     (setvar "OSMODE" 0 )
     (command "UNITS" "5" "" "" "" "" "")   
     (setq p10 (getpoint "\nPick center of Bottom Bracket  "))
     (command "LAYER" "S" "GRAfram" "")
     (command "CIRCLE" p10 "D" 1.875);41mm approx 1 7/8inch
     (command "CIRCLE" p10 "D" 0.875);20mm approx 7/8inch 25.4mm=1inch
)       
(defun efram2 ();sets some points to zoom closer
     (setq zp1 (polar p10 (dtr 200) 60) 
          zp2 (polar p10 (dtr 33)  60)
     )
     ;(command "ZOOM" "W" zp1 zp2 )
)
(defun efram3 () ;seat tube
     (prompt "\n Design Name   < ")
     (setq tmpdname (getstring t " >: " ))
     (if tmpdname (setq dname tmpdname))    
     ;
     (if (= stube nil)(setq stube 1.0))
     (prompt "\nCurrent Diameter of seat tube   < ")(princ stube)
     (setq tmpstube (getdist  " >: " ))
     (if tmpstube (setq stube tmpstube))    
     ;
     (if (= ttube nil)(setq ttube 1.0))
     (prompt "\nCurrent Diameter of top tube   < ")(princ ttube)
     (setq tmpttube (getdist  " >: " ))
     (if tmpttube (setq ttube tmpttube))    
     ;
     (if (= atube nil)(setq atube 1.0))
     (prompt "\nCurrent Diameter of front angled tube   < ")(princ atube)
     (setq tmpatube (getdist  " >: " ))
     (if tmpatube (setq atube tmpatube))    
     ;
     (if (= stang nil)(setq stang (dtr 71)))
     (prompt "\nCurrent seat tube angle  < ")(if stang (princ (angtos stang)))
     (setq tmpstang (getangle  " >: " ))
     (if tmpstang (setq stang tmpstang))    
     ;
     (if (= hang nil)(setq hang (dtr 71)))
     (prompt "\nCurrent head tube angle  < ")(if hang (princ (angtos hang)))
     (setq tmphang (getangle  " >: " ))
     (if tmphang (setq hang tmphang))    
     ;
     (if (= ll nil)(setq ll 23.0))
     (prompt "\nCurrent seat tube length in inches  < ")(princ ll)
     (setq tmpll (getdist  " >: " ))
     (if tmpll (setq ll tmpll))    
     ;
     (if (= a nil)(setq a 24.0))
     (prompt "\nCurrent top tube length in inches  < ")(princ a)
     (setq tmpa (getdist  " >: " ))
     (if tmpa (setq a tmpa))    
     ;
     (if (= b nil)(setq b 21.0))
     (prompt "\nCurrent Chainstay length in inches   < ")(princ b)
     (setq tmpb (getdist  " >: " ))
     (if tmpb (setq b tmpb))    
     ;
     (if (= e nil)(setq e 3.0))
     (prompt "\nCurrent Bot. Brkt Drop in inches   < ")(princ e)
     (setq tmpe (getdist  " >: " ))
     (if tmpe (setq e tmpe))    
     ;
     (if (= f nil)(setq f 2.0))
     (prompt "\nCurrent Fork offset < Rake >    < ")(princ f)
     (setq tmpf (getdist  " >: " ))
     (if tmpf (setq f tmpf))    
     ;
     (setq vtang (- (dtr 90) stang);angle for vert from BB to center of top tube
          sbtot (- (dtr 180) stang)   ;adjust angle to proper ref point
          vbtot (- (dtr 180) vtang)   ;adjust angle to proper ref point
          p40 (polar p10 sbtot ll)   
          p11 (polar p10 (dtr 90) e)
     )
     (command "LAYER" "S" "BLUfram" "")
     (command "LINE" p10 p11 "")
     (command "LINE" p10 p40 "")   
     (setq btof (dtr 0))
     (setq ftob (dtr 180))
     (setq p10r (polar p10 (- sbtot (dtr 90)) (/ stube 2)))
     (setq p40r (polar p40 (- sbtot (dtr 90)) (/ stube 2)))
     (setq str1 (polar p10r sbtot (/ (distance p10 p40) 2)))
     (setq p10l (polar p10 (+ sbtot (dtr 90)) (/ stube 2)))
     (setq p40l (polar p40 (+ sbtot (dtr 90)) (/ stube 2)))
     (setq stl1 (polar p10l sbtot (/ (distance p10 p40) 2)))
     (setq strmid (polar p10r sbtot (/ ll 2)));fillet use
     (setq stlmid (polar p10l sbtot (/ ll 2)));fillet use
     (command "LAYER" "S" "GREfram" "")
     (command "LINE" p10r p40r "")
     (command "LINE" p10l p40l "")
)   
(defun efram4 () ;top tube
     (setq p60 (polar p40 btof a))
     (command "LAYER" "S" "BLUfram" "")
     (command "LINE" p40 p60 "")
     (setq vrt (* ll (cos  vtang))); gets vert dist to center of top tube
     (setq vrtfin (- vrt e))            ; gets vert from wheel center to ttube
     (setq vrt10 (polar p11 (dtr 90) vrtfin)) ;sets plumb line to top tube
)   
(defun efram5 () ;top tube
     (setq tbtot (angle p40 p60))
     (setq p40r (polar p40 (- tbtot (dtr 90)) (/ ttube 2)))
     (setq p60r (polar p60 (- tbtot (dtr 90)) (/ ttube 2)))
     (setq p40l (polar p40 (+ tbtot (dtr 90)) (/ ttube 2)))
     (setq p60l (polar p60 (+ tbtot (dtr 90)) (/ ttube 2)))
     (setq ttrmid (polar p40r tbtot (/ a 2)));gets midpoint for fillet use
     (setq ttlmid (polar p40l tbtot (/ a 2)));gets midpoint for fillet use
     (command "LAYER" "S" "GREfram" "")
     (command "LINE" P40r p60r "")
     (command "LINE" p40l p60l "")
)   
(defun efram6 () ;gets headset angle and determines its length
     (setq hbtot (- (dtr 180) hang));gets headset bot to top angle
     (setq httob (+ hbtot (dtr 180)));gets headset top to bottom angle
     (setq hltor (- (dtr 90) hang));gets headset left to right
     (setq hrtol (+ (dtr 90) hang));gets headset right to left
     (setq triga hltor)
     (setq trigc (/ vrtfin (cos triga)));trig func to find fr whl axle
     (setq p100 (polar p60 httob trigc));point of intersect axle-hdset angle
     (setq whl 26);27 wheel is 638mm
     (setq hdstlen (- (distance p100 p60) (+ (/ whl 2) 4)));head stem length
     ;4" above wheel hdstlen is dist between intersections of tubes
     (setq p20 (polar p100 (dtr 0) f));point of axle
     (setq hltor (- hbtot (dtr 90)))
     (setq hrtol (+ hbtot (dtr 90)))
     (setq p61 (polar p60 hbtot 2));2inch extnsn of headset above top tube
     (setq p61r (polar p61 hltor 0.75));3/4
     (setq p61l (polar p61 hrtol 0.75));3/4
     (setq p62r (polar p61r hbtot 0.25));1/4   
     (setq p62l (polar p61l hbtot 0.25));1/4
)
(defun efram7 ()
     (command "LAYER" "S" "GREfram" "")
     (command "PLINE" p61l p62l p62r p61r "C")
     (setq p65 (polar p60 httob hdstlen));center line intersections of 
     ;downtube and headset
     (setq p68 (polar p65 httob 2));2 inch
     (setq p68r (polar p68 hltor 0.75));3/4
     (setq p68l (polar p68 hrtol 0.75));3/4
     (setq p69r (polar p68r hbtot 0.25));1/4
     (setq p69l (polar p68l hbtot 0.25));1/4
     (command "PLINE" p69l p68l p68r p69r "c" )
     (command "LINE" p61r p68r "")(command "LINE" p61l p68l "")
)
(defun efram8 () ;wh center
     (setq bc (sqrt (- (* b b) (* e e))))
     (setq p30 (polar p11 ftob bc))
     (setq whlbse (distance p30 p20))
)   
(defun efram9 ()  ;chainstay
     (setq cftob (angle p10 p30))
     (command "LAYER" "S" "BLUfram" "")
     (command "LINE" p10 p30 "")
     (command "LINE" p30 p40 "")
     (setq p10r (polar p10 (- cftob (dtr 90)) 0.375));=3/8inch
     (setq p30r (polar p30 (- cftob (dtr 90)) 0.25));=1/4inch
     (command "LAYER" "S" "GREfram" "")
     (command "LINE" P10r p30r "")
     (setq p10l (polar p10 (+ cftob (dtr 90)) 0.375));=3/8inch
     (setq p30l (polar p30 (+ cftob (dtr 90)) 0.25));=1/4 inch
     (command "LINE" p10l p30l "")
     (setq cslmid (polar p10l cftob (/ (distance p10l p30l) 2)));fillet use
     (setq csrmid (polar p10r cftob (/ (distance p10r p30r) 2)));fillet use
)   
(defun efram10 () ;FORK
     (setq flng (distance p68 p20))
     (command "LAYER" "S" "BLUfram" "")
     (command "LINE" p68 p20 "")
     (command "LINE" p68 p61 "")
     (command "LINE" p30 p20 "")
     (setq fkmd (/ flng 2))
     (setq f10 (polar p68 httob fkmd))
     (setq f5l (polar p69l hltor 0.25))
     (setq f5r (polar p69r hrtol 0.25))
     (setq f10l (polar f10 hrtol 0.5))
     (setq f10r (polar f10 hltor 0.5))
     (command "LAYER" "S" "REDfram" "")
     (command "LINE" f5l f10l "")
     (command "LINE" f5r f10r "")
     (setq f2lng (distance f10 p20))
     (setq p20l (polar p20 hrtol 0.375))
     (setq p20r (polar p20 hltor 0.375))
)
(defun efram11 ()
     (setq fkrad (distance p61l p20l))
     (command "ARC" f10l "E" p20l "R" fkrad )
     (command "ARC" f10r "E" p20r "R" fkrad )
)
(defun efram12 () ;front angle tube 
     (setq fbtot (angle p10 p65))
     (setq p10r (polar p10 (- fbtot (dtr 90)) (/ atube 2)))
     (setq p65r (polar p65 (- fbtot (dtr 90)) (/ atube 2)))
     (command "LAYER" "S" "GREfram" "")
     (command "LINE" P10r p65r "")
     (setq p10l (polar p10 (+ fbtot (dtr 90)) (/ atube 2)))
     (setq p65l (polar p65 (+ fbtot (dtr 90)) (/ atube 2)))
     (command "LINE" p10l p65l "")
     (command "LAYER" "S" "BLUfram" "")
     (command "LINE" p10 p65 "")
     (setq falmid (polar p10l fbtot (/ (distance p10l p65l) 2)));fillet use
     (setq farmid (polar p10r fbtot (/ (distance p10r p65r) 2)));fillet use
)     
(defun efram13 () ;back angled tube
     (setq bbtot (angle p30 p40))
     (setq p30r (polar p30 (- bbtot (dtr 90)) 0.25))
     (setq p40r (polar p40 (- bbtot (dtr 90)) 0.25))
     (command "LAYER" "S" "GREfram" "") (command "LINE" P30r p40r "")
     (setq p30l (polar p30 (+ bbtot (dtr 90)) 0.25))
     (setq p40l (polar p40 (+ bbtot (dtr 90)) 0.25))
     (command "LINE" p30l p40l "")
     (setq barmid (polar p30r bbtot (/ (distance p30r p40r) 2)))
     (setq balmid (polar p30l bbtot (/ (distance p30l p40l) 2)))
)   
(defun efram14 () ;fillet tubes
     (command "LAYER" "OFF" "BLUfram" "")
     (command "LAYER" "S" "GREfram" "")
     (setq zp1 (polar p30 (dtr 225) b))
     (setq zp2 (polar p60 (dtr 45)  b))
     (command "ZOOM" "W" zp1 zp2 )
     (command "FILLET" "R" "0.5")
     (command "FILLET" falmid strmid )
     (command "FILLET" strmid ttrmid )
     (command "FILLET" barmid csrmid )
     (command "FILLET" barmid stlmid )
     (command "FILLET" barmid csrmid )
     (command "FILLET" csrmid stlmid )
     (command "FILLET" balmid ttlmid )
     (command "FILLET" balmid cslmid )
     (command "FILLET" farmid cslmid )
     (command "LAYER" "ON" "BLUfram" "")
     (command "ZOOM" "E")
)
;
(defun efram15 () ; spoked wheels- 
    
     (command "INSERT" "DWG/B-CRANK1.dwg" p10 "" "" "")
     
     (command "INSERT" "DWG/frwhl.dwg" p20 "" "" "")
     
     ;(command "ZOOM" "E" "")
     
;back whl
   (command "INSERT" "DWG/bkwhl.dwg" p30 "" "" "")
   (command "INSERT" "DWG/hndlbar.dwg" p61  "" "" "")
   (command "INSERT" "DWG/bikeseat.dwg" p40 "" "" "")
)                                                                           
;
(defun efram16 () 
     (setq breakx1 "******************************************")
     (princ (strcat "\n\t"  "***"  breakx1 ))
     (princ)
     (setq breakx1 "***********************")
     (princ (strcat "\n\t"  "***"  breakx1 ))
     (princ)
     (setq breakx1 "************")
     (princ (strcat "\n\t"  "***"  breakx1 ))
     (princ)
     (setq breakx1 "****")
     (princ (strcat "\n\t"  "***"  breakx1 ))
     (princ)
     (setq dname1 dname)
     (princ (strcat "\n\t"  "***Designers or bike name........."  dname1 ))
     (princ)
     (setq breakx1 "****")
     (princ (strcat "\n\t"  "***"  breakx1 ))
     (princ)
     (setq breakx1 "************")
     (princ (strcat "\n\t"  "***"  breakx1 ))
     (princ)
     (setq breakx1 "***********************")
     (princ (strcat "\n\t"  "***"  breakx1 ))
     (princ)
     (setq breakx1 "******************************************")
     (princ (strcat "\n\t"  "***"  breakx1 ))
     (princ)
     (setq stube1 (rtos stube))
     (princ (strcat "\n\t"  "     Diameter of seat tube.........."  stube1 ))
     (princ)
     (setq ttube1 (rtos ttube))
     (princ (strcat "\n\t"  "     Diameter of top tube..........." ttube1 ))  
     (princ)
     (setq atube1 (rtos atube))
     (princ (strcat "\n\t"  "     Dia. of front angled tube......" atube1 ))   
     (princ)
     (setq stang1 (rtos (rtd stang)))
     (princ (strcat "\n\t"  "     Seat tube angle................" stang1 ))
     (princ)
     (setq hang1 (rtos (rtd hang)))
     (princ (strcat "\n\t"  "     HeadSet Angle.................."  hang1 ))
     (princ)
     (setq ll1 (rtos ll))
     (princ (strcat "\n\t"  "     Lenght of seat tube........... " ll1))
     (princ)
     (setq a1 (rtos a))
     (princ (strcat "\n\t"  "     Length of top tube............." a1))
     (princ)
     (setq b1 (rtos b))
     (princ (strcat "\n\t"  "     Chain stay length.............." b1))
     (princ)
     (setq e1 (rtos e))
     (princ (strcat "\n\t"  "     Bot. Brkt. Drop................" e1))
     (princ)
     (setq f1 (rtos f))
     (princ (strcat "\n\t"  "     Fork Rake......................" f1))
     (princ)
     (setq whl1 (rtos whl))                            
     (princ (strcat "\n\t"  "     Wheel Diameter................." whl1)) 
     (princ)
     (setq whlbse1 (rtos whlbse))
     (princ (strcat "\n\t"  "     Wheel base ...................." whlbse1 ))
     (princ)
     (setq breakx1 "******************************************")
     (princ (strcat "\n\t"  "***"  breakx1 ))
     (princ)
)
(defun efram17  () 
     ;prints info out to text file 
     (setq bk (open "DWG/bike-in.txt" "w"))
     (setq breakx1 "**********************************************************")
     (print breakx1 bk)
     ;
     (setq dname1 dname)
     (setq dname2 (strcat "      Bike name......................" dname1 ))
     (print dname2 bk)
     ;
     (setq breakx1 "**********************************************************")
     (print breakx1 bk)
     ;
     (setq stube1 (rtos stube))
     (setq stube2 (strcat "      Diameter of seat tube.........."  stube1 ))
     (print stube2 bk)
     ;
     (setq ttube1 (rtos ttube))
     (setq ttube2 (strcat "      Diameter of top tube..........." ttube1 ))  
     (print ttube2 bk)
     ;
     (setq atube1 (rtos atube))
     (setq atube2 (strcat "      Diameter of Down angled tube.." atube1 ))   
     (print atube2 bk)
     ;
     (setq stang1 (rtos (rtd stang)))
     (setq stang2 (strcat "      Seat tube angle................" stang1 ))
     (print stang2 bk)
     ;
     (setq hang1 (rtos (rtd hang)))
     (setq hang2 (strcat  "      HeadSet Angle.................."  hang1 ))
     (print hang2 bk)
     ;
     (setq ll1 (rtos ll))
     (setq ll2 (strcat    "      Cen. of Brkt to cen. of Top Tube " ll1))
     (print ll2 bk)
     ;
     (setq a1 (rtos a))
     (setq a2 (strcat     "      Length of top tube............." a1))
     (print a2 bk)
     ;
     (setq b1 (rtos b))
     (setq b2 (strcat     "      Bot. Brkt. to Rear Axle........" b1))
     (print b2 bk)
     ;
     (setq whl1 (rtos whl))                            
     (setq whl2 (strcat   "      Wheel Diameter................." whl1)) 
     (print whl2 bk)
     ;
     ;
     ;   (setq spnum1 (rtos spnum))
     ;   (setq spnum2 (strcat "      Number of spokes..............." spnum1 )) 
     ;   (print spnum2 bk)
     ;
     (setq whlbse1 (rtos whlbse))
     (setq whlbse2 (strcat "      Wheel base ...................." whlbse1 ))
     (print whlbse2 bk)
     ;
     (setq breakx1 "**********************************************************")
     (print breakx1 bk)
     ;
     (close bk )
)  
;
(defun efram18 ()
     (setvar "CMDECHO" 0)
     (setq pta (getpoint "\nSelect point to begin text: "))
     (setq infile "DWG/bike-in.txt")
     ;(setq infile (getstring "\nName of text file to input: "))
     (setq h1 0.5)
     ;(setq h1 (getvar "textsize"))
     (setq f1 (open infile "r"))
     (setq pta (polar pta (/ pi 2) (* h1 1.5)))
     (while (setq t1 (read-line f1))
          (setq ptb (polar pta (* 3 (/ pi 2)) (* h1 1.5)))
          (command "text" ptb h1 0 t1 nil)
     (setq pta ptb))
     (close f1)
     (princ)
)
;
(defun c:SPCAD ()
     (setvar "CMDECHO" 0)
     (mlayer "yelfram" "YELLOW" "CONTINUOUS")
     (mlayer "blufram" "BLUE" "CONTINUOUS")
     (mlayer "redfram" "RED" "CONTINUOUS")
     (mlayer "grefram" "GREEN" "CONTINUOUS")
     (mlayer "cyafram" "CYAN" "CONTINUOUS")
     (mlayer "whifram" "WHITE" "CONTINUOUS")
     (mlayer "grafram" "14" "CONTINUOUS")

     (efram1)
     (efram2)
     (efram3)
     (efram4)
     (efram5)
     (efram6)
     (efram7)
     (efram8)
     (efram9)
     (efram10)
     (efram11)
     (efram12)
     (efram13)
     (efram14)
     (efram15)
     (efram16)
     (efram17)
     (efram18)
     (command "setvar" "LUNITS" uni1)
)
