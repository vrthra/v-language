[show dup puts].
'hello' puts
[fact 
    zero?
        [pop 1]
        [dup 1 - fact *]
    ifte].

'5 fact(120):' put
5 fact show
[120 !=] ['fact failed' throw ] if
pop

[area
  [pi 3.1415].
  [sq dup *].
  sq pi *].

'3 area(28.2735):' put
3 area show
[28.2735 !=] ['area failed' throw ] if
pop

[fib 
    [small?]
    [] 
    [pred dup pred]
    [+]
    binrec].
'6 fib(8):' put
6 fib show
[8 !=] ['fib failed' throw ] if
pop

[root
    # define our parameters (In classical concatanative languages, the internal
    # definitions are not used, but it makes our lives easier).
    [a b c] let

    "a:" put a put " b:" put b put " c:" put c puts
    # define the discriminent
    [discr b dup * 4 a * c * - sqrt].
    "D:" put discr puts

    # and fetch the roots.
    [root1 0 b - discr + 2 a * /].
    [root2 0 b - discr - 2 a * /].

    # output results
    "root1 :" put root1 show
    "root2 :" put root2 show
    root1
    root2
].

#Usage 
2 4 -30 root
[-5.0 !=] ['root1 failed' throw ] if
pop
[3.0 !=] ['root2 failed' throw ] if
pop

[cmdthrows
        [dup puts 'false shield' puts false] shield
        'hi there throw' throw
].
[mycmd
        [dup puts 'true shield' puts true] shield
        cmdthrows
].
mycmd

"--------Success-----------" puts
