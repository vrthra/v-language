# About SICP
#
# The following V code is derived from the examples provided in the
# book: "Structure and Interpretation of Computer Programs, Second Edition"
# by Harold Abelson and Gerald Jay Sussman with Julie Sussman.
#      http://mitpress.mit.edu/sicp/
#

[book:chapter puts].
[book:section puts].
[book:exercise puts].

[SICP Chapter 01 Examples in V] book:chapter

[1.1.1 The Elements of Programming - Expressions] book:section

486 puts
137 349 + puts
1000 334 - puts
5 99 * puts
10 5 / puts
2.7 10.0 + puts
21 35 + 12 + 7 + puts
25 4 * 12 * puts
3 5 * 10 + 6 - puts
3 2 4 * 3 + 5 +  * 10 7 - 6 + + puts


[1.1.2 The Elements of Programming - Naming and the Environment] book:section

[size 2].
size puts
5 size * puts
[pi 3.14159].
[radius 10.0].
pi radius * radius * puts
[circumference 2.0 pi * radius *].
circumference puts


[1.1.3 The Elements of Programming - Evaluating Combinations] book:section

2 4 6 * + 3 5 + 7 + * puts

[1.1.4 The Elements of Programming - Compound Procedures] book:section

[square dup *].
# with vars.
# [square [x] let x x *].

21 square puts
2 5 + square puts
3 square square puts

[sum_of_squares square swap square +].
#[sum_of_squares square swap square +].
#[sum_of_squares [x y] let x square y square +].

[f dup 2 * [1 +] dip sum_of_squares].
#[f [a] let a 1 + a 2 * sum_of_squares].

5 f puts

[1.1.5 The Elements of Programming - The Substitution Model for Procedure Application] book:section

5 f puts
5 dup 2 * [1 +] dip sum_of_squares puts
5 5 2 * [1 +] dip sum_of_squares puts
5 10 [1 +] dip sum_of_squares puts
6 10 sum_of_squares puts
6 10 square swap square + puts
6 10 10 * swap square + puts
6 100 swap square + puts
100 6 square + puts
100 6 6 * + puts
100 36 + puts
136 puts

[1.1.6 The Elements of Programming - Conditional Expressions and Predicates] book:section

[abs [x] let
   [ [x 0 >] [x]
     [x 0 =] [0]
     true    [0 x -]
   ] when
].

[abs [x] let
   [x 0 <] [0 x -]
     [x]
   ifte
].

[x 6].
x 5 > x 10 < and puts
[ge [x y] let
   x y > x y = or
].
[ge_1 [x y] let
   x y < not
].

[Exercise 1.1] book:exercise

10 puts
5 3 + 4 + puts
9 1 - puts
6 2 / puts
2 4 * 4 + 6 - puts
[a 3].
[b a 1 +].
a b + a + b * puts
a b = puts
[b a > b a b * < and] [b] [a] ifte puts
[ [a 4 =] [6]
  [b 4 =] [6 7 + a +]
  true    [25]
] when puts
2 [b a >] [b] [a] ifte + puts

[ [a b >] [a]
  [a b <] [b]
  true    [-1]
] when a 1 + * puts

[Exercise 1.2] book:exercise

5.0 4.0 + 2.0 3.0 6.0 4.0 5.0 / + - - +
3.0 6.0 2.0 - * 2.0 7.0 - * / puts

[Exercise 1.3] book:exercise

[three_n [n1 n2 n3] let
   [n1 n2 >] [
      [n1 n3 >] [
         [n2 n3 >] [n1 n1 * n2 n2 * +]
            [n1 n1 * n3 n3 * +]
         ifte
      ] [n1 n1 * n3 n3 * +]
      ifte
   ] [[n2 n3 >] [
         [n1 n3 >] [n2 n2 * n1 n1 * +]
            [n2 n2 * n3 n3 * +]
         ifte
      ] [n2 n2 * n3 n3 * +]
      ifte
   ] ifte
].

[Exercise 1.4] book:exercise

[a_plus_abs_b [a b] let
   [b 0 >] [a b +]
     [a b -]
   ifte
]

[Exercise 1.5] book:exercise
[p p].
[test [x y] let
   [x 0 =] [0]
     [y]
   ifte
].
# commented out as this is in infinite loop
# 0 p test


[1.1.7 The Elements of Programming - Example: Square Roots by Newton's Method] book:section

[square dup *].

[good_enough? [guess x] let
   guess square x - abs 0.001 <
].

[average [x y] let
   x y + 2.0 /
].

[improve [guess x] let
   guess x guess / average
].

[sqrt_iter [guess x] let
   [guess x good_enough?] [guess]
      [guess x improve x sqrt_iter]
   ifte
].

[sqrt_0 [x] let
    1.0 x sqrt_iter
].

9 sqrt_0 puts
100 37 + sqrt_0 puts
2.0 sqrt 3.0 sqrt + sqrt puts
1000.0 sqrt square puts

[Exercise 1.6] book:exercise
# It has no meaning in V, if the argument is a quote -- [] then it
# is not evaluated until it is dequoted. standard ifte is nothing but
# a normal v word that unquotes the correct argument.

[Exercse 1.7] book:exercise
[Exercise 1.8] book:exercise

[1.1.8 The Elements of Programming - Procedures as Black-Box Abstractions] book:section

[square dup *].
[square log double exp].

[double dup +].

# Local variable names are not encouraged in V as it is based on combinatory calculus
# but see below

[square [x] let x x *].
[square [y] let y y *].

[good_enough? [guess x] let
   guess square x - abs 0.001 <
].

[improve [guess x] let
   guess x guess / average
].

[sqrt_iter [guess x] let
    [guess x good_enough?] [guess]
       [guess x improve x sqrt_iter]
    ifte
].

[sqrt 1.0 swap sqrt_iter].

5 square puts
25 sqrt puts

# Block-structured
[sqrt [x] let 
   [good_enough? [guess x] let
       guess square x - abs 0.001 <
   ].

   [improve [guess x] let 
       guess x guess / average
   ].

   [sqrt_iter [guess x] let
      [guess x good_enough?] [guess]
         [guess x improve x sqrt_iter]
      ifte
   ].

   1.0 x sqrt_iter
].

# Taking advantage of lexical scoping
[sqrt [x] let 
   [good_enough? [guess] let
       guess square x - abs 0.001 <
   ].

   [improve [guess] let 
       guess x guess / average
   ].

   [sqrt_iter [guess] let
      [guess good_enough?] [guess]
         [guess improve sqrt_iter]
      ifte
   ].

   1.0 sqrt_iter
].

[1.2.1 Procedures and the Processes They Generate - Linear Recursion and Iteration] book:section

# Recursive
[factorial
   [1 =] [1]
      [dup 1 - factorial *]
   ifte
].

6 factorial puts

[fact_iter [product counter max_count] let
    [counter max_count >] [product]
        [counter product * counter 1 + max_count fact_iter]
    ifte
].


[factorial [n] let
   1 1 n fact_iter
].

# Iterative, block-structured (from footnote)
[factorial [n] let
   [iter [product counter] let
      [counter n >] [product]
        [counter product * counter 1 + fact_iter]
      ifte
   ].
   1 1 iter
].

# Exercise 1.9
[inc [a] let a 1 + ].
[dec [a] let a 1 - ].
[plus [a b] let
   [a 0 =] [b]
      [b a dec plus inc]
   ifte
].

[plus_1 [a b] let
   [a 0 =] [b]
      [b inc a dec plus_1]
   ifte
].

# Exercise 1.10
[A [x y] let
  [ [y 0 =] [0]
    [x 0 =] [2 y *]
    true    [x 1 - x y 1 - A A]
  ] when
].

1 10 A puts
2 4 A puts
3 3  A puts

[fx [n] let 0 n A].
[g [n] let 1 n A].
[h [n] 2 n A].
[k [n] 5 n * n *].


[1.2.2 Procedures and the Processes They Generate - Tree Recursion] book:section

# Recursive
[fib [n] let
  [ [n 0 =] [0]
    [n 1 =] [1]
    true [n 1 - fib n 2 - fib +]
  ] when
].

# Iterative
[fib_iter [a b count] let
   [count 0 =] [b]
      a b + a count 1 - fib_iter
   ifte
].

[fib [n] let
   1 0 n fib_iter
].

# Counting change
[first_denomination(x)
   [ [x 1 =] [1]
     [x 2 =] [5]
     [x 3 =] [10]
     [x 4 =] [25]
     [x 5 =] [50]
   ] when
].

[cc [amount kinds_of_coins] let
   [ [amount 0 =] [1]
     [amount 0 <] [0]
     [kinds_of_coins 0 =] [0]
     true [
      amount kinds_of_coins 1 - cc
      amount kinds_of_coins first_denomination - kinds_of_coins cc +]
   ] when
].

[count_change [amount] let
   amount 5 cc
].

100 count_change puts

# Exercise 1.11
[fi [n] let
   [n 3 <] [n]
     [n 1 - fi 2 n 2 - fi * + 3 n 3 - fi * +]
   ifte
].

[fi_iter [a b c count] let
   [count 0 =] [c]
     [a 2 b * + 3 c * + a b count 1 - fi_iter]
   ifte
].

[f [n] let 2 1 0 n fi_iter].

# Exercise 1.12
[pascals_triangle [n k] let 
   [n 0 = k 0 = n k = or or] [1]
     [n 1 - k 1 - pascals_triangle n 1 - k pascals_triangle +]
   ifte
].


# 1.2.3 Procedures and the Processes They Generate - Orders of Growth

# Exercise 1.15
[cube [x] x x * x *].
[p [x] let 3.0 x * 4.0 x cube * -].
[sine [angle] let
   [angle 0.1 > not] [angle]
     [angle 3.0 / sine p]
   ifte
].


# 1.2.4 Procedures and the Processes They Generate - Exponentiation

# Linear recursion
[expt [b n] let
   [n 0 =] [1]
     [b b n 1 - expt *]
   ifte
].

# Linear iteration
[expt_iter [b counter product] let
   [counter 0 =] [product]
      [b counter 1 - b product * expt_iter]
   ifte
].

[expt_1 [b n] let
   b n 1 expt_iter
].

# Logarithmic iteration
[even? [n] let n 2 mod 0 eq].

[fast_expt [b n] let
   [ [n 0 =] [1]
     [n even?] [b n 2 / fast_expt square]
     true [b b n 1 - fast_expt *]
   ] when
].

# Exercise 1.17
[multiply [a b] let
   [b 0 =] [0]
      [a a b dec multiply plus]
   ifte
].

# Exercise 1.19
# exercise left to reader to solve for p' and q'
# [fib_iter [a b p q count] let
#    [ [count 0 =] [b]
#      [count even?] [a b p' q' count 2 / fib_iter]
#      true [ b q * a q * + a p * + b p * + a q * + p q count 1 - fib_iter]
#    ] when
# ].
# [fib [n] let 
#    1 0 0 1 n fib_iter
# ].


# 1.2.5 Procedures and the Processes They Generate - Greatest Common Divisors

[mod [a n] let a n / >int n * a swap -].
[gcd [a b] let
   [b 0 =] [a]
      [b a b mod gcd]
   ifte
].

40 6 gcd puts

# Exercise 1.20
206 40 gcd puts


# 1.2.6 Procedures and the Processes They Generate - Example: Testing for Primality

# prime
[divides? [a b] let b a mod 0 =].

[find_divisor [n test_divisor] let
   [ [test_divisor square n >] [n]
     [test_divisor n divides?] [test_divisor]
     true [n test_divisor 1 + find_divisor]
   ] when
].

[smallest_divisor [n] let n 2 find_divisor].

[prime? [n] let n n smallest_divisor =].

# fast_prime
[expmod [nbase nexp m] let
   [ [nexp 0 =] [1]
     [nexp even?] [nbase nexp 2 / m expmod square m mod]
     true [base nbase nexp 1 - m expmod * m mod]
   ] when
].

# random yet to be done.
[fermat_test [n] let
   [try_it [a] let a n n expmod a eq].
   n 1 - random 1 + n try_it
].

[fast_prime [n ntimes] let
   [ [ntimes 0 =] [true]
     [n fermat_test] [n ntimes 1 - fast_prime]
     true [false]
   ] when
].

# Exercise 1.21
199 smallest_divisor puts
1999 smallest_divisor puts
19999 smallest_divisor puts

# Exercise 1.22
[report_prime [elapsed_time] let
   ' *** ' put elapsed_time puts
].

[start_prime_test [n start_time] let
   [n prime?] # now is not implemented yet.
      [now start_time - report_prime]
   if
].

[timed_prime_test [n] let
   '' puts n put
   n now start_prime_test
].

# Exercise 1.25
[expmod_1 [nbase nexp m] let
   nbase nexp fast_expt m mod
].

# Exercise 1.26
[expmod_2 [nbase nexp m] let
   [ [nexp 0 =] [1]
     [nexp even?] [nbase nexp 2 / m expmod nbase nexp 2 / m expmod * m mod]
     [true]  [nbase nbase nexp 1 - m expmod * m mod]
   ] when
].

# Exercise 1.27
[carmichael [n] let
   n 10 fast_prime n prime? not and
].

561 carmichael puts
1105 carmichael puts
1729 carmichael puts
2465 carmichael puts
2821 carmichael puts
6601 carmichael puts

# 1.3 Formulating Abstractions with Higher-Order Procedures

[cube [x] x x * x *].

# 1.3.1 Formulating Abstractions with Higher-Order Procedures - Procedures as Arguments

[sum_integers [a b] let
   [a b >] [0]
      [a a 1 + b sum_integers +:
   ifte
].

[sum_cubes [a b] let
   [a b >] [0]
      [a cube a 1 + b sum_cubes +]
   ifte
].

[pi_sum [a b] let
   [a b >] [0]
      [1 a a 2 + * / a 4 + b pi_sum +]
   ifte
].

[sum [term a next b] let
   [a b >] [0]
      [a term i term a next i next b sum +]
   ifte
].

# Using sum
[inc_1 [n] let n 1 +].

[sum_cubes_1 [a b] let
   [cube] a [inc] b sum
].

1 10 sum_cubes_1 puts

[identity 0 pop].

[sum_integers_1 [a b] let
   [identity] a [inc_1] b sum
].

1 10 sum_integers_1 puts

[pi_sum_1 [a b] let
   [pi_term x { return 1.0 / (x * (x + 2.0))].
   [pi_next(x) { return x + 4.0; ].
   [pi_term] a [pi_next] b sum
].

8.0 1 1000 pi_sum_1 * puts

[integral [f a b dx] let
   [add_dx [x] let x dx + ].
   f a dx 2 / + [add_dx] b sum * dx
].

[cube [x] let x x * x *].

[cube] 0 1 0.01 integral puts
[cube] 0 1 0.001 integral puts

# Exercise 1.29
[simpson(f, a, b, n) {
   var h = abs(b - a) / n;
   function sum_iter(term, start, next, stop, acc) {
      if (start > stop)
         return acc;
      else
         return sum_iter(term, next(start), next, stop, acc + term(a + start * h));
   }
   return h * sum_iter(f, 1, inc, n, 0.0);
].
simpson(cube, 0.0, 1.0, 100);

// Exercise 1.30
function sum_iter(term, a, next, b, acc) {
   if (a > b)
      return acc;
   else
      return sum_iter(term, next(a), next, b, acc + term(a));
}
// 'sum_cubes_2' reimplements 'sum_cubes_' but uses 'sum_iter' in place of 'sum'
function sum_cubes_2(a, b) { return sum_iter(cube, a, inc, b, 0); }
sum_cubes_2(1, 10);

// Exercise 1.31
// a.
function product(term, a, next, b) {
   if (a > b)
      return 1;
   else
      return term(a) * product(term, next(a), next, b);
}
function factorial_2(n) { return product(identity, 1, inc, n); }

// b.
function product_iter(term, a, next, b, acc) {
   if (a > b)
      return acc;
   else
      return product_iter(term, next(a), next, b, acc * term(a));
}
function factorial_3(n) { return product_iter(identity, 1, inc, n, 1); }

// Exercise 1.32
// a.
function accumulate(combiner, nullValue, term, a, next, b) {
   if (a > b)
      return nullValue;
   else
      return combiner(term(a), accumulate(combiner, nullValue, term, next(a), next, b));
}

// sum:     accumulate(plus, 0, identity, a, inc, b);
function sum_1(a, b) {
   return accumulate(plus, 0, identity, a, inc, b)
}
function product_1(a, b) { return a * b; }
// product: accumulate(multiply, 1, identity, a, inc, b);

// b.
// NOTE: starting value of 'acc' is 'nullValue'
function accumulate_iter(combiner, term, a, next, b, acc) {
   if (a > b)
      return acc;
   else
      return accumulate_iter(combiner, term, next(a), next, b, combiner(acc, term(a)));
}

// sum:     accumulate_iter(plus, identity, a, inc, b, 0);
function sum_2(a, b) {
    return accumulate_iter(function(x,y) { return x+y }, identity, a, inc, b, 0)
}
// function times(a, b) { return a * b; }
// product: accumulate_iter(times, identity, a, inc, b, 1);
function product_2(a, b) {
   return accumulate_iter(multiply, identity, a, inc, b, 1)
}

// Exercise 1.33
function filtered_accumulate(combiner, nullValue, term, a, next, b, pred) {
   if (a > b)
      return nullValue;
   else if (pred(a))
      return combiner(term(a), filtered_accumulate(combiner, nullValue, term, next(a), next, b, pred));
   else
      return filtered_accumulate(combiner, nullValue, term, next(a), next, b, pred);
}

// a.
filtered_accumulate(plus, 0, square, 1, inc, 5, prime);  // 39

// b. Not sure how to implement this without modifying 'filtered_accumulate' to have 'pred'
//    accept two arguments


[edit]
// 1.3.2 Formulating Abstractions with Higher-Order Procedures - Constructing Procedures Using Lambda

function pi_sum_2(a, b) {
   return sum(
      function(x) { return 1.0 / (x * (x + 2.0)); },
      a,
      function(x) { return x + 4.0 },
      b);
}

function integral_1(f, a, b, dx) {
   return sum(f, a + (dx / 2.0), function(x) { return x + dx; }, b) * dx;
}

function plus4(x) { return x + 4; }

plus4_1 = function(x) { return x + 4; }

print ((function(x, y, z) { return x + y + square(z) }) (1, 2, 3));

// Using let
function f_1(x, y) {
   function f_helper(a, b) {
      return x*square(a) + y*b + a*b;
   }
   return f_helper(1 + x*y, 1 - y)
}

function f_2(x, y) {
   return (function(a, b) { return x*square(a) + y*b + a*b; }) (1 + x*y, 1 - y);
}

function f_3(x, y) {
   a = 1 + x*y;
   b = 1 - y;
   return x*square(a) + y*b + a*b;
}

// javascript does not have let binding - used lambda to emulate
var x = 5;
print (function() {
         var x = 3;
         return x + (x * 10);
       }() + x);

var x = 2;
print (function(x) {
         var y = x + 2;
         var x = 3;
         return x * y;
       }(x));

function f_4(x, y) {
   a = 1 + x*y;
   b = 1 - y;
   return x*square(a) + y*b + a*b;
}

// Exercise 1.34
function f_5(g) { return g(2); }
print (f_5(square));
print (f_5(function(z) { return z * (z + 1) } ));


[edit]
// 1.3.3 Formulating Abstractions with Higher-Order Procedures - Procedures as General Methods

// Half-interval method
function close_enough(x, y) {
   return abs(x - y) < 0.001;
}

function positive(x) { return x >= 0.0; }
function negative(x) { return !(positive(x)); }

function search(f, neg_point, pos_point) {
   midpoint = average(neg_point, pos_point);
   if (close_enough(neg_point, pos_point))
      return midpoint;
   else
      test_value = f(midpoint);
      if (positive(test_value))
         return search(f, neg_point, midpoint);
      else if (negative(test_value))
         return search(f, midpoint, pos_point);
      else
         return midpoint;
}

function half_interval_method(f, a, b) {
   a_value = f(a);
   b_value = f(b);
   if (negative(a_value) && positive(b_value))
      return search(f, a, b);
   else if (negative(b_value) && positive(a_value))
      return search(f, b, a);
   else
      throw ("Exception: Values are not of opposite sign " + a + " " + b);
}

print (half_interval_method(Math.sin, 2.0, 4.0));

print (half_interval_method(function(x) { return x*x*x - 2.0*x - 3.0; }, 1.0, 2.0));

// Fixed points
tolerance = 0.00001

function fixed_point(f, first_guess) {
   function close_enough(v1, v2) {
      return abs(v1 - v2) < tolerance;
   }
   function tryit(guess) {
      next = f(guess);
      if (close_enough(guess, next))
         return next;
      else
         return tryit(next);
   }
   return tryit(first_guess);
}

print (fixed_point(Math.cos, 1.0));

print (fixed_point(function(y) { return Math.sin(y) + Math.cos(y); }, 1.0));

// note: this function does not converge
function sqrt_4(x) {
   return fixed_point(function(y) { return parseFloat(x) / y; }, 1.0)
}

function sqrt_5(x) {
   return fixed_point(function(y) { return average(y, parseFloat(x) / y); }, 1.0)
}

// Exercise 1.35
function golden_ratio() {
   return fixed_point(function(x) { return 1.0 + 1.0 / x; }, 1.0);
}

// Exercise 1.36
// Add the following line to function, 'fixed_point':
//  ... var next = f(guess);
//  print(next);
//  ... if (close_enough(guess, next))
// -- 35 guesses before convergence
print(fixed_point(function(x) { return Math.log(1000.0) / Math.log(x); }, 1.5));
// -- 11 guesses before convergence (average_damp defined below)
print(fixed_point(average_damp(function(x) { return Math.log(1000.0) / Math.log(x); }), 1.5));

// Exercise 1.37
// exercise left to reader to define cont_frac
// cont_frac(function(i) { return 1.0; }, function(i) { return 1.0; }, k)

// Exercise 1.38 - unfinished

// Exercise 1.39 - unfinished


[edit]
// 1.3.4 Formulating Abstractions with Higher-Order Procedures - Procedures as Returned Values

function average_damp(f) {
   return function(x) { return average(parseFloat(x), f(x)); }
}

print ((average_damp(square)) (10.0));

function sqrt_6(x) {
   return fixed_point(average_damp(function(y) { return parseFloat(x) / y; }), 1.0);
}

function cube_root(x) {
   return fixed_point(average_damp(function(y) { return parseFloat(x) / square(y); }), 1.0)
}

print (cube_root(8));

// Newton's method
dx = 0.00001
function deriv(g) {
   return function(x){ return parseFloat(g(x + dx) - g(x)) / dx; };
}

function cube_2(x) { return x * x * x; }

print (deriv(cube_2) (5.0));

function newton_transform(g) {
   return function(x) { return x - (parseFloat(g(x)) / (deriv(g) (x))); };
}

function newtons_method(g, guess) {
   return fixed_point(newton_transform(g), guess);
}

function sqrt_7(x) {
   return newtons_method(function(y) { return square(y) - x; } , 1.0);
}

// Fixed point of transformed function
function fixed_point_of_transform(g, transform, guess) {
   return fixed_point(transform(g), guess);
}

function sqrt_8(x) {
   return fixed_point_of_transform(function(y) { return x / y; }, average_damp, 1.0);
}

function sqrt_9(x) {
   return fixed_point_of_transform(function(y) { return square(y) - x; }, newton_transform, 1.0)
}

// Exercise 1.40
function cubic(a, b, c) {
   return function(x) { return x*x*x + a*x*x + b*x + c; };
}

print(newtons_method(cubic(5.0, 3.0, 2.5), 1.0)); // -4.452...

// Exercise 1.41
function double_(f) {
   return function(x) { return f(f(x)); };
}
print((double_(inc))(5));                         //  7
print((double_(double_(inc)))(5));                //  9
print((double_(double_(double_(inc))))(5));       // 13

// Exercise 1.42
function compose_(f, g) {
   return function(x) { return f(g(x)); };
}
print((compose_(square, inc))(6));                // 49

// Exercise 1.43
function repeated(f, n) {
   function iterate(arg, i) {
      if (i > n)
         return arg;
      else
         return iterate(f(arg), i + 1);
   }

   return function(x) { return iterate(x, 1); };
}
print((repeated(square, 2))(5));                  // 625

// Exercise 1.44 ('n-fold-smooth' not implemented)
function smooth(f, dx) {
   return function(x) { return average(x, (f(x-dx) + f(x) + f(x+dx)) / 3.0); };
}
print(fixed_point(smooth(function(x) { return Math.log(1000.0) / Math.log(x); }, 0.05), 1.5));

// Exercise 1.45 - unfinished

// Exercise 1.46 ('sqrt' not implemented)
function iterative_improve(good_enough, improve) {
   function iterate_(guess) {
      var next = improve(guess);
      if (good_enough(guess, next))
         return next;
      else
         return iterate_(next);
   }
   return function(x) { return iterate_(x); };
}
function fixed_point_(f, first_guess) {
   var tolerance = 0.00001;
   function good_enough(v1, v2) {
      return Math.abs(v1 - v2) < tolerance;
   }
   return (iterative_improve(good_enough, f))(first_guess);
}
print(fixed_point_(average_damp(function(x) { return Math.log(1000.0) / Math.log(x); }), 1.5));

// Note:  Must be careful using lexical scoping in V.
// The following will print "after"
var vx = "before";
function fv() { return vx; }
var vx = "after";
print (fv());

// The following will print "second" two times
function fg() { return "first"; }
print(fg());
function fg() { return "second"; }
print(fg());

