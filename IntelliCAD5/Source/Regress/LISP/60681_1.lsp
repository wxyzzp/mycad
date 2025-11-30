(defun c:60681()
(setq hnd (assoc '5 (entget(entlast))))
(if (= clr60681 nil)(setq clr60681 1))
(command "dimclrd" clr60681 
	"dim" "update" "last" "" 
	"exit")
(setq clr60681 (+ clr60681 1))
(setq hndnew (assoc '5 (entget(entlast))))
(if (equal hnd hndnew) (princ "OK") (entsel))
)
