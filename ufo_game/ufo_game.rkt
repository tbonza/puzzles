;; The first three lines of this file were inserted by DrRacket. They record metadata
;; about the language level of this file in a form that our tools can easily process.
#reader(lib "htdp-beginner-reader.ss" "lang")((modname ufo_game) (read-case-sensitive #t) (teachpacks ()) (htdp-settings #(#t constructor repeating-decimal #f #t none #f () #f)))
;; A SpaceGame is a structure
;  (make-space-game Posn Number).
; interpretation (make-space-game (make-posn ux uy) tx)
; describes a configuration where the UFO is
; at (ux, uy) and the tank's x-coordinate is tx
(require 2htdp/universe)
(require 2htdp/image)

;; Constants

(define-struct origin [x y])

; Scaling factors used for movement along x axis.
(define-struct delta [left right])

(define SHIP-DIAMETER 20)
(define SHIP-RADIUS (/ SHIP-DIAMETER 2))
(define ORIGIN (make-origin 0 0))
(define WIDTH (* SHIP-DIAMETER 30))
(define HEIGHT (* SHIP-DIAMETER 30))
(define GROUND (- HEIGHT 10))
(define DY (/ SHIP-DIAMETER 20))
(define DX (/ SHIP-DIAMETER 20))
(define JUMP-RADIUS (/ SHIP-DIAMETER 4))
(define JUMP-DIRECTION 2) ; x +/- coordinate
(define KEY-SPACE " ")

;; Graphical Constants

(define BACKGROUND (empty-scene WIDTH HEIGHT))

(define UFO
  (overlay/align "middle" "top"
                 (circle SHIP-RADIUS "solid" "grey")
                 (ellipse
                  (* SHIP-DIAMETER 3) (* SHIP-DIAMETER 1.5) "solid" "orange")))

(define TANK
  (overlay/align "middle" "top"
          (rectangle (* SHIP-DIAMETER 3) SHIP-DIAMETER "solid" "olive")
          (ellipse (* SHIP-DIAMETER 2.5)
                      (* SHIP-DIAMETER 1.5) "outline" "black")))
(define MISSILE
  (overlay/offset
   (triangle (* SHIP-DIAMETER 0.5) "solid" "teal")
   0 (* SHIP-DIAMETER 0.75)
   (overlay/offset
   (rectangle (* SHIP-DIAMETER 0.5) SHIP-DIAMETER "solid" "teal")
   0 (* SHIP-DIAMETER 0.5)
   (star-polygon (* SHIP-DIAMETER 0.5)
                 (* SHIP-DIAMETER 0.25)
                 (* SHIP-DIAMETER 0.1) "solid" "pink"))))

;; Data Definitions

(define-struct ufo [x y])
; A UFO is a Posn.
; interpretation (make-posn x y) is the UFO's location
; (using the top-down, left-to-right convention)
(check-expect (ufo-x (make-ufo 3 4)) 3)
(check-expect (ufo-y (make-ufo 3 4)) 4)

(define-struct tank [loc vel])
; A Tank is a structure:
;  (make-tank Number Number).
; interpretation (make-tank x dx) specifies the position:
; (x, HEIGHT) and the tank's speed: dx pixels/tick
(check-expect (tank-loc (make-tank 3 4)) 3)

(define-struct missile [x y])
; A Missile is a Posn.
; interpretation. (make-posn x y) is the missile's place.
(check-expect (missile-y (make-missile 3 4)) 4)

; A SIGS is one of:
; - (make-aim UFO Tank)
; - (make-fired UFO Tank Missile)
; interpretation represents the complete state of a
; space invader game.
(define-struct aim [ufo tank])
(define-struct fired [ufo tank missile])

(check-expect
 (tank-loc (aim-tank (make-aim (make-ufo 1 2) (make-tank 3 4)))) 3)
(check-expect
 (ufo-x (aim-ufo (make-aim (make-ufo 1 2) (make-tank 3 4)))) 1)
(check-expect
 (tank-vel (fired-tank (make-fired (make-ufo 1 2) (make-tank 3 4)
                                   (make-missile 5 6)))) 4)

;; Game Utils

(define (sum-tail arr ans)
  (if (empty? arr) ans
      (+ (first arr) (sum-tail (rest arr) ans))))

(define (reduce-sum arr)
  (sum-tail arr 0))

(check-expect (reduce-sum (list 1 2)) 3)
(check-expect (sum-tail (list 1 2) 0) 3)

(define (map-greater-than arr x)
  (if (empty? arr) arr
      (cons (> (first arr) x)
            (map-greater-than (rest arr) x))))

(define (convert-bool x)
  (if x 1 0))

(define (map-bool-int arr)
  (if (empty? arr) arr
      (cons (convert-bool (first arr))
            (map-bool-int (rest arr)))))

(check-expect (reduce-sum (map-bool-int (list #t #f #t))) 2)
(check-expect (reduce-sum (map-bool-int (map-greater-than (list 1 2 3) 2))) 1)

; List, Integer -> Integer
; Checks the distribution of a list by converting each item to
; boolean true if the value is greater than median integer m. Sums
; the boolean values, where true is converted to integer value one,
; and returns the sum of true values.
(define (check-median arr m)
  (reduce-sum (map-bool-int (map-greater-than  arr m))))

(check-expect (check-median (list 1 2 3 4) 2) 2)

;; Rendering the game

; Tank Image -> Image 
; adds t to the given image im
(define (tank-render t im)
  (place-image TANK
   (tank-loc t) (- HEIGHT SHIP-DIAMETER)
   im))

; UFO Image -> Image 
; adds u to the given image im
(define (ufo-render u im)
  (place-image UFO
   (ufo-x u) (ufo-y u)
   im))

; Missile Image -> Image
; adds m to the given image im
(define (missile-render m im)
  (place-image MISSILE (missile-x m) (missile-y m) im))


; SIGS -> Image
; adds TANK, UFO, and possibly MISSILE to
; the BACKGROUND scene.
;
; Example scene:
;     (si-render (make-fired (make-ufo 110 100) (make-tank 150 190)
;                            (make-missile 150 150)))
(define (si-render s)
  (cond
    [(aim? s) (tank-render (aim-tank s)
                           (ufo-render (aim-ufo s) BACKGROUND))]
    [(fired? s)
     (tank-render
      (fired-tank s)
      (ufo-render (fired-ufo s)
                  (missile-render (fired-missile s)
                                  BACKGROUND)))]))

; Coordinates x1 x2 -> Float
; Euclidean distance between two points.
(define (dist x1 y1 x2 y2)
  (sqrt (+ (expt (- x2 x1) 2) (expt (- y2 y1) 2))))

; check distance works with a 3-4-5 right triangle.
(check-expect (= (dist 3 0 0 4) 5) true)

; UFO position x y -> Boolean
; Intepretation. When the distance between the UFO and the GROUND
; becomes 0, the value is true, and the game is over.
(define (ufo-lands u)
  (= (dist (ufo-x u) (ufo-y u) (ufo-x u) GROUND) 0))

(check-expect (ufo-lands (make-ufo 30 (- HEIGHT 10))) true)

; missile hits ufo
(define (missile-hits-ufo m u)
  (< (dist (missile-x m) (missile-y m)
        (ufo-x u) (ufo-y u))
     SHIP-DIAMETER))

(check-expect (missile-hits-ufo (make-missile 4 5) (make-ufo 8 10)) true)

; SIGS delta -> SIGS
; Intepretation. Increment the 'fired' state by delta.
(define (step-fired s delta)
  (make-fired
   (step-ufo (fired-ufo s))
   (step-tank (fired-tank s) (delta-left delta))
   (step-missile (fired-missile s))))

; SIGS -> SIGS
; Interprtation. Reset state of fired where missile has reached the
; maximum height to a missile which has just launched from the tank.
(define (reset-fired s delta)
  (make-fired
   (step-ufo (fired-ufo s))
   (step-tank (fired-tank s) (delta-right delta))
   (make-missile
    (tank-loc (fired-tank s))
    (delta-left (+ SHIP-DIAMETER delta)))))

; SIGS -> Boolean
; game stops if the UFO lands or if the missile hits the UFO. For both
; conditions, check the proximity of one object to the other.
(define (si-game-over s)
  (cond
    [ (aim? s) (ufo-lands (aim-ufo s)) ]
    [ (fired? s)
      (or (ufo-lands (fired-ufo s))
          (missile-hits-ufo (fired-missile s) (fired-ufo s))) ]))

(check-expect (si-game-over
               (make-aim (make-ufo 30 (- HEIGHT 10))
                         (make-tank 80 (- HEIGHT 10)))) true)

; SIGS -> Image
; render final game state when UFO lands or missile hits the UFO.
(define (si-render-final s)
  (if
   (and (fired? s)
        (missile-hits-ufo (fired-missile s) (fired-ufo s)))
      (underlay
       BACKGROUND
       (text "You win!" (* SHIP-RADIUS 3) "red"))
      (underlay
       BACKGROUND
       (text "Game Over" (* SHIP-RADIUS 3) "olive"))))
      
    

; Integer -> Integer
; Interpretation. The proposed position on the
; x-axis is either in-bounds or re-positioned inbounds.

(define (x-axis-jump-inbounds next-move)
  (cond
    [ (> next-move (- SHIP-DIAMETER)) (- WIDTH (* 1.35 SHIP-DIAMETER)) ]
    [ (< next-move (+ SHIP-DIAMETER (origin-x ORIGIN))) (origin-x ORIGIN) ]
    [ else next-move ]))

(check-expect (= (x-axis-jump-inbounds (- (* 2.0 SHIP-DIAMETER)))
                                          (origin-x ORIGIN)) #true)

; SIGS -> SIGS
; Interpretation. Move the tank by increasing or decreasing the x-axis.
(define (step-tank t d)
  (make-tank
   (+ (- (tank-loc t)
         (* (tank-vel t) (delta-left d)))
         (* (tank-vel t) (delta-right d)))
   (tank-vel t)))

(check-expect (tank-loc (step-tank (make-tank 0 1) (make-delta 1 0))) -1)
(check-expect (tank-loc (step-tank (make-tank 0 1) (make-delta 0 1))) 1)
(check-expect (tank-loc (step-tank (make-tank 0 2) (make-delta 1 0))) -2)

; SIGS -> SIGS
; Move the missile, if it exists, by increasing the y-axis. 
(define (step-missile m)
      (make-missile (missile-x m) (- (missile-y m) DY)))

(check-expect (missile-y (step-missile (make-missile 0 0))) (* -1 DY))
(check-expect (missile-y (step-missile (make-missile 0 1))) (* -1 (- DY 1)))

; Number -> Number
(define (jump-move x step-size) (+ x step-size))

(define (jump-direction x)
  (if (= x 0) -1 1))

(define (jump-ufo x step-size)
  (x-axis-jump-inbounds (jump-move x step-size)))

;(check-expect (= (jump-ufo (origin-x ORIGIN) -5) (origin-x ORIGIN)) #true)
(check-random (jump-direction (random 2))
              (if (= (random 2) 0) -1 1))

; SIGS -> SIGS
; The UFO will move linearly down the y-axis but also
; jump randomly left or right along the x-axis.
(define (step-ufo u)
  (make-ufo (ufo-x u)
   ;(jump-ufo (ufo-x u) delta)
   (+ (ufo-y u) DY)))

; SIGS Number -> SIGS
; moves the space-invader objects predictably by delta
(define (si-move-proper s d)
  (cond
    [ (aim? s) (make-aim 
                 (step-ufo (aim-ufo s)) 
                 (step-tank (aim-tank s) d)) ]
    [ (fired? s) (make-fired 
                   (step-ufo (fired-ufo s))
                   (step-tank (fired-tank s) d)
                   (step-missile (fired-missile s))) ]))

; SIGS -> SIGS
; Interpretation. Routes movement of objects contained
; within the game state.
(define (si-move w delta)
  (si-move-proper w delta))

(define (si-update-tick s)
  (si-move s (make-delta 0 0)))

; SIGS -> SIGS
; Interpretation. Routes movement of objects
(define (si-update-delta s d)
  (si-move s (make-delta
              (delta-left d)
              (delta-right d))))
  
; SIGS KeyEvent -> SIGS
; Interpretation. Updates the SIGS given a key event of
; left arrow to move the tank left, right arrow to move
; the tank right, and space bar to fire the missile if it
; hasn't been launched yet.
;  delta [ufo tank missile]
(define (si-control s ke)
  (cond
   [(key=? ke "left") ; move tank left.
     (si-update-delta s (make-delta 1 0))]
    [(key=? ke "right") ; move tank right.
     (si-update-delta s (make-delta 0 1))] 
    [(key=? ke KEY-SPACE) ; launch missile.
     (si-update-delta
      (make-fired
       (make-ufo (ufo-x (aim-ufo s)) (ufo-y (aim-ufo s)))
       (make-tank (tank-loc (aim-tank s)) (tank-vel (aim-tank s)))
       (make-missile (tank-loc (aim-tank s)) (- WIDTH (* SHIP-DIAMETER 5))))
      (make-delta 0 0))]
    [else s]))

; SIGS KeyEvent -> SIGS
; Interpretation. Manage state given keyboard updates.
; start game ->

;; Game Constants
(define WORLD-TEST-INITIALIZE-STATE-AIM
  (make-aim (make-ufo (* SHIP-DIAMETER 2.5)
                      (+ (origin-y ORIGIN) SHIP-DIAMETER))
            (make-tank (* SHIP-DIAMETER 2.5)
                       (- HEIGHT SHIP-DIAMETER))))

(define WORLD-TEST-INITIALIZE-STATE-FIRED
  (make-fired (make-ufo (* SHIP-DIAMETER 2.5)
                      (+ (origin-y ORIGIN) SHIP-DIAMETER))
            (make-tank (* SHIP-DIAMETER 2.5)
                       (- HEIGHT SHIP-DIAMETER))
            (make-missile (* SHIP-DIAMETER 2.8)
                          (- HEIGHT SHIP-DIAMETER))))

(define (initialize-world-x-axis v)
  (floor (+ (/ v 2) (random (- WIDTH (* SHIP-DIAMETER v))))))

(define WORLD-INITIALIZE
  (make-aim (make-ufo (initialize-world-x-axis 5)
                      (+ (origin-y ORIGIN) SHIP-DIAMETER))
            (make-tank (initialize-world-x-axis 5)
                       2.5)))

;; Main function
(define (main-simulation s)
  (big-bang s
    [to-draw si-render]
    [on-tick si-update-tick]
    [stop-when si-game-over si-render-final]))

(define (main s)
  (big-bang s
    [to-draw si-render]
    [on-tick si-update-tick]
    [on-key si-control]
    [stop-when si-game-over si-render-final]))
