(define (problem prob1)
(:domain localization)
(:objects
     r0 r1 r2 r3 r4 r5 - region
     R2D2 - robot
)
(:init
    (robot_in R2D2 r0)

    (= (act-cost) 0)
    (= (dummy) 0)

 
)
(:goal 
     (and (and (visited r1)  (visited r2))
          (robot_in R2D2 r5))
)
(:metric minimize (act-cost) )
)


