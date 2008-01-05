[a 1].
[b 1].  
[closure
    [a 100].
    [b 200].
    $me
].

# generate a closure and evaluate the value of a in the closure
closure [a] &i

# duplicate a closure and rebind the value of a in it,
# then check that the rebound value is present in the original
# closure. Note that this will work only if singleassignment is false.
closure dup [[a 101].] &i [a] &i

