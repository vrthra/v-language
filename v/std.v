# Standard definitions.
[newline '' puts].

[pred 1 -].
[succ 1 +].

[dup [a : a a] view].
[pop [a :] view].
[swap [a b : b a] view].
[lroll [[a *rest] : [*rest a]] view].
[rroll [[*rest a] : [a *rest]] view].

[rollup [a b c : c a b] view].
[rolldown [a b c : b c a] view].
[rotate [a b c : c b a] view].

[rollupd [rollup] dip].
[rolldownd [rolldown] dip].
[rotated [rotate] dip].


[cons [a [*rest] : [a *rest]] view].
[unit [] cons].
[concat [[*a] [*b] : [*a *b]] view].

[dip [a b : [b i a]] view i].
[dupd [dup] dip].
[popd [pop] dip].
[swapd [swap] dip].
[dipd [dip] cons dip].

[x dup i].
[id [a : a] view].

[uncons [[a *rest] : a [*rest]] view].
[first [[a *rest] : a] view].
[rest [[a *rest] : [*rest]] view].

[zero? >decimal 0.0 =].
[empty? size zero?].
[null? [number?] [zero?] [empty?] ifte].
[small? [list?] [dup size swap pop dup zero? swap 1 = or] [dup zero? swap 1 = or] ifte].
[has? [=] split pop size zero? not swap pop].
[in? swap has?].
[at dup 1 + swap [take] dip drop i].

[lset
#(val [lst] idx)
   [val lst idx : [lst idx take val lst idx 1 + drop ] ] view i cons concat
].

[size 0 [pop 1 +] fold].

[leaf? list? not].

[max [a b : [[a b >] [a] [b] ifte]] view i].
[min [a b : [[a b <] [a] [b] ifte]] view i].


[all? map true [and] fold].
[all! map! true [and] fold].
[some? map false [or] fold].
[some! map! false [or] fold].

[filter split pop].
[filter! split! pop].

[step [swap] dip fold].


[xor [a b : a b a b] view or [and not] dip and].

[of swap at].

[swons swap cons].
[reverse [] swap [swons] step].
[shunt [swons] step].
[swoncat swap concat].
[unitlist [] cons].
# parents 1 for map, 2 for let
[let reverse [unit cons reverse $me &parent &parent &. true] map! pop].

[abs unit [java.lang.Math abs] concat java].
[acos unit [java.lang.Math acos] concat java].
[sqrt >string unit [java.lang.Double new] concat java unit [java.lang.Math sqrt] concat java].

# note that swap is needed since dip does a swap
[binrec
    [if then rec1 rec2 :
        [if then
            [rec1 i
                # execute binrec on both parts.
                [if then rec1 rec2 binrec] dip
                [if then rec1 rec2 binrec] i
             rec2 i]
         ifte]] view i].

[genrec
    [if then rec1 rec2 :
        [if then
            [rec1 i
                [if then rec1 rec2 genrec]
             rec2 i]
         ifte]] view i].
[linrec
    [if then rec1 rec2 :
        [if then
            [rec1 i
                # we dont need [] i, it is just for clarity.
                [if then rec1 rec2 linrec] i
             rec2 i]
         ifte]] view i].

[powerlist 
    [null?]
    [unitlist]
    [uncons]
    [dup swapd [cons] map popd swoncat]
    linrec].


[tailrec [] linrec].
#[tailrec
#    [if then rec :
#        [if
#            then
#            [rec i
#                [if then rec tailrec] i
#            ]
#         ifte]] view i].

# from joy mailing list.
#[primrec
#    [pr
#        [pop pop small?]
#        [pop pop]
#        [[dup pred] dipd
#            dup
#            [pr] dip
#            i]
#        ifte
#    ].
#    [first] dip pr].

[primrec
    [lzero?
        [list?] [empty?]
        [zero?]
        ifte].
    [lnext
        [list?] [rest]
        [pred]
        ifte].

    [param then rec :
        [[param lzero?] then
            # we dont need [] i, it is just for clarity.
            [param
                [param lnext then rec primrec] i
            rec i]
         ifte]] view i].

# recursively apply rec to the leaves of a tree.
[treestep
    [tree rec] let
    tree
    [leaf?] rec
        [[empty?] [pop]
            [dup
                [first rec treestep] dip
                [rest rec treestep] i]
       ifte]
    ifte
].

# produce the same structure as input tree.
#[treemap
#    [tree rec] let
#    tree
#    [leaf?] rec
#        [[empty?] []
#            [dup
#                [first rec treemap] dip
#                [rest rec treemap] i cons]
#       ifte]
#    ifte
#].

[treeshunt [swons] treestep].
[treeflatten [] swap treeshunt reverse].

[treerec
    [tree o c] let
    tree
    [leaf?] o
        [[empty?] [pop]
            [[o c treerec] c i]
        ifte]
    ifte
].

[treegenrec
    [tree o1 o2 c] let
    tree
    [leaf?] o1
        [[empty?] [pop]
            [o2 i [o1 o2 c treegenrec] c i]
        ifte]
    ifte
].

[treereverse [] [reverse] [map] treegenrec].
[treemap [] [map] treegenrec].
#[treemap [map] treerec].
