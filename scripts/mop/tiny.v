# beginings of an MOP based on tiny-clos from AMOP

[ %allocate-instance
    #(class nfields)
    true swap ["Not a procedure. can not apply" throw] swap
    %allocate-instance-internal
].

