[let reverse [unit cons reverse $me $parent @ true] map pop].

[abs unit [java.lang.Math abs] concat java].
[acos unit [java.lang.Math acos] concat java].
[sqrt >string unit [java.lang.Double new] concat java unit [java.lang.Math sqrt] concat java].
[pred 1 -].
[succ 1 +].

[dup [a : a a] V].
[dupd [dup] dip].
[pop [a :] V].
[popd [pop] dip].
[swap [a b : b a] V].
[swapd [swap] dip].
[lroll [[a *rest] : [*rest a]] V].
[rroll [[*rest a] : [a *rest]] V].


[cons [a [*rest] : [a *rest]] V].
[unit [] cons].
[concat [[*a] [*b] : [*a *b]] V].
[dip [a b : [b i a]] V i].
[x dup i].
[id [a : a] V].

[uncons [[a *rest] : a [*rest]] V].
[first [[a *rest] : a] V].
[rest [[a *rest] : [*rest]] V].

[zero? dup >decimal 0.0 =].
[empty? dup size zero? swap pop].
[null? number? [zero?] [empty?] ifte].
[small? [list?] [dup size swap pop zero? swap 1 = or] [zero? swap 1 = or] ifte].

[leaf? list? not].

[max [a b : [[a b >] [a] [b] ifte]] V i].
[min [a b : [[a b <] [a] [b] ifte]] V i].


[all map true [and] fold].
[all& map& true [and] fold].
[some map false [or] fold].
[some& map& false [or] fold].


[xor [a b : a b a b] V or [and not] dip and].


