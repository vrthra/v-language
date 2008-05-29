[test
    [expected fn msg] let
    #push in a sentry.
    'sentry'
    expected put ' ' put fn put ' - ' put
    fn i
    expected i not [msg throw] if
    #check the sentry to ensure that the stack is not messed.
    'sentry' != [?? 'incorrect stack' throw] if
    msg put ' :success' puts].

[scripts/mop/tiny] use

'class1' 5 %allocate-instance
[[
    [1 [[Not a procedure. can not apply throw] true class1 false false false false false]
]] =] [%mop:instances] '%allocate-instance' test

'class2' 5 %allocate-instance
[[
    [2 [[Not a procedure. can not apply throw] true class2 false false false false false]]
    [1 [[Not a procedure. can not apply throw] true class1 false false false false false]]
] =] [%mop:instances] '%mop:instances - alloc 2' test

'class3' 5 %allocate-entity
[[
    [3 [[Tried to call an entity before proc is set. throw] false class3 false false false false false]]
    [2 [[Not a procedure. can not apply throw] true class2 false false false false false]]
    [1 [[Not a procedure. can not apply throw] true class1 false false false false false]]
] =] [%mop:instances] '%allocate-entity' test

1 %mop:set-instance-class-to-self!
[[
    [3 [[Tried to call an entity before proc is set. throw] false class3 false false false false false]]
    [2 [[Not a procedure. can not apply throw] true class2 false false false false false]]
    [1 [[Not a procedure. can not apply throw] true 1 false false false false false]]
] =] [%mop:instances] '%mop:set-instance-class-to-self' test

[
    [[Not a procedure. can not apply throw] true 1 false false false false false]
=] [1 %mop:get-instance] '%mop:get-instance' test

[ true =] [2 %mop:instance?] '%mop:instance? - 1' test
[ false =] [4 %mop:instance?] '%mop:instance? - 2' test


2 [new proc] %mop:set-instance-proc!
[[
    [3 [[Tried to call an entity before proc is set. throw] false class3 false false false false false]]
    [2 [[new proc] true class2 false false false false false]]
    [1 [[Not a procedure. can not apply throw] true 1 false false false false false]]
] =] [%mop:instances] '%mop:set-instance-proc!' test

3 2 'new val' %mop:instance-set!
[[
    [3 [[Tried to call an entity before proc is set. throw] false class3 false false 'new val' false false]]
    [2 [[new proc] true class2 false false false false false]]
    [1 [[Not a procedure. can not apply throw] true 1 false false false false false]]
] ?? =] [%mop:instances] '%mop:instance-set!' test

"_____________________
Success
_____________________" puts
??
