[beer
    [bottles
        [0 =] ['No more bottles of beer' put] if 
        [1 =] ['One bottle of beer' put] if 
        [1 >] [dup put ' bottles of beer' put] if].

    [0 =] [newline]
        [bottles ' on the wall, ' put bottles newline
        'Take one down and pass it around, ' put pred bottles ' on the wall' puts newline]
   tailrec].

99 beer

