# beginings of an MOP based on tiny-clos from AMOP

# loosen the strings a little.
false [v.V singleassign$] cons java

# create an 'n' element list
[make-list
    [] swap [zero? not] [swap true swap cons swap pred] while pop
].

[%allocate-instance
    #(class nfields)
    true swap ["Not a procedure. can not apply" throw] swap
    %allocate-instance-internal
].

[%allocate-entity
    #(class nfields)
    false swap ["Tried to call an entity before proc is set." throw] swap
    %allocate-instance-internal
].

# bootstrapping, these will get redefined soon.

[%allocate-instance-internal []].
[%instance? []].
[%instance-class []].
[%set-instance-class-to-self []].

[%set-instance-proc []].
[%instance-ref []].
[%instance-set! []].

# create a closure for our MOP internal objects
[
    [instances []].
    [%instance
        #(closure)
        # return the matching element in instances.
    ].

    [%allocate-instance-internal
        #(class lock proc nfields)
        [class lock proc nfields] let
        [vector nfields 3 + make-list].
    ].
] i
