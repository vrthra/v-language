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
    [[1 <class>] [[Not a procedure. can not apply throw] true class1 false false false false false]
]] =] [%mop:instances] '%allocate-instance' test

'class2' 5 %allocate-instance
[[
    [[2 <class>] [[Not a procedure. can not apply throw] true class2 false false false false false]]
    [[1 <class>] [[Not a procedure. can not apply throw] true class1 false false false false false]]
] =] [%mop:instances] '%allocate-instance - alloc 2' test

'class3' 5 %allocate-entity
[[
    [[3 <class>] [[Tried to call an entity before proc is set. throw] false class3 false false false false false]]
    [[2 <class>] [[Not a procedure. can not apply throw] true class2 false false false false false]]
    [[1 <class>] [[Not a procedure. can not apply throw] true class1 false false false false false]]
] =] [%mop:instances] '%allocate-entity' test

[1 <class>] %set-instance-class-to-self!
[[
    [[3 <class>] [[Tried to call an entity before proc is set. throw] false class3 false false false false false]]
    [[2 <class>] [[Not a procedure. can not apply throw] true class2 false false false false false]]
    [[1 <class>] [[Not a procedure. can not apply throw] true [1 <class>] false false false false false]]
] =] [%mop:instances] '%set-instance-class-to-self' test

[
    [[Not a procedure. can not apply throw] true [1 <class>] false false false false false]
=] [[1 <class>] %mop:get-instance] '%mop:get-instance' test

[ true =] [[2 <class>] %instance?] '%instance? - 1' test
[ false =] [[4 <class>] %instance?] '%instance? - 2' test


[2 <class>] [new proc] %set-instance-proc!
[[
    [[3 <class>] [[Tried to call an entity before proc is set. throw] false class3 false false false false false]]
    [[2 <class>] [[new proc] true class2 false false false false false]]
    [[1 <class>] [[Not a procedure. can not apply throw] true [1 <class>] false false false false false]]
] =] [%mop:instances] '%set-instance-proc!' test

[3 <class>] 2 'new val' %instance-set!
[[
    [[3 <class>] [[Tried to call an entity before proc is set. throw] false class3 false false 'new val' false false]]
    [[2 <class>] [[new proc] true class2 false false false false false]]
    [[1 <class>] [[Not a procedure. can not apply throw] true [1 <class>] false false false false false]]
] ?? =] [%mop:instances] '%instance-set!' test

['<list>' =] [[1 2 3] class-of] 'class-of <list>' test
['<integer>' =] [1 class-of] 'class-of <int>' test
['class2' =] [[2 <class>] class-of] 'class-of? class2' test
[[1 <class>] =] [[1 <class>] class-of] 'class-of? <class>' test

"_____________________
Success
_____________________" puts
??
