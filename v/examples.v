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

[comment pop].
#=========================================
[The tests have this format.
There are two quotes, one the result and the second the test quote.

First the test quote is evaluated, this will give an answer in the stack. Then the result
quote is executed, and it should leave a value true in the stack.
ie:

# result verification quote. (This checks if the value currently in the stack is equal to '2')
[
    2 =
]
# test quote. this is executed first, and then the result verification quote is executed.
[
    1 1 +
] 'message' test
# the 'message' is printed along with success or failure of the test.

] comment

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
    [uncons [>] split&]
    [[swap] dip cons concat]
    binrec].

[[0 2 4 6 6 7 8 9] =] [[0 9 6 7 8 4 6 2] qsort] '(binrec) fib' test
#=========================================
[qsort1
    [joinparts 
        [p l1 l2] let
        l1 p l2 cons concat].
    [small?]
    []
    [uncons [>] split&]
    [joinparts]
    binrec].

[[0 2 4 6 6 7 8 9] =] [[0 9 6 7 8 4 6 2] qsort1] '(binrec let) fib' test
#=========================================
[qsort2
    [joinparts [p [*l1] [*l2] : [*l1 p *l2]] V].
    [small?]
    []
    [uncons [>] split&]
    [joinparts]
    binrec].

[[0 2 4 6 6 7 8 9] =] [[0 9 6 7 8 4 6 2] qsort2] '(binrec V) fib' test
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

[cmdthrows
        [dup puts 'false shield' puts false] shield
        'hi there throw' throw].
[mycmd
        [dup puts 'true shield' puts true] shield
        cmdthrows].
mycmd

#=========================================
# stack shufflers that can be used for data structures
# the common format is [x1 x2 x3 ... : y1 y2 y3] V
# where the portions before ':' are the template for
# stack and portions after it are the template for result.
# the x1 x2 etc are symbols that gets bound to what
# is available on the stack. If these symbols repeat
# on the left side, then they are replaced on the
# body of left quote.
# list destructuring is also possible this way.
# ie: [1 2] [[a b] : a b] V => 1 2 on the stack.
# '_' is used to ignore the value on the stack
# '*' is used to indicate that there are 0 or more elements
# left. and *xxx can be used to name them.
# so [1 2 3 4 5] [[a *rest] : *rest a] V => 2 3 4 5 1 on the stack.
# it can also be done on tail.
# so [1 2 3 4 5] [[*rest a] : a *rest] V => 5 1 2 3 4 on the stack.
#=========================================
# basic
# 1 2 3 [a b c : a b c] => 1 2 3
[
    [1 2 3] =
]
[
    1 2 3 [a b c : a b c] V unit cons cons
] 'V(basci)' test

# reverse
# 1 2 3 [a b c : c b a] => 3 2 1
[
    [3 2 1] =
]
[
    1 2 3 [a b c : c b a] V unit cons cons
] 'V(reverse)' test

#list
# 1 2 3 [a b c : [a b c]] => [1 2 3]
[
    [1 2 3] =
]
[
    1 2 3 [a b c : [a b c]] V
] 'V(list:1)' test

# 1 2 3 [a b c : a [a b] a] => 1 [1 2] 1
[
    [1 [1 2] 1] =
]
[
    1 2 3 [a b c : a [a b] a] V unit cons cons
] 'V(list:2)' test

# with extra data
# 1 2 3 [a b c : a [5 5] a] => 1 [5 5] 1
[
    [1 [5 5] 1] =
]
[
    1 2 3 [a b c : a [5 5] a] V unit cons cons
] 'V(prefilled)' test

# more interesting stuff.
# ignore some parts.
# [1 2] 3 [[_ b] c : [b c]] => [2 3]
[
    [2 3] =
]
[
    [1 2] 3 [[_ b] c : [b c]] V
] 'V(_)' test

# slurp
# [1 2 4 5 6] 3 [[a *] b : [a b]] => [1 3]
[
    [1 3] =
]
[
    [1 2 4 5 6] 3 [[a *] b : [a b]] V
] 'V(*)' test

# reverse slurp
# [1 2 4 5 6] 3 [[* a] b : [a b]] => [6 3]
[
    [6 3] =
]
[
    [1 2 4 5 6] 3 [[* a] b : [a b]] V
] 'V(reverse*)' test

# named slurp
# [1 2 4 5 6] 3 [[a *rest] b : [*rest b]] => [2 4 5 6 3]
[
    [2 4 5 6 3] =
]
[
    [1 2 4 5 6] 3 [[a *rest] b : [*rest b]] V
] 'V(named *)' test

# named reverse slurp
# [1 2 4 5 6] 3 [[*rest a] b : [[b *rest] a]] => [[3 1 2 4 5] 6]
[
    [[3 1 2 4 5] 6] =
]
[
    [1 2 4 5 6] 3 [[*rest a] b : [[b *rest] a]] V
] 'V(named reverse *)' test

#=======================================
#java
[
    9 =
]
[
    ["I am here" length] java
] 'V (java primitive)' test

[
    100 =
]
[
    [-100 java.lang.Math abs] java
] 'V (java static method)' test

[
    integer?
]
[
    [java.util.Date new] java unit [getDay] concat java
] 'V (java constructor)' test

[
    'abc' =
]
[
    [[~a ~b ~c] java.lang.String new] java
] 'V (java array)' test

[
    '0.002' =
]
[
    [v.V version$] java
] 'V (java field read access)' test

[
    '0.001' =
]
[
    ['0.001' v.V version$] java
] 'V (java field write access)' test

#=============================================
# math
[
    0.8762980611683406 =
]
[
    0.64 acos
] 'math acos' test

"--------Success-----------" puts
