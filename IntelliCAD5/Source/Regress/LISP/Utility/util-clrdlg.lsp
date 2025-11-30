;;;
;;; Lisp Acceptance Test
;;;	Utility functions
;;;		acad_colordlg
;;;
;;; 17 Jun 98	MichaelHu
;;;

*** CHECKS ***
- verify returns correct value.
- verify canceling returns nil.
- verify correct color swatch appears.
- verify selecting a color square changes the currently selected color.

;;; ---------------------------------------------
;;; no arguments.
- (acad_colordlg) (Bad arguments)

;;; ---------------------------------------------
;;; boundary tests.
- (acad_colordlg n)
	where n is:
		--- numerics.
		0	(BYBLOCK is default)
		9	(9  is default)
		-0	(BYBLOCK  is default)
		-9	(Bad arguments)
		--- invalid inputs.
		- just below below 0.
		/	(Bad arguments)
		- just below above 9.
		:	(Bad arguments)
		- booleans.
		t	(Bad arguments)
		nil	(Bad arguments)
		--- integers.
		- zero.
		0				(BYBLOCK is default)
		- min int.
		-2147483648		(Bad arguments)
		- max int.
		2147483647		(Bad arguments)
		- min int minus one.
		-2147483649		(Bad arguments)
		- max int plus one.
		2147483648		(Bad arguments)
		--- reals.
		- zero.
		0.0				(Bad arguments)
		- min real.
		9.9e-324		(Bad arguments)
		- max real.
		9.9e307			(Bad arguments)
		- min real minus one.
		9.9e-325		(Bad arguments)
		- max real plus one.
		9.9e308			(Invalid floating point value)
		- invalid decimal.
		.5				(invalid dotted pair)
- (acad_colordlg 3)
	then enter n, where n is:
		--- numerics.
		0	(BYBLOCK is default)
		9	(9  is default)
		-0	(BYBLOCK  is default)
		-9	(Bad arguments)
		--- invalid inputs.
		- just below below 0.
		/	(Bad arguments)
		- just below above 9.
		:	(Bad arguments)
		- booleans.
		t	(Bad arguments)
		nil	(Bad arguments)
		--- integers.
		- zero.
		0				(BYBLOC is default)
		- min int.
		-2147483648		(Bad arguments)
		- max int.
		2147483647		(Bad arguments)
		- min int minus one.
		-2147483649		(Bad arguments)
		- max int plus one.
		2147483648		(Bad arguments)
		--- reals.
		- zero.
		0.0				(Bad arguments)
		- min real.
		9.9e-324		(Bad arguments)
		- max real.
		9.9e307			(Bad arguments)
		- min real minus one.
		9.9e-325		(Bad arguments)
		- max real plus one.
		9.9e308			(Invalid floating point value)
		- invalid decimal.
		.5				(invalid dotted pair)

;;; ---------------------------------------------
;;; min/max valid color with.
- (acad_colordlg n)
	where n is:
		- min color.
		0				(BYBLOCK is default)
		- max color.	(BYLAYER is default)
		256
		- min plus one.
		1				(Red is default)
		- max minus one.
		255				(255 is default)
		- min minus one.
		-1				(Bad arguments)
		- max plus one.
		257				(Bad arguments)
- (acad_colordlg n nil)
	where n is:
		- min color.
		0				(Bad arguments)
		- max color.
		256				(Bad arguments)
		- min plus one.
		1				(Red is default; no BYBLOCK/BYLAYER buttons)
		- max minus one.
		255				(255 is default; no BYBLOCK/BYLAYER buttons)
		- min minus one.
		-1				(Bad arguments)
		- max plus one.
		257				(Bad arguments)
- (acad_colordlg 3)
	then enter n, where n is:
		- min color.
		0				(BYBLOCK is default)
		- max color.	(BYLAYER is default)
		256
		- min plus one.
		1				(Red is default)
		- max minus one.
		255				(255 is default)
		- min minus one.
		-1				(Bad arguments)
		- max plus one.
		257				(Bad arguments)
- (acad_colordlg 3 nil)
	then enter n, where n is:
		- min color.
		0				(Bad arguments)
		- max color.	(Bad arguments)
		256
		- min plus one.
		1				(Red is default; no BYBLOCK/BYLAYER buttons)
		- max minus one.
		255				(255 is default; no BYBLOCK/BYLAYER buttons)
		- min minus one.
		-1				(Bad arguments)
		- max plus one.
		257				(Bad arguments)

;;; ---------------------------------------------
;;; IntelliCAD help tests.
- (acad_colordlg 1)			(Red is default)
- (acad_colordlg 1 t)		(Red is default)
- (acad_colordlg 1 nil)		(Red is default; no BYBLOCK/BYLAYER buttons)

;;; ---------------------------------------------
;;; AutoCAD help tests.
- (acad_colordlg 3)			(Green is default)

