(define (domain joint_bar)
(:requirements :strips :equality :typing :adl :conditional-effects)

(:types link joint angle)

(:predicates
	(connected ?j - joint ?l - link) ;; considero solo il fatto che un link sia connesso ad un joint, ignoro la direzione.
	(angle-before ?a - angle ?a1 - angle)
	(angle-joint ?a - angle ?j - joint)
	(fixed ?j - joint)
	(affected ?j -joint ?l - link ?j1 - joint) ;; nel momento in cui uso un link come child, per ruotare il joint ?j1, tutti i joint successivi cambiano angolo.
)

(:action increase_angle
:parameters (?link1 ?link2 - link ?joint - joint ?a1 ?a2 - angle)
:precondition (and 
	(connected ?joint ?link1)
	(connected ?joint  ?link2)
	(not (= ?link1 ?link2))
	(angle-joint ?a1 ?joint)
	(angle-before ?a1 ?a2)
	)
:effect 
    (and 
	    (not (angle-joint ?a1 ?joint)) 
	    (angle-joint ?a2 ?joint)
	    (forall (?js - joint ?a3 ?a4 - angle )
		    (when (and (affected ?js ?link1 ?joint) (not (= ?js ?joint)) (angle-joint ?a3 ?js) (angle-before ?a3 ?a4) )
		        (and
			        (not (angle-joint ?a3 ?js))
			        (angle-joint ?a4 ?js)
		        )
		    )
	    )
    )
)


(:action decrease_angle
:parameters (?link1 ?link2 - link ?joint - joint ?a1 ?a2 - angle)
:precondition (and 
	(connected ?joint ?link1)
	(connected ?joint  ?link2)
	(not (= ?link1 ?link2))
	(angle-joint ?a1 ?joint)
	(angle-before ?a2 ?a1)
	)
:effect 
    (and 
	    (not (angle-joint ?a1 ?joint)) 
	    (angle-joint ?a2 ?joint)
	    (forall (?js - joint ?a3 ?a4 - angle )
		    (when (and (affected ?js ?link1 ?joint) (not (= ?js ?joint)) (angle-joint ?a3 ?js) (angle-before ?a4 ?a3) )
		        (and
			        (not (angle-joint ?a3 ?js))
			        (angle-joint ?a4 ?js)
		        )
		    )
	    )
    )
)


)
