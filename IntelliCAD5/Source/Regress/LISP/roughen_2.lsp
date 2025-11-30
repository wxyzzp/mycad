(defun C:ROUGHEN_2 ()          
  ;;error handler
 (setq olderr *error*)
 (defun *error* (msg)
  (if (or (= msg "Function cancelled")
   (= msg "quit / exit abort")
        )
      (princ)
      ;;otherwise report error message
      (princ (strcat "\nError: " msg))
    )
    (setvar "CMDECHO" ocmd)
    (setvar "OSMODE" oosmode)
    (setq *error* olderr)               ;restore old error handler
    (setq olderr nil)
    (princ)
  )

    (defun daed_mgetreal (promptstr initbits defreal / answer)
    (setq promptstr (strcat promptstr
                            (if defreal
                              (strcat " <" (rtos defreal 2) ">: ")
                              ": "
                            )
                    )
    )
    (if defreal
      (initget initbits)
      (initget (1+ initbits))
    )
    (setq answer (getreal promptstr))
    (if (null answer)
      (setq answer defreal)
    )
    answer
  )

  ;; (getinput)
  ;; Gets input from user: entity, Last, or Options
  (defun getinput (/ en)
    (prompt (strcat "\nDrawing scale = "
                    (rtos daed_dwgscale 2 4)
                    "  Roughness period = "
                    (rtos daed_basicziglen 2 4)
                    "  Roughness amplitude = "
                    (rtos daed_roughness 2 4)
            )
    )
    (initget "Last Options")
    (setq en (entsel "\nSelect a polyline or line or [Options]: "))
  )

  ;;Set default length and roughness of "zigzags" here
  (if (null daed_dwgscale)
    (setq daed_dwgscale    (if (zerop (getvar "DIMSCALE"))
                             1
                             (getvar "DIMSCALE")
                           )
          daed_basicziglen 1.0         ;larger is longer segment
          daed_roughness   1.0         ;larger is rougher amplitude
    )
  )

  (setq ocmd (getvar "CMDECHO"))
  (setvar "CMDECHO" 0)
  (setq oosmode (getvar "OSMODE"))
  (setvar "OSMODE" 0)

  ;;Allow user to change the roughening parameters
  (while (= (setq en (getinput)) "Options")
    (setq daed_dwgscale    (daed_mgetreal "\nDrawing scale factor" 6 daed_dwgscale)
          daed_basicziglen (daed_mgetreal
                             "\nRoughness period (larger numbers give fewer zigzags)"
                             6
                             daed_basicziglen
                           )
          daed_roughness   (daed_mgetreal
                             "\nRoughness amplitude (larger numbers give higher spikes)"
                             6
                             daed_roughness
                           )
    )
  )


  ;;User selected an entity, so proceed
  (if en
    (progn
      ;;Multiply roughening factors by drawing scale factor
      (setq basicziglen (* daed_basicziglen daed_dwgscale)
            roughness   (* daed_roughness daed_dwgscale)
      )
      (if (= en "Last")
        (setq en (entlast))             ;Last entity
        (setq en (car en))              ;selected entity
      )
      (setq etype    (cdr (assoc 0 (entget en)))
            lstverts nil
      )
      (cond
        ;;Entity is a line, so convert it to a polyline
        ((equal etype "LINE")
         (command "._PEDIT" en "_Yes" "_eXit")
         (setq en    (entlast)          ;reset en
               etype (cdr (assoc 0 (entget en)))
         )
        )
      )

      (cond

        ;;Entity is an lwpolyline
        ((equal etype "LWPOLYLINE")
         ;;Build list of vertices
         (mapcar
           '(lambda (expr)
              (if (= (car expr) 10)
                (setq lstverts (cons (cdr expr) lstverts))
              )
            )
           (entget en)
         )
         (setq lstverts (reverse lstverts))
        )

        ;;Entity is a polyline
        ((equal etype "POLYLINE")
         ;;Build list of vertices
         (setq en1 (entnext en))        ;first vertex
         (while (/= "SEQEND" (cdr (assoc 0 (entget en1))))
           (setq lstverts (cons (cdr (assoc 10 (entget en1))) lstverts)
                 en1      (entnext en1) ;next vertex
           )
         )
         (setq lstverts (reverse lstverts))
        )

        (T (prompt "\nObject is not a polyline or line."))
      )

      ;;There are vertices, so process the lwpolyline or polyline
      (if lstverts
        (progn
          (command "._PEDIT" en "_Edit") ;edit vertex
          (setq vertnum 1)
          (while (< vertnum (length lstverts))
            (setq vtx1    (nth (1- vertnum) lstverts)
                  vtx2    (nth vertnum lstverts)
                  seglen  (distance vtx1 vtx2)
                  segang  (angle vtx1 vtx2)
                  zignum  (fix (/ seglen basicziglen)) ;# of "zigzags"...
                  zignum  (max zignum 2) ; but not less than 2
                  ziglen  (/ seglen zignum) ;zigzag length
                  zigdist ziglen
                  i       2
            )

            (repeat (1- zignum)
              (setq newvtx     (polar vtx1 segang zigdist) ;vertex init. loc.
                    newvtx     (trans newvtx 0 1)
                    wanderdist (* roughness (daed_randnum))
                    wanderang  (if (= (rem i 2) 1) ;wander back & forth
                                 (+ segang (/ pi 4))
                                 (- segang (/ pi 4))
                               )
              )
              (command "_Insert"
                       newvtx
              )
              ;;PEDIT behaves inconsistently with polylines vs. lwpolylines
              (if (equal etype "POLYLINE")
                (command "_Next")
              )
              (command "_Move"
                       (polar newvtx wanderang wanderdist)
              )
              (setq zigdist (+ ziglen zigdist)
                    i       (1+ i)
              )
            )

            (command "_Next")
            (setq vertnum (1+ vertnum))
          )
          (command "_eXit" "_eXit")     ;exit PEDIT
          (redraw en)
        )
      )
    )

    (prompt "\nNo object selected.")
  )

  (setvar "CMDECHO" ocmd)
  (setvar "OSMODE" oosmode)
  (setq *error* olderr)
  (princ)
)

(defun daed_randnum (/ modulus multiplier increment random)
  (if (not daed_seed)
    (setq daed_seed (getvar "DATE"))
  )
  (setq modulus    65536
        multiplier 25173
        increment  13849
        daed_seed  (rem (+ (* multiplier daed_seed) increment) modulus)
        random     (/ daed_seed modulus)
  )
)

(prompt "\nTip: ROUGHEN.LSP now supports  lwpolylines."
)
(prompt   "\nROUGHEN Type ROUGHEN to run it."
)
(princ)
