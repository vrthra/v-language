# beginings of an MOP based on tiny-clos from AMOP

# loosen the strings a little.
false [v.V singleassign$] cons java pop

[%allocate-instance
    #(class nfields)
    true swap ["Not a procedure. can not apply" throw] swap
    %mop:allocate-instance-internal
].

[%allocate-entity
    #(class nfields)
    false swap ["Tried to call an entity before proc is set." throw] swap
    %mop:allocate-instance-internal
].

# bootstrapping, these will get redefined soon.
[%mop
    [allocate-instance-internal instance? instance-class
    set-instance-class-to-self! set-instance-proc! instance-ref
    instance-set! get-instance instances]


    [-instances []].
    
    # closure id.
    [-clid 0].
    [instances -instances].
    [filter-instance
        #(id)
        # return the matching element in instances.
        # used instead of get-vector, returns a quote [id instance]
        # so that we can use size on it to check for null.
        -instances [first =] filter swap pop
    ].

    [get-instance
        filter-instance i i swap pop
    ].

    # create an 'n' element list
    [make-list
        [] swap [zero? not] [swap false swap cons swap pred] while pop
    ].

    [set-class
        [[proc lock class *rest] newcl : [proc lock newcl *rest]] view
    ].

    [set-proc
        [[proc lock class *rest] newproc : [newproc lock class *rest]] view
    ].

    [set-val
        #(lst newval idx)
        3 + [swap] dip lset
    ].

    [update-instance
        #(clid newcl)
        swap -instances [ [first =] [pop swap unit cons] if] map
        swap pop swap pop unit [-instances] swap concat .!
    ].


    [allocate-instance-internal
        #(class lock proc nfields)
        [class lock proc nfields : [[proc lock class] nfields make-list concat]] view i
        # create the next instance.
        [-clid] -clid succ unit concat .!

        -clid swap unit cons
        # add the created vector to the list of instances
        -instances cons unit [-instances] swap concat .!
    ].

    [instance?
        #(clid)
        filter-instance size zero? not
    ].
    
    [instance-class
        #(clid)
        get-instance 2 at
    ].
    
    [set-instance-class-to-self!
        #(clid)
        dup dup get-instance swap set-class update-instance
    ].

    [set-instance-proc!
        #(clid proc)
        swap dup get-instance [swap] dip swap set-proc update-instance
    ].

    [instance-ref
        #(clid idx)
        swap get-instance 3 + at
    ].

    [instance-set!
        #(clid idx newval)
        [clid idx newval : clid newval idx clid] view get-instance [nw idx lst : lst nw idx] view set-val update-instance
    ].
] module

[%allocate-instance-internal %mop:allocate-instance-internal].
[%instance? %mop:instance?].
[%instance-class %mop:instance-class].
[%set-instance-class-to-self! %mop:set-instance-class-to-self!].
[%set-instance-proc! %mop:set-instance-proc!].
[%instance-ref %mop:instance-ref].
[%instance-set! %mop:instance-set!].

