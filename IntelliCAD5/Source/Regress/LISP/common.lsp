;;; ===========================================================================
;;;
;;; common.lsp
;;;
;;; this file contains harn_init, which I stole from DanSchu.
;;;
;;; 10 Jun 98	MichaelHu
;;;
;;; ===========================================================================
(defun harn_init()
	 (if (/= "DL_Load_Failed" dl_loaded_flg); load DOSLIB if not already loaded
		  ;;; detect which CAD app. is alive and load the appropriate DOSLIB version
		  (IF (or
					 (= (getvar "_vernum") "S.0.179")
					 (= (getvar "_vernum") "P.0.31")
					 (= (getvar "_vernum") "P.0.33")
					 (= (getvar "_vernum") "S.0.79")
				) ;means we're in ACAD 14.X
				(progn
					 (setq dl_loaded_flg (arxload (strcat testscrdir "doslib14.arx") "DL_Load_Failed"))
				);progn
				;else if you fell thru to here you're in ICAD, so...
				(progn
					 (setq dl_loaded_flg (XLOAD (strcat testscrdir "doslib14i.dll") "DL_Load_Failed"))
				);progn
		  );if
		  (if (= "P.0.33" (getvar "_vernum"))
					 (setvar "dwgcheck" 0); only supported in R14.01 & later AutoCADs
		  );if
	);if
)

