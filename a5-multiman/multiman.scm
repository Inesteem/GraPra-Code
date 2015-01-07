(display "reading config file")(newline)
(use-modules (ice-9 optargs))

(load (string-append (getcwd) "/reader-extensions.scm"))

(define (load-shaders)
  (load (string-append (getcwd) "/terrain.shader")))
(load-shaders)

(make-perspective-camera "playercam" (list 4 4 10) (list -0.3 -0.2 -1) (list 0 1 0) 45 1 1 500)



(texture-from-file "terrain_hm" "eire.png" 'tex2d #t)
(texture-from-file "terrain_0" "terrain_0_water.png" 'tex2d #t)
(texture-from-file "terrain_1" "terrain_1.png" 'tex2d #t)
(texture-from-file "terrain_2" "terrain_2.png" 'tex2d #t)
(texture-from-file "terrain_3" "terrain_3.png" 'tex2d #t)

(set-move-factor! 1)

(let ((rc-file (string-append (getenv "HOME") "/.terrainrc")))
  (if (file-exists? rc-file)
      (load rc-file)))

