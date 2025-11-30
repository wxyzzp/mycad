;;;
;;; Lisp Acceptance Test
;;;
;;; 17 Jun 98	MichaelHu
;;;

;;; ----------------------------------------------------------------------
;;; setup stuff.
;;; ----------------------------------------------------------------------

;;
;; error handler.
;;
; save the original error handler in a publicly
;	available spot, so we can get it back later if
;	something goes wrong.
(if (= nil olderrx)
	(setq olderrx *error*)
)
(setq olderr *error*)
(defun *error* ( errmsg / )
	(if
		(or
			(/= msg "Function cancelled")
			(= msg "quit / exit abort")
		)
		; if the condition is met.
		(progn
			(logmsg "Exiting due to cancel, quit, or exit")
		)
	)
	(logmsg (strcat "***Error (" errloc "): " errmsg))
	(deinit)	
	(princ)
)

;;
;; this test assumes it is run by a higher power.  for stand-alone
;;		testing, uncomment the following setq's.
;(setq testscrdir "e:/tools/acctests/"); local machine.
(setq testscrdir "Lisp/"); SourceSafe shadowed.

;;; ----------------------------------------------------------------------
;;; logging stuff.
;;; ----------------------------------------------------------------------
(setq logfil (strcat "log/LispAccTest-" logfilnam ".log"))

;;
;; initlog
;;
;; truncate the log file.
;;
(defun initlog ( / )
	(setq outfil (open logfil "w"))
	(if (= nil outfil)
		(princ "\n\n***Invalid log file!")
	)
)

;;
;; logmsg
;;
;; write a message to the log file.
;;
(defun logmsg ( msg / )
	(princ (strcat "\n" msg))
	(setq outfil (open logfil "a"))
	(if (/= nil outfil)
		(progn
			(write-line msg outfil)
			(close outfil)
		)
		(progn
			(princ "\n\n***Invalid log file!")
		)
	)
)

;;
;; logang
;;
;; write a "case = result" message, where result is an angle.
;;
(defun logang ( case result / )
	(logall case (angtos result 0 4))
)

;;
;; 
;; logbool
;;
;; write a "case = result" message, where result is a boolean.
;;
(defun logbool ( case result / )
	(logall case result)
)

;;
;; 
;; logflt
;;
;; write a "case = result" message, where result is a float.
;;
(defun logflt ( case result / )
	(logall case result)
)

;;
;; 
;; loglist
;;
;; write a "case = result" message, where result is a list.
;;
(defun loglist ( case result / )
	(logall case result)
)

;;
;; 
;; logall
;;
;; write a "case = result" message, where result is anything.
;;
(defun logall ( case result / myres mytab fst)
	(setq myres result)

	(if (listp myres)
		; it's a list.
		(progn
			(if (/= "\t" (substr case 1 1))
				(setq mytab "\t")
				(setq mytab (strcat case "\t"))
			)
			(if (= nil myres)
				(progn
					(logmsg (strcat case " = nil"))
				); nil
				(progn
					(logmsg (strcat case ":"))
					(while (/= 0 (length myres))
						(setq fst (car myres))
						(setq myres (cdr myres))
						(logall mytab fst)
					)
				); not nil, so a list.
			)
		); progn a list.
		; it's something other than a list.
		(progn
			(cond
				((= 'STR		(type myres))	(logmsg (strcat case " = " result)))
				((= 'INT		(type myres))	(logmsg (strcat case " = " (rtos result 2 0))) )
				((= 'REAL		(type myres))	(logmsg (strcat case " = " (rtos result 2 4))) )
				((= 'SYM		(type myres))	(logmsg (strcat case " = T")) )
				((= nil			(type myres))	(logmsg (strcat case " = nil")) )
				((= 'ENAME		(type myres))	(logmsg (strcat case " = ENAME")) )
				((= 'EXSUBR		(type myres))	(logmsg (strcat case " = EXSUBR")) )
				((= 'FILE		(type myres))	(logmsg (strcat case " = FILE")) )
				((= 'PAGETB		(type myres))	(logmsg (strcat case " = PAGETB")) )
				((= 'PICKSET		(type myres))	(logmsg (strcat case " = PICKSET")) )
				((= 'SUBR		(type myres))	(logmsg (strcat case " = SUBR")) )
				(T								(logmsg (strcat mytab "Unknown type: " case)) )
			)
		); progn not a list.
	)
)

;;
;; logstr
;;
;; write a "case = result" message, where result is a string.
;;
(defun logstr ( case result / )
	(logall case result)
)

;;; ----------------------------------------------------------------------
;;; init/deinit stuff.
;;; ----------------------------------------------------------------------

;;
;; init
;;
;; initializes the tests.
;;
(defun init ()
	(princ "\n\nStarting initialization...")
	(initlog)
	(load (strcat testscrdir "common"))
	(harn_init)
	(load (strcat testscrdir "error"))
	(princ "Finished")
)

;;
;; deinit
;;
;; de-initializes the tests and closes anything
;;		init opened.
;;
(defun deinit ()
	(princ "\n\nStarting deinitialization...")
	(setq *error* olderr)
	(princ "Finished")
)

;;;
;;; LoadTest
;;;
;;; helper to load this file.
;;;
(defun c:RunTest ()
	(command "._SCRIPT" (strcat testscrdir logfilnam "/AccTest-" logfilnam))
)
