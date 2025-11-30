;;;
;;; Lisp Acceptance Test
;;;	Symbol Table functions
;;;		tblnext
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
	(defun do_tblnext ( tbl / is_more cnt )
		(setq cnt 0)
		(setq is_more t)
		(while is_more
			(if (= 0 cnt)
				(setq is_more (tblnext tbl t))
				(setq is_more (tblnext tbl))
			)
			(foreach pair is_more
				(logall (itoa (car pair)) (cdr pair))
			)
			(setq cnt (1+ cnt))
		)
	)

	(defun do_appid ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(logall "APPID Table" "")
		(do_tblnext "appid")
		(setq *error* prverr)
	)

	(defun do_block ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(if (= nil (tblsearch "block" "test"))
			(progn
				(command "._line" '(0 0) '(5 5) '(5 0) "")
				(command "._block" "test" '(0 0) (entlast) "")
			)
		)
		(logall "BLOCK Table" "")
		(do_tblnext "block")
		(setq *error* prverr)
	)

	(defun do_dimstyle ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(logall "DIMSTYLE Table" "")
		(do_tblnext "dimstyle")
		(setq *error* prverr)
	)

	(defun do_layer ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(logall "LAYER Table" "")
		(do_tblnext "layer")
		(setq *error* prverr)
	)

	(defun do_ltype ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(logall "LTYPE Table" "")
		(do_tblnext "ltype")
		(setq *error* prverr)
	)

	(defun do_style ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(logall "STYLE Table" "")
		(do_tblnext "style")
		(setq *error* prverr)
	)

	(defun do_ucs ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(if (= nil (tblsearch "ucs" "test"))
			(progn
				(command "._ucs" "_save" "test")
			)
		)
		(logall "UCS Table" "")
		(do_tblnext "ucs")
		(setq *error* prverr)
	)

	(defun do_view ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(if (= nil (tblsearch "view" "test"))
			(progn
				(command "._view" "_save" "test")
			)
		)
		(logall "VIEW Table" "")
		(do_tblnext "view")
		(setq *error* prverr)
	)

	(defun do_vport ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(if (= nil (tblsearch "vport" "test"))
			(progn
				(command "._vports" "_save" "test")
			)
		)
		(logall "VPORT Table" "")
		(do_tblnext "vport")
		(setq *error* prverr)
	)

