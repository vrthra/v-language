'Utils' puts
[
    #qsort
    [qsort
      [joinparts [p [*l1] [*l2] : [*l1 p *l2]] view].
      [split_on_first uncons [>] split].
      [small?]
        []
        [split_on_first [l1 l2 : [l1 qsort l2 qsort joinparts]] view i]
      ifte].

#qsort like joy
[qsort
   [small?] []
     [uncons [>] split]
     [[p [*l] [*g] : [*l p *g]] view]
   binrec].

[8 7 6 5 4 2 1 3 9] qsort puts
] pop

#[splitat [dup] dip swap [dup] dip swap take [drop] dip].

# take one element, use it as pivot on the other list, split it
# join the pivot to the first portion, 

# take next element, use it as pivot on the other remaining list, split it
# join the [earlier list] [current first portion] pivot, repeat.

    #([m] [arr1] [arr2] )
    # take an element e2 from arr2, use it as a pivot, split arr1 to A B
    # join m A e2 to form next m, set B as next arr1
    

[merge
    [mergei 
        uncons [swap [>] split] dip
        [[*m] e2 [*A1] B1 a2_ : [*m *A1 e2] B1 a2_] view].

    [a b : [] a b] view
    [size zero?] [pop concat]
        [mergei]
    tailrec
].

[msort
   [splitat [arr a : [arr a take arr a drop]] view i].
   [splitarr dup size 2 / >int splitat].

   [small?] []
     [splitarr]
     [merge]
   binrec].

[8 7 6 5 4 2 1 3 9] msort puts

