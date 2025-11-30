;;;
;;; Lisp Acceptance Test
;;;	Symbol Table functions
;;;		tblobjname
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
	(defun do_tblobjname ( tbl / ans )
		(logall "Found test?" (= nil (tblobjname tbl "test")))
	)

	(defun do_appid ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(logall "APPID Table" "")
		(do_tblobjname "appid")
		(setq *error* prverr)
	)

	(defun do_block ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(logall "BLOCK Table" "")
		(do_tblobjname "block")
		(setq *error* prverr)
	)

	(defun do_dimstyle ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(logall "DIMSTYLE Table" "")
		(do_tblobjname "dimstyle")
		(setq *error* prverr)
	)

	(defun do_layer ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(logall "LAYER Table" "")
		(do_tblobjname "layer")
		(setq *error* prverr)
	)

	(defun do_ltype ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(logall "LTYPE Table" "")
		(do_tblobjname "ltype")
		(setq *error* prverr)
	)

	(defun do_style ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(logall "STYLE Table" "")
		(do_tblobjname "style")
		(setq *error* prverr)
	)

	(defun do_ucs ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(logall "UCS Table" "")
		(do_tblobjname "ucs")
		(setq *error* prverr)
	)

	(defun do_view ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(logall "VIEW Table" "")
		(do_tblobjname "view")
		(setq *error* prverr)
	)

	(defun do_vport ( / prverr )
		(setq prverr *error*)
		(setq *error* silent_err)
		(logall "VPORT Table" "")
		(do_tblobjname "vport")
		(setq *error* prverr)
	)

