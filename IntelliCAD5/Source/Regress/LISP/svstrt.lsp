(prompt "\nCommands: SVSTRT ")
(defun c:svstrt ()
   (setq zbase (getreal "\nY axis value for Z=0 <0.0>: "))
   (if (null zbase) (setq zbase 0.0))
   (setvar "CMDECHO" 0)
)
; -----------------------------------------------------------------
(prompt "\nCommands: SVZSET ")
(defun c:svzset ()
   (setq tz (getreal "\nStarting Elevation bottom:")) ; Z1 value?
   (cond ((null tz)  ; No entry, use CP or ZBASE values.
             (if (null cp) ; Current point equal to nil?
                   (setq pp (list zbase zbase)) ; Use base value.
                   (setq pp cp) ; Set elevations from current point.
             )
         )
         (t          ; User entry made, ask for Z2 value.
             (setq pp (+ tz zbase)) ; Add offset to entry, put in PP.
             (setq tz (getreal "\nStarting Elevation top:"))
             (if (null tz); No Input read?
                        (setq pp (list pp pp)) ; Use Bottom entry.
                        (setq pp (list pp (+ tz zbase))) ;Add offset
             )
         )
   )
   (setq tz (getreal "\nEnding Elevation bottom:"))
   (cond ((null tz) ; No user entry made, use previous point.
               (setq cp pp)
         )
         (t  ; User made entry.
             (setq cp (+ tz zbase)) ; Add offset and save in CP.
             (setq tz (getreal "\nEnding Elevation top:"))
             (if (null tz)  ; No input read?
                      (setq cp (list cp cp)) ;Use bottom value for top.
                      (setq cp (list cp (+ tz zbase))) ; Add offset.
             )
         )
   )
)
; ----------------------------------------------------------------
(prompt "\nCommands: SVLINE ")
(defun c:svline ()
  (setq pp (list pp (getpoint "\nStarting Point:")))
  (if (eq (cadr pp) nil) ; No input read?
      (setq pp (list (car pp) (getvar "LASTPOINT"))) ; Use last point
  )
  (prompt "\nTo Point:")
  (command "_line" (cadr pp)) ; Start line command for Rubber band.
  (setq cp (list cp (getpoint))) ; Build Current point list.
  (while (not (eq (cadr cp) nil)) ; As long as points are entered.
     (command (cadr cp) "") ; Finish line command for top view.
     (if (= (caadr pp) (caadr cp))
        (prompt "No change in X") ; No change in X, do not draw side view.
        (command "_line" (list (caadr pp) (caar pp))   ; Draw side view
                        (list (caadr pp) (cadar pp))  ; Otherwise.
                        (list (caadr cp) (cadar cp))
                        (list (caadr cp) (caar cp))
                        "_c"
        )
     )
     (setq pp cp) ; Set current point into previous point.
     (prompt "\nTo Point:")
     (command "_line" (cadr pp)) ; Start line command for Rubber band.
     (setq cp (list (car cp) (getpoint))) ; Build current point list.
   )
   (command "") ; Terminate the line command.
   (setq pp (car pp)) ; Set lists to Z vector data only.
   (setq cp (car cp))
)

(princ)
