;;;
;;; Lisp Acceptance Test
;;;	Symbol Table functions
;;;		tblsearch
;;;
;;; 17 Jun 98	MichaelHu
;;;
	(logmsg "")
	(load (strcat testscrdir "error"))

	;;; ------------------------------------------
	;;; no arguments.
	(defun noargs ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(setq rc (snvalid))
		(logall errloc rc)
		(setq *error* prverr)
	)
	;;; ------------------------------------------
	;;; no arguments.
	(defun do_tblsearch ( tbl / ans )
		(setq ans (tblsearch tbl "test"))
		(if (/= nil ans)
			(foreach pair ans
				(logall (itoa (car pair)) (cdr pair))
			)
			(logall tbl "didn't find it")
		)
	)

	(defun do_appid ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(logall "APPID Table" "")
		(do_tblsearch "appid")
		(setq *error* prverr)
	)

	(defun do_block ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(logall "BLOCK Table" "")
		(do_tblsearch "block")
		(setq *error* prverr)
	)

	(defun do_dimstyle ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(logall "DIMSTYLE Table" "")
		(do_tblsearch "dimstyle")
		(setq *error* prverr)
	)

	(defun do_layer ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(logall "LAYER Table" "")
		(do_tblsearch "layer")
		(setq *error* prverr)
	)

	(defun do_ltype ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(logall "LTYPE Table" "")
		(do_tblsearch "ltype")
		(setq *error* prverr)
	)

	(defun do_style ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(logall "STYLE Table" "")
		(do_tblsearch "style")
		(setq *error* prverr)
	)

	(defun do_ucs ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(logall "UCS Table" "")
		(do_tblsearch "ucs")
		(setq *error* prverr)
	)

	(defun do_view ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(logall "VIEW Table" "")
		(do_tblsearch "view")
		(setq *error* prverr)
	)

	(defun do_vport ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(logall "VPORT Table" "")
		(do_tblsearch "vport")
		(setq *error* prverr)
	)

