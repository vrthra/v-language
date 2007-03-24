# This is an example association list from doc id to word id list.  
# an example adapted from the code here http://www.kimbly.com/code/invidx/
# V does not have the any bias towards numbers.
[data
    [ ['d1' ['AA' 'BB' 'CC']] 
      ['d2' ['AA' 'BB']] 
      ['d3' ['AA']]
      ['d4' []]
    ]
].

# invert :: Docs_to_words -> words_to_docs
# takes an association list from docs to words, returns an association
#   list from words to docs
[invert
    dup word_list
    [swap docs_with_word unit cons]        # combine the word and the docs list
    map&
].

# word_list :: Docs_to_words -> words
# returns the union of all words in all docs
[word_list
    [rest first] map&   # strip off the doc ids
    [] [concat] fold&   # concat all the word lists
    nub                 # remove duplicate words
].              

# doc_list :: Docs_to_words -> docs
# returns the set of all documents in the list
[doc_list [first] map&].        # just keep the doc ids

# docs_with_word :: Docs_to_words word -> docs
[docs_with_word
    [rest first in?] filter&    # remove docs that don't have the given word
    doc_list
].

# nub :: list -> list
# Removes duplicate elements in a list.  The name "nub" comes from the 
# function of the same name in haskell.  This implementation is O(n^2)
[nub
    [] [[has?]            # if the current element is already in the list,
            [pop]         # ignore it
            [swons]       # otherwise add it to the list
        ifte
    ] fold&
].

data invert [puts] step

