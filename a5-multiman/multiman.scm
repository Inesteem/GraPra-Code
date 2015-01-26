(display "reading config file")(newline)
(use-modules (ice-9 optargs))

(load (string-append (getcwd) "/reader-extensions.scm"))

(define (load-shaders)
  (load (string-append (getcwd) "/multiman.shader")))
(load-shaders)

;(make-perspective-camera "playercam" (list 4 4 10) (list -0.3 -0.2 -1) (list 0 1 0) 45 1 1 500)
;(make-perspective-camera "ufocam" (list 0 10 995) (list 1 -0.1 1) (list 0 1 0) 45 1 1 3000)
;(make-orthographic-camera "lightcam" (list 0 10 995) (list 0 -1 0) (list 1 0 0) 10 -10 10 -10 10 5000)

(define cam (make-perspective-camera "cam" (list 0.561 0.687 0.386) (list -0.045 -0.993 0.105) (list 0.998 -0.043 0.024) 45 1 0.01 10))
(define playercam (make-perspective-camera "playercam" (list 16 20 16) (list 0 -0.9 0.4) (list 0 0.4 0.9) 45 1 1 100))
(define orthocam (make-orthographic-camera "orthocam" (list  0.0 0.0 0.0) (list 0.0 0.0 -1.0) (list 0.0 1.0 0.0) 50 0 50 0 0.01 1000))

(use-camera playercam)


(texture-from-file "terrain_hm" "eire.png" 'tex2d #t)
(texture-from-file "terrain_0" "terrain_0_water.png" 'tex2d #t)
(texture-from-file "terrain_1" "terrain_1.png" 'tex2d #t)
(texture-from-file "terrain_2" "terrain_2.png" 'tex2d #t)
(texture-from-file "terrain_3" "terrain_3.png" 'tex2d #t)
(texture-from-file "selection_circle" "selection_circle.png" 'tex2d #t)
(texture-from-file "slidebar2" "slidebar_2.png" 'tex2d #t)
(texture-from-file "dorf" "Menu/dorf.png" 'tex2d #t)
(texture-from-file "none" "exchange_1.png" 'tex2d #t)
(texture-from-file "alpha_mask_1" "alpha_mask_2.png" 'tex2d #t)

(texture-from-file "game_won" "Menu/game_won.png" 'tex2d #t)
(texture-from-file "game_loose" "Menu/game_loose.png" 'tex2d #t)
(texture-from-file "game_paused" "Menu/game_paused.png" 'tex2d #t)
(texture-from-file "game_start" "Menu/game_start.png" 'tex2d #t)
(texture-from-file "black_screen" "Menu/black_screen.png" 'tex2d #t)

(texture-from-file "pacman" "pacman.png" 'tex2d #t)
(texture-from-file "iconbar" "Menu/bar_2.png" 'tex2d #t)
(texture-from-file "nu_b_s2" "Menu/noupgrade_button_settlement2.png" 'tex2d #t)
(texture-from-file "nu_b_s3" "Menu/noupgrade_button_settlement3.png" 'tex2d #t)
(texture-from-file "u_b_s2" "Menu/upgrade_button_settlement2.png" 'tex2d #t)
(texture-from-file "u_b_s3" "Menu/upgrade_button_settlement3.png" 'tex2d #t)
(texture-from-file "nu_b_t1" "Menu/noupgrade_button_turret1.png" 'tex2d #t)
(texture-from-file "nu_b_t2" "Menu/noupgrade_button_turret2.png" 'tex2d #t)
(texture-from-file "u_b_t1" "Menu/upgrade_button_turret1.png" 'tex2d #t)
(texture-from-file "u_b_t2" "Menu/upgrade_button_turret2.png" 'tex2d #t)
(texture-from-file "pacman_defence" "Menu/pacman_defence_2.png" 'tex2d #t)
(texture-from-file "pacman_units" "Menu/pacman_units_2.png" 'tex2d #t)
(texture-from-file "pacman_unit_production" "Menu/pacman_production_2.png" 'tex2d #t)

(texture-from-file "test" "explosions-04.png" 'tex2d #t)


(set-move-factor! 1)

(let ((rc-file (string-append (getenv "HOME") "/.multimanrc")))
  (if (file-exists? rc-file)
      (load rc-file)))

