(display "reading config file")(newline)
(use-modules (ice-9 optargs))

(load (string-append (getcwd) "/reader-extensions.scm"))

(define (load-shaders)
  (load (string-append (getcwd) "/multiman.shader")))
(load-shaders)

(make-perspective-camera "playercam" (list 4 4 10) (list -0.3 -0.2 -1) (list 0 1 0) 45 1 1 500)

(set-move-factor! 1)

(let ((rc-file (string-append (getenv "HOME") "/.multimanrc")))
  (if (file-exists? rc-file)
      (load rc-file)))

