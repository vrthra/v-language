#==============================================================================
# Unit tests for V
#==============================================================================

[debug? false].

#==============================================================================
# A simple helper procedure. It prints out its argument list only on debug.
#==============================================================================
[show
    debug?
        [[put] step "" puts]
        [pop]
    ifte].
#==============================================================================
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

#==============================================================================
#The tests have this format.
#There are two quotes, one the result and the second the test quote.
#
#First the test quote is evaluated, this will give an answer in the stack. Then the result
#quote is executed, and it should leave a value true in the stack.
#ie:
#-- result verification quote. (This checks if the value currently in the stack is equal to '2')
#[
#    2 =
#]
#-- test quote. this is executed first, and then the result verification quote is executed.
#[
#    1 1 +
#] 'message' test
#-- the 'message' is printed along with success or failure of the test.

#==============================================================================
"stack shuffle tests" puts
[[2 1] =] [1 2 swap unit cons] 'swap' test
[[1 1] =] [1 dup unit cons] 'dup' test
[1 =] [1 2 pop] 'pop' test
[2 =] [1 2 popd] 'popd' test
[[2 1 3] =] [1 2 3 swapd unit cons cons] 'swapd' test
[[1 1 2] =] [1 2 dupd unit cons cons] 'dupd' test
#==============================================================================
"comparison tests" puts
[not] [0 0 >] 'false' test 
[id] [1 0 >] 'true' test
[not] [0 1 >] '>' test
[not] [1 1 >] '>' test
[not] [0 0 <] '<' test
[not] [1 0 <] '<' test
[id] [0 1 <] '<' test
[not] [1 1 <] '<' test
[not] [0 0 >] '>' test
[id] [1 0 >=] '>=' test
[not] [0 1 >=] '>=' test
[id] [1 1 >=] '>=' test
[id] [0 0 <=] '<=' test
[not] [1 0 <=] '<=' test
[id] [0 1 <=] '<=' test
[id] [1 1 <=] '<=' test
[id] [1 1 ==] '==' test
[not] [1 1 !=] '!=' test
[id] [0 0 ==] '==' test 
[not] [0 0 !=] '!=' test
#==============================================================================
"boolean tests" puts
[not] [true not] 'not' test
[id] [false not] 'not' test
[id] [true true and] 'and' test
[id] [true true or] 'or' test
[not] [true true xor] 'xor' test
[not] [true false and] 'and' test
[id] [true false or] 'or' test
[id] [true false xor] 'xor' test
[not] [false true and] 'and' test
[id] [false true or] 'or' test
[id] [false true xor] 'xor' test
[not] [false false and] 'and' test
[not] [false false or] 'or' test
[not] [false false xor] 'xor' test
#==============================================================================
"arithmetic tests" puts
[0 =] [0 0 +] '+' test
[1 =] [1 0 +] '+' test
[2 =] [1 1 +] '+' test
[1 =] [0 succ] 'succ' test
[2 =] [1 succ] 'succ' test
[0 =] [1 pred] 'pred' test
[1 =] [2 pred] 'pred' test
[13 =] [6 7 +] '+' test
[13 =] [7 6 +] '+' test
[1 =] [7 6 -] '-' test
[42 =] [7 6 *] '*' test
[6 =] [42 7 /] '/' test
[8.4 =] [42 5 /] '/' test
[6 =] [42 3 4 + /] '+ /' test
[3 =] [42 7 / 3 -] '/ -' test
[5 =] [6 5 4 - -] '- -' test
[7 =] [6 5 + 4 -] '+ -' test
[15 =] [6 5 4 + +] '+ +' test
#==============================================================================
"conditional tests" puts
[1 =] [true 1 2 choice] 'choice' test
[2 =] [false 1 2 choice] 'choice' test
[1 =] [true [1] if] 'if' test
[id] [false [1] if true] 'if' test
[1 =] [true [1] [2] ifte] 'ifte' test
[2 =] [false [1] [2] ifte] 'ifte' test
[2 =] [1 [zero?] [1] [2] ifte swap pop] 'ifte' test
[1 =] [0 [zero?] [1] [2] ifte swap pop] 'ifte' test
[1 =] [0 1 max] 'max' test
[1 =] [1 0 max] 'max' test
[0 =] [0 1 min] 'min' test
[0 =] [1 0 min] 'min' test
[13 =] [9 13 max] 'max' test
[13 =] [13 9 max]  'max' test
[9 =] [9 13 min] 'min' test
[9 =] [13 9 min] 'min' test
[3 = swap 3 = and] [1 2 3 [+] dip] 'dip' test

#==============================================================================
# basic stack operations
[333 =] [111  222 +] ' simple addition ' test
[21 =] [1  2  +  3  4  +  *] ' operations on stack (1)' test
[true =] [2  2  +  2  2  *  =] ' operations on stack (2)' test
[true =] [6  6  *  5  7  *  >] ' operations on stack (3)' test
[false =] [true  false  or  true  and  not] 'boolean operations' test
#=========================================
# list operations.
[[5 4 3 2 1] =] [[1 2 3 4 5] reverse] 'list reverse' test
[[peter paul mary jane] =] [[peter paul]  [mary jane]  concat] 'list concat' test

#=========================================
# list destructuring with symbols on stack.
[
    [b] =
]
[
    [b] uncons pop unit
] 'list destructuring (1)' test

[
    [b] =
]
[
    [b] first unit
] 'list destructuring (2)' test

[
    [3 4 1 2] =
]

[
    [1 2] [3 4] swoncat
] 'list destructuring (swoncat)' test
#internal definitions
#=========================================
[int0
    [myfunct
      [mf dup +].
      [mf] i
    ].
    [1 2 3 4] [myfunct] map
].

[[2 4 6 8] =] [int0] 'internal (1)' test
#=========================================
[int1
    [myfunct dup +].
    [1 2 3 4] [myfunct] map
].

[[2 4 6 8] =] [int1] 'internal (1)' test
#=========================================
[int2
    [myfunct 3 >].
    [1 2 3 4] [myfunct] filter
].

[[4] =] [int2] 'internal (2)' test
#=========================================
[fact 
    [zero?]
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
    [rest null?]
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
[pcomb
    [[]]
    [concat]
    primrec].

[[a b c d b c d c d d] =] [[a b c d] pcomb] '(primrec) pcomb' test
#=========================================
[area
  [pi 3.1415].
  [sq dup *].
  sq pi *].

[28.273500000000002 =] [3 area] '(internal def) area ' test
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
    [uncons [>] split]
    [[swap] dip cons concat]
    binrec].

[[0 2 4 6 6 7 8 9] =] [[0 9 6 7 8 4 6 2] qsort] '(binrec) qsort' test
#=========================================
[qsort1
    [joinparts 
        [p l1 l2] let
        l1 p l2 cons concat].
    [small?]
    []
    [uncons [>] split]
    [joinparts]
    binrec].

[[0 2 4 6 6 7 8 9] =] [[0 9 6 7 8 4 6 2] qsort1] '(binrec let) qsort' test
#=========================================
[qsort2
    [joinparts [p [*l1] [*l2] : [*l1 p *l2]] view].
    [small?]
    []
    [uncons [>] split]
    [joinparts]
    binrec].

[[0 2 4 6 6 7 8 9] =] [[0 9 6 7 8 4 6 2] qsort2] '(binrec view) qsort' test
#=========================================
[qsort3
    [joinparts [p [*l1] [*l2] : [*l1 p *l2]] view].
    [split_on_first_element uncons [>] split].
    [small?]
        []
        [split_on_first_element [l1 l2 : [l1 qsort3 l2 qsort3 joinparts]] view i]
    ifte].

[[0 2 4 6 6 7 8 9] =] [[0 9 6 7 8 4 6 2] qsort3] '(view) qsort' test
#=========================================


[roots
    # define our parameters (In classical concatanative languages, the internal
    # definitions are not used, but it makes our lives easier).
    [a b c] let

    [<< swap cons].
    [! [unit i] map].

    [" a: " a " b: " b " c: " c]! show
    
    [discr b dup * 4 a * c * - sqrt].
    [] " discr: " << discr << reverse show

    [root1 0 b - discr + 2 a * /].
    [root2 0 b - discr - 2 a * /].

    [] " root1: " << root1 << " root2: " << root2 << reverse show

    root1 root2].

[-5.0 = swap 3.0 = and] [2 4 -30 roots] 'roots' test
#=========================================

#=========================================
# stack shufflers that can be used for data structures
# the common format is [x1 x2 x3 ... : y1 y2 y3] view
# where the portions before ':' are the template for
# stack and portions after it are the template for result.
# the x1 x2 etc are symbols that gets bound to what
# is available on the stack. If these symbols repeat
# on the left side, then they are replaced on the
# body of left quote.
# list destructuring is also possible this way.
# ie: [1 2] [[a b] : a b] view => 1 2 on the stack.
# '_' is used to ignore the value on the stack
# '*' is used to indicate that there are 0 or more elements
# left. and *xxx can be used to name them.
# so [1 2 3 4 5] [[a *rest] : *rest a] view => 2 3 4 5 1 on the stack.
# it can also be done on tail.
# so [1 2 3 4 5] [[*rest a] : a *rest] view => 5 1 2 3 4 on the stack.
#=========================================
# basic
# 1 2 3 [a b c : a b c] => 1 2 3
[
    [1 2 3] =
]
[
    1 2 3 [a b c : a b c] view unit cons cons
] 'view(basic)' test

# reverse
# 1 2 3 [a b c : c b a] => 3 2 1
[
    [3 2 1] =
]
[
    1 2 3 [a b c : c b a] view unit cons cons
] 'view(reverse)' test

#list
# 1 2 3 [a b c : [a b c]] => [1 2 3]
[
    [1 2 3] =
]
[
    1 2 3 [a b c : [a b c]] view
] 'view(list:1)' test

# 1 2 3 [a b c : a [a b] a] => 1 [1 2] 1
[
    [1 [1 2] 1] =
]
[
    1 2 3 [a b c : a [a b] a] view unit cons cons
] 'view(list:2)' test

# with extra data
# 1 2 3 [a b c : a [5 5] a] => 1 [5 5] 1
[
    [1 [5 5] 1] =
]
[
    1 2 3 [a b c : a [5 5] a] view unit cons cons
] 'view(prefilled)' test

# more interesting stuff.
# ignore some parts.
# [1 2] 3 [[_ b] c : [b c]] => [2 3]
[
    [2 3] =
]
[
    [1 2] 3 [[_ b] c : [b c]] view
] 'view(_)' test

# slurp
# [1 2 4 5 6] 3 [[a *] b : [a b]] => [1 3]
[
    [1 3] =
]
[
    [1 2 4 5 6] 3 [[a *] b : [a b]] view
] 'view(*)' test

# reverse slurp
# [1 2 4 5 6] 3 [[* a] b : [a b]] => [6 3]
[
    [6 3] =
]
[
    [1 2 4 5 6] 3 [[* a] b : [a b]] view
] 'view(reverse*)' test

# named slurp
# [1 2 4 5 6] 3 [[a *rest] b : [*rest b]] => [2 4 5 6 3]
[
    [2 4 5 6 3] =
]
[
    [1 2 4 5 6] 3 [[a *rest] b : [*rest b]] view
] 'view(named *)' test

# named reverse slurp
# [1 2 4 5 6] 3 [[*rest a] b : [[b *rest] a]] => [[3 1 2 4 5] 6]
[
    [[3 1 2 4 5] 6] =
]
[
    [1 2 4 5 6] 3 [[*rest a] b : [[b *rest] a]] view
] 'view(named reverse *)' test

#=======================================
#locals
[
    100 =
]
[
    [myval 100].
    [myfunc [myval 200]. ].
    myfunc
    myval
] 'locals' test

#=============================================
#trees
[
    [1 484 121 4 484 49284 1089 9 16] = 
]
[
    [1 [22 11] [2 [22 [222] 33] 3] 4] [dup *] treestep
    unit cons cons cons cons cons cons cons cons
] 'treestep' test

[
    [1 [484 121] [4 [484 [49284] 1089] 9] 16] =
]
[
    [1 [22 11] [2 [22 [222] 33] 3] 4] [dup *] treemap
] 'treemap' test

[
    [1 22 11 2 22 222 33 3 4] =
]
[
    [1 [22 11] [2 [22 [222] 33] 3] 4] treeflatten
] 'treeflatten' test
[
    [[8 [7 6 5] 4 3] 2 1] =
]
[
    [1 2 [3 4 [5 6 7] 8] ] treereverse
] 'treereverse' test
[
    [1 [4 9] [[[16]]] 25] =
]
[
    [ 1 [2 3] [[[4]]] 5 ] [dup *] [map] treerec
] 'treerec' test

[
    [ [1 2 3] [1 2] [1 3] [1] [2 3] [2] [3] [] ] =
]
[
    [1 2 3]  powerlist
] 'powerlist' test
#=============================================
#modules
[tst [using open]
    [open 'abc'].
    [hide 'def'].
    [using hide].
] module

['abc' =] [tst:open] 'module' test
[put ' throw expected (success)' puts true] shield
['def' =] [tst:using] 'module' test
['!' =] [tst:hide] 'module' test
#=============================================
#shield
[cmdthrows
        [dup puts 'false shield' puts false] shield
        'hi there throw' throw].
[mycmd
        [dup puts 'true shield' puts true] shield
        cmdthrows].
mycmd

#=============================================
#java
[
    9 =
]
[
    ["I am here" length] java
] 'java(primitive)' test

[
    100 =
]
[
    [-100 java.lang.Math abs] java
] 'java(static method)' test

[
    integer?
]
[
    [java.util.Date new] java unit [getDay] concat java
] 'java(constructor)' test

[
    'abc' =
]
[
    [[~a ~b ~c] java.lang.String new] java
] 'java(array)' test

[
    '0.001' =
]
[
    ['0.001' v.V version$] java
] 'java(field write access)' test

[
    '0.001' =
]
[
    [v.V version$] java
] 'java(field read access)' test

#=============================================
# math
[
    0.8762980611683406 =
]
[
    0.64 acos
] 'math acos' test

#=============================================

??
"--------Success-----------" puts
