(define (problem joint_bar_task)
(:domain joint_bar)
(:objects
    angle0 angle90 angle180 angle270 - angle
    link1 link2 link3 - link
)
(:init
    (affected link2 link1)
    (affected link3 link1)
    (affected link3 link2)
    (angle-ord angle0 angle90)
    (angle-ord angle90 angle180)
    (angle-ord angle180 angle270)
    (angle-ord angle270 angle0)
    (has-angle link1 angle0)
    (has-angle link2 angle0)
    (has-angle link3 angle0)
    (is-child-of link2 link1)
    (is-child-of link3 link2)
)
(:goal (and
    (has-angle link2 angle90)
    (has-angle link3 angle180)
)))
