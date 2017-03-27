(define (domain joint_bar)
(:requirements :strips :equality :typing :adl :conditional-effects)

(:types link angle)

(:predicates
	(angle-ord ?a - angle ?a1 - angle)
	(has-angle ?l - angle ?j - link)
	(affected ?l1 - link ?l2 - link)
    (is-child-of ?l1 - link ?l2 - link)
)

(:action increase_angle
:parameters (?child_link - link ?a1 ?a2 - angle)
:precondition (and 
	(has-angle ?a1 ?child_link)
	(angle-ord ?a1 ?a2)
	)
:effect 
    (and 
	    (not (has-angle ?a1 ?child_link)) 
	    (has-angle ?a2 ?child_link)
	    (forall (?ls - link ?a3 ?a4 - angle)
		    (when (and (affected ?ls ?child_link) (has-angle ?a3 ?ls) (angle-ord ?a3 ?a4) )
		        (and
			        (not (has-angle ?a3 ?ls))
			        (has-angle ?a4 ?ls)
		        )
		    )
	    )
    )
)

(:action decrease_angle
:parameters (?child_link - link ?a2 ?a1 - angle)
:precondition (and 
	(has-angle ?a2 ?child_link)
	(angle-ord ?a1 ?a2)
	)
:effect 
    (and 
	    (not (has-angle ?a2 ?child_link)) 
	    (has-angle ?a1 ?child_link)
	    (forall (?ls - link ?a3 ?a4 - angle)
		    (when (and (affected ?ls ?child_link) (has-angle ?a4 ?ls) (angle-ord ?a3 ?a4) )
		        (and
			        (not (has-angle ?a4 ?ls))
			        (has-angle ?a3 ?ls)
		        )
		    )
	    )
    )
)
)