"use strict";
const lazy = require('./lazy.js');
const {delay, enumFrom, zipWith, map, tail, cons} = lazy;

// In this lab, you will use JavaScript's first class functions and
// closures to implement Haskell-style lazy evaluation (call-by-need).
//
// You can run this file by running the following command:
//
//      node lazy.js
//
// In the beginning, you should get an exception on the enumFrom test.

// ----------------------------------------------------------------- //
//                          Lazy Evaluation
// ----------------------------------------------------------------- //

// There are two basic properties about lazy evaluation.
//
//  1. When an expression is 'delay'ed, it is not evaluated immediately;
//  instead we return a *thunk* which evaluates the expression when
//  it is 'force'd (i.e., when we need it).
//
//  2. Once a delayed expression is forced, it is never evaluated
//  again: the result is cached and returned directly if the thunk
//  is forced again.
//
// Here is a simple example of creating a thunk and then forcing it
// twice.  Notice that the expression we pass to delay has to be written
// in a function() { ... } expression, since otherwise JavaScript will
// eagerly evaluate it!

console.log("== TEST: Memoization ==");

let t = delay(function() { console.log("Call me once"); return 2; });
console.log("1st force");
console.log(force(t));
console.log("2nd force");
console.log(force(t));

function force(t) {
    return t();
}

// ----------------------------------------------------------------- //
//                          Streams
// ----------------------------------------------------------------- //

// With delay and force, we can implement infinite streams of
// numbers.  A stream is simply a thunk that, when forced, gives
// you an object containing 'head', the head element of the
// stream, and 'tail', a thunk which can be forced to give
// you the rest of the stream.  Here are two example functions:

// 'repeat' accepts an element n, and creates a stream of that
// element repeated, e.g., [n, n, ...].

function repeat(n) {
  return delay(function() {
    return { 
      head: n,
      tail: repeat(n) 
    };
  });
}

// 'take' accepts a number n and a stream, and returns a list (actually, a
// JavaScript array) of the first n elements of the stream.  If you want to see
// what a stream looks like, we suggest you 'take' some elements and print them
// to the console.

function take(n, thunk_xs) {
  let r = [];
  for (let i = 0; i < n; i++) {
    let xs = force(thunk_xs);
    r.push(xs.head);
    thunk_xs = xs.tail;
  }
  return r;
}

console.log("== TEST: repeat ==");
console.log(take(5, repeat(2)));


console.log("== TEST: enumFrom ==");
console.log(take(5, enumFrom(2)));


console.log("== TEST: map ==");
let xs = map(function(x) {console.log("Multiplying " + x); return x * 2}, enumFrom(4));
console.log("Taking... (this should happen before any Multiplying)");
console.log(take(5, xs));

console.log("== TEST: zipWith ==");
console.log(take(5, zipWith(function(x,y) {return x > y},
                            enumFrom(8),
                            map(function(x) {return x * x}, enumFrom(1)))));


console.log("== TEST: tail ==");
console.log(take(5, tail(enumFrom(2))));

console.log("== TEST: cons ==");
console.log(take(5, cons(1, enumFrom(2))));