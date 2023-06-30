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
    (= (on_robot R2D2) 0)
    (= (assignment_at r1) 1)
    (= (assignment_at r2) 1)
    (= (assignment_at r3) 1)
    (= (assignment_at r4) 1)

 
)
(:goal 
     (and (= (on_robot R2D2) 2)
          (robot_in R2D2 r5))
)
(:metric minimize (act-cost) )
)


