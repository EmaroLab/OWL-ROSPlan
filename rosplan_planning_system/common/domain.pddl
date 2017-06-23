(define (domain joint_bar)
(:requirements :strips :equality :typing :adl :conditional-effects)

(:types link angle)

(:predicates
	(angle-ord ?a - angle ?a1 - angle)
	(has-angle ?l - link ?a - angle)
	(affected ?l1 - link ?l2 - link)
    (is-child-of ?l1 - link ?l2 - link)
)

(:action increase_angle
:parameters (?child_link - link ?a1 ?a2 - angle)
:precondition (and 
	(has-angle ?child_link ?a1 )
	(angle-ord ?a1 ?a2)
	)
:effect 
    (and 
	    (not (has-angle ?child_link ?a1)) 
	    (has-angle ?child_link ?a2)
	    (forall (?ls - link ?a3 ?a4 - angle)
		    (when (and (affected ?ls ?child_link) (has-angle ?ls ?a3) (angle-ord ?a3 ?a4) )
		        (and
			        (not (has-angle ?ls ?a3))
			        (has-angle ?ls ?a4)
		        )
		    )
	    )
    )
)

(:action decrease_angle
:parameters (?child_link - link ?a2 ?a1 - angle)
:precondition (and 
	(has-angle ?child_link ?a2)
	(angle-ord ?a1 ?a2)
	)
:effect 
    (and 
	    (not (has-angle ?child_link ?a2)) 
	    (has-angle ?child_link ?a1)
	    (forall (?ls - link ?a3 ?a4 - angle)
		    (when (and (affected ?ls ?child_link) (has-angle ?ls ?a4) (angle-ord ?a3 ?a4) )
		        (and
			        (not (has-angle ?ls ?a4))
			        (has-angle ?ls ?a3)
		        )
		    )
	    )
    )
)
)
