# Quadratic equation.
[root
    # define our parameters (In classical concatanative languages, the internal
    # definitions are not used, but it makes our lives easier).
    [a b c] let

    # define the discriminent
    [discr b dup * 4 a * c * - sqrt].

    # and fetch the roots.
    [root1 0 b - discr + 2 a * /].
    [root2 0 b - discr - 2 a * /].

    # output results
    "root1 :" put root1 puts
    "root2 :" put root2 puts
].

#Usage 
2 4 -30 root

