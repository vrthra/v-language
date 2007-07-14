# Standard definitions.
[swons swap cons].
[reverse [] swap [swons] step].

[let reverse [unit cons reverse . true] map pop].

[abs unit [java.lang.Math abs] concat java].
[acos unit [java.lang.Math acos] concat java].
[sqrt >string unit [java.lang.Double new] concat java unit [java.lang.Math sqrt] concat java].
[pred 1 -].
[succ 1 +].

[dup [a : a a] view].
[dupd [dup] dip].
[pop [a :] view].
[popd [pop] dip].
[swap [a b : b a] view].
[swapd [swap] dip].
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
[x dup i].
[id [a : a] view].

[uncons [[a *rest] : a [*rest]] view].
[first [[a *rest] : a] view].
[rest [[a *rest] : [*rest]] view].

[zero? >decimal 0.0 =].
[empty? size zero?].
[null? [number?] [zero?] [empty?] ifte].
[small? [list?] [dup size swap pop dup zero? swap 1 = or] [dup zero? swap 1 = or] ifte].
[has? swap in?].

[leaf? list? not].

[max [a b : [[a b >] [a] [b] ifte]] view i].
[min [a b : [[a b <] [a] [b] ifte]] view i].


[all map true [and] fold].
[all& map& true [and] fold].
[some map false [or] fold].
[some& map& false [or] fold].


[xor [a b : a b a b] view or [and not] dip and].

[of swap at].

