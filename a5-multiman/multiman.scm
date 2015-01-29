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
(texture-from-file "terrain_1" "Grass0100_7_S.png" 'tex2d #t)
(texture-from-file "terrain_2" "ConcreteRough0009_2_S.png" 'tex2d #t)
(texture-from-file "terrain_3" "terrain_3.png" 'tex2d #t)
(texture-from-file "selection_circle" "selection_circle.png" 'tex2d #t)
(texture-from-file "slidebar2" "slidebar_2.png" 'tex2d #t)
(texture-from-file "dorf" "Menu/dorf.png" 'tex2d #t)
(texture-from-file "none" "exchange_1.png" 'tex2d #t)
(texture-from-file "alpha_mask_1" "alpha_mask_2.png" 'tex2d #t)

(texture-from-file "boom" "boom.png" 'tex2d #t)

(texture-from-file "game_won" "Menu/game_won.png" 'tex2d #t)
(texture-from-file "game_loose" "Menu/game_loose.png" 'tex2d #t)
(texture-from-file "game_paused" "Menu/game_paused.png" 'tex2d #t)
(texture-from-file "game_start" "Menu/game_start.png" 'tex2d #t)
(texture-from-file "black_screen" "Menu/black_screen.png" 'tex2d #t)

(texture-from-file "plant_1" "./render-data/models/blends/textures/plants/NaturePlants0053_1_S.png" 'tex2d #t)
(texture-from-file "plant_2" "./render-data/models/blends/textures/plants/NaturePlants0049_1_S.png" 'tex2d #t)
(texture-from-file "plant_3" "./render-data/models/blends/textures/plants/NaturePlants0036_1_S.png" 'tex2d #t)
(texture-from-file "plant_4" "./render-data/models/blends/textures/plants/NaturePlants0029_1_S.png" 'tex2d #t)
(texture-from-file "plant_5" "./render-data/models/blends/textures/plants/NaturePlants0028_1_S.png" 'tex2d #t)
(texture-from-file "plant_6" "./render-data/models/blends/textures/plants/NaturePlants0026_1_S.png" 'tex2d #t)
(texture-from-file "plant_7" "./render-data/models/blends/textures/plants/NaturePlants0023_1_S.png" 'tex2d #t)
(texture-from-file "plant_8" "./render-data/models/blends/textures/plants/NaturePlants0022_1_S.png" 'tex2d #t)
(texture-from-file "plant_9" "./render-data/models/blends/textures/plants/NaturePlants0021_1_S.png" 'tex2d #t)
(texture-from-file "plant_10" "./render-data/models/blends/textures/plants/NaturePlants0019_1_S.png" 'tex2d #t)
(texture-from-file "plant_11" "./render-data/models/blends/textures/plants/NaturePlants0014_1_S.png" 'tex2d #t)
(texture-from-file "plant_12" "./render-data/models/blends/textures/plants/NaturePlants0005_1_S.png" 'tex2d #t)
(texture-from-file "plant_13" "./render-data/models/blends/textures/plants/NaturePlants0003_1_S.png" 'tex2d #t)

(texture-from-file "interface_bbm" "Menu/InterfaceBomberman.png" 'tex2d #t)
(texture-from-file "interface_bbm_menu" "Menu/InterfaceBomberman_menu.png" 'tex2d #t)
(texture-from-file "interface_pm" "Menu/InterfacePacman.png" 'tex2d #t)
(texture-from-file "interface_pm_menu" "Menu/InterfacePacman_menu.png" 'tex2d #t)
(texture-from-file "nu_b_s2" "Menu/noupgrade_button_settlement2.png" 'tex2d #t)
(texture-from-file "nu_b_s3" "Menu/noupgrade_button_settlement3.png" 'tex2d #t)
(texture-from-file "nu_b_sm" "Menu/upgrade_button_settlement_max.png" 'tex2d #t)
(texture-from-file "u_b_s2" "Menu/upgrade_button_settlement2.png" 'tex2d #t)
(texture-from-file "u_b_s3" "Menu/upgrade_button_settlement3.png" 'tex2d #t)
(texture-from-file "nu_b_t1" "Menu/noupgrade_button_turret1.png" 'tex2d #t)
(texture-from-file "nu_b_t2" "Menu/noupgrade_button_turret2.png" 'tex2d #t)
(texture-from-file "nu_b_tm" "Menu/upgrade_button_turret_max.png" 'tex2d #t)
(texture-from-file "u_b_t1" "Menu/upgrade_button_turret1.png" 'tex2d #t)
(texture-from-file "u_b_t2" "Menu/upgrade_button_turret2.png" 'tex2d #t)
(texture-from-file "pacman_defence" "Menu/pacman_defence_2.png" 'tex2d #t)
(texture-from-file "pacman_units" "Menu/pacman_units_2.png" 'tex2d #t)
(texture-from-file "pacman_unit_production" "Menu/pacman_production_2.png" 'tex2d #t)
(texture-from-file "bbm_defence" "Menu/bbm_defence.png" 'tex2d #t)
(texture-from-file "bbm_units" "Menu/bbm_units.png" 'tex2d #t)
(texture-from-file "bbm_unit_production" "Menu/bbm_production.png" 'tex2d #t)
(texture-from-file "menu" "Menu/menu_button.png" 'tex2d #t)
(texture-from-file "main" "Menu/main.png" 'tex2d #t)
(texture-from-file "exit" "Menu/exit.png" 'tex2d #t)
(texture-from-file "pause" "Menu/pause.png" 'tex2d #t)
(texture-from-file "resume" "Menu/resume.png" 'tex2d #t)
(texture-from-file "exit_game" "Menu/exit_game.png" 'tex2d #t)
(texture-from-file "yes" "Menu/yes.png" 'tex2d #t)
(texture-from-file "no" "Menu/no.png" 'tex2d #t)

(texture-from-file "load_screen_0" "Menu/load_screen_0.png" 'tex2d #t)
(texture-from-file "load_screen_1" "Menu/load_screen_1.png" 'tex2d #t)
(texture-from-file "load_screen_2" "Menu/load_screen_2.png" 'tex2d #t)
(texture-from-file "load_screen_3" "Menu/load_screen_3.png" 'tex2d #t)
(texture-from-file "load_screen_4" "Menu/load_screen_4.png" 'tex2d #t)
(texture-from-file "load_screen_5" "Menu/load_screen_5.png" 'tex2d #t)

(texture-from-file "test" "explosions-04.png" 'tex2d #t)


(set-move-factor! 1)

(let ((rc-file (string-append (getenv "HOME") "/.multimanrc")))
  (if (file-exists? rc-file)
      (load rc-file)))

