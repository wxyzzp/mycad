;CRDBYPCK.lsp 09/28/98 Jeff Foster
;
;OBJECTIVE***
;The purpose of this routine is to allow the user to pick points
;and have the coordinates written as entities to the drawing at
;each point as picked.
;
;TO RUN***
;At the command line, type (load "c:/lispdir/crdbypck")
;where c:/ is the drive where CRDBYPCK.lsp is contained
;where lispdir/ is the directory where CRDBYPCK.lsp is contained
;
;
;If you find this routine to be helpful, please give consideration
;to making a cash contribution of $10.00 to:
;         Jeff Foster
;         PO Box 1936
;         Garner, NC 27529
;         http://www.cadshack.com

(DEFUN C:CRDBYPCK (/ SS EN ED AS PT1 PT2 C_LAY)
  (command "undo" "m")
  (PTS)
)

(DEFUN PTS ()
    (setq
      c_lay (getvar "clayer")
      pickpt (getpoint "\nPick point to label: ")
    )
    (if (= (tblsearch "LAYER" "COORDS") nil)
      (command "layer" "m" "coords" "c" "11" "" "")
      (command "layer" "s" "coords" "")
    )
    (setq
      txt_info (getvar "textstyle")
      txt_info (tblsearch "STYLE" (strcase txt_info))
      c_hgt (cdr (assoc 40 txt_info))
    )
    (if (> c_hgt 0)
      (FIXSTYLE)
    )
    (while (/= pickpt nil)
      (setq
        xpick (nth 0 pickpt)
        ypick (nth 1 pickpt)
      )
      (COMMAND "TEXT" "M"
                      (POLAR pickpt (/ PI 2) (* 0.205 (GETVAR "DIMSCALE")))
                      (* 0.075 (GETVAR "DIMSCALE"))
                      ""
                      (STRCAT "X=" (RTOS xpick 2 2))
      )
      (COMMAND "TEXT" "M"
                      (POLAR pickpt (/ PI 2) (* 0.1025 (GETVAR "DIMSCALE")))
                      (* 0.075 (GETVAR "DIMSCALE"))
                      ""
                      (STRCAT "Y=" (RTOS ypick 2 2))
      )
      (setq pickpt (getpoint "\rPick point to label <Enter to quit>: "))
    )
    (if (> c_hgt 0)
      (RESTORESTYLE)
    )
    (SETVAR "CLAYER" C_LAY)
)

(Defun FIXSTYLE ()
  (command "-style" txt_info "" "0")
  (while (= (strcase (getvar "cmdnames")) "-STYLE")
    (command "")
  )
)

(Defun RESTORESTYLE ()
  (command "-style" txt_info "" c_hgt)
  (while (= (strcase (getvar "cmdnames")) "-STYLE")
    (command "")
  )
)

(C:CRDBYPCK)
