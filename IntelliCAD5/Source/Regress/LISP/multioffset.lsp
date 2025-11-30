(prompt "\nType multioffset to run....")
(defun c:multioffset ( / off sel1 side n lng)
(setq off (getdist "\nOffset Distance : ")
sel1 (ssget)
side (getpoint "\nSelect Side to Offset: ")
n 0
lng (sslength sel1))
(repeat lng
(setq ent (ssname sel1 n))
(command "OFFSET" off ent side "")
(setq n (1+ n))
);repeat
(princ)
);defun
(princ)