# definition of show.
[debug? true].
[show
    [q] let
    debug?
    [ q [put 1] map "" puts ]
    if].

[test
    [expected fn msg] let
    expected put ' ' put
    fn put ' : ' put
    fn i
    expected i not [msg throw] if
    msg put ' success' puts].

#=========================================
'Starting tests...' puts
'=========================================' puts
# basic stack operations
[333 =] [111  222 +] ' simple addition ' test
[21 =] [1  2  +  3  4  +  *] ' operations on stack (1)' test
[true =] [2  2  +  2  2  *  =] ' operations on stack (2)' test
[true =] [6  6  *  5  7  *  >] ' operations on stack (3)' test
[false =] [true  false  or  true  and  not] 'boolean operations' test
#=========================================
# list operations.
[[5 4 3 2 1] =] [[1 2 3 4 5] rev] 'list reverse' test
[[peter paul mary jane] =] [[peter paul]  [mary jane]  concat] 'list reverse' test

#=========================================
[fact 
    zero?
        [pop 1]
        [dup 1 - fact *]
    ifte].

[120 =] [5 fact] '(ifte) fact ' test
#=========================================
[gfact
    [null?]
    [succ]
    [dup pred]
    [i *]
    genrec].

[120 =] [5 gfact] '(genrec) gfact ' test
#=========================================
[lfact
    [null?]
    [succ]
    [dup pred]
    [*]
    linrec].

[120 =] [5 lfact] '(linrec) lfact ' test
#=========================================
[t-last
    [rest& null?]
    [first]
    [rest]
    tailrec].

[5 = swap 0 = and] [0 [3 2 1 5] t-last] '(tailrec) t-last' test
#=========================================
[pfact
    [1]
    [*]
    primrec].

[120 =] [5 pfact] '(primrec) pfact ' test
#=========================================
[area
  [pi 3.1415].
  [sq dup *].
  sq pi *].

[28.2735 =] [3 area] '(internal def) area ' test
#=========================================
[fib 
    [small?]
    [] 
    [pred dup pred]
    [+]
    binrec].

[8 =] [6 fib] '(binrec) fib' test
#=========================================

[qsort
    [small?]
    []
    [uncons [>] split&]
    [[swap] dip cons concat]
    binrec].

[[0 2 4 6 6 7 8 9] =] [[0 9 6 7 8 4 6 2] qsort] '(binrec) fib' test
#=========================================


[roots
    # define our parameters (In classical concatanative languages, the internal
    # definitions are not used, but it makes our lives easier).
    [a b c] let

    [<< swap cons].
    [! [unit i] map].

    [" a: " a " b: " b " c: " c]! show
    
    [discr b dup * 4 a * c * - sqrt].
    [] " discr: " << discr << rev show

    [root1 0 b - discr + 2 a * /].
    [root2 0 b - discr - 2 a * /].

    [] " root1: " << root1 << " root2: " << root2 << rev show
    root1 root2].

[-5.0 = swap 3.0 = and] [2 4 -30 roots] 'roots' test
#=========================================

[cmdthrows
        [dup puts 'false shield' puts false] shield
        'hi there throw' throw].
[mycmd
        [dup puts 'true shield' puts true] shield
        cmdthrows].
mycmd

"--------Success-----------" puts
