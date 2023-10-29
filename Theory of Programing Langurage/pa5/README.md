# Running Haskell Locally

You should install the [Haskell Platform](https://www.haskell.org/ghcup/).
We will be using `cabal` to test our code, so after installing, make sure that you can run

```shell
cabal
```

in command line without issue. This will be the sign you are ready to code.


## Overview

This assignment will test your understanding of type classes and polymorphishm.

Recall that the problems require relatively little code ranging from 2 to 15
lines.  If any function requires more than that, you can be sure that you need
to rethink your solution.

1. [lib/TypeClasses.hs](/lib/TypeClasses.hs) has skeleton functions with missing bodies
   that you will fill in,
2. [test/Test.hs](/test/Test.hs) has some sample tests, and testing code that
   you will use to check your assignments before submitting.

You should only need to modify the parts of the files which say:

```haskell
error "TBD: ..."
```

with suitable Haskell code.

**NOTE:** Start early, to avoid any unexpected shocks late in the process.


#### Grading

We have included the points-worth of each question in the comments.

#### Submission

You must submit the assignment via gradescope. 
Submit your assignment by uploading the following files only:

- TypeClasses.hs

Unlike our first assignment, the autograder will not look for your files or
otherwise fix your bugs. You may only upload these files and you must ensure
they can be loaded.


## Assignment Testing and Evaluation

Most of the points, will be awarded automatically, by **evaluating your
functions against a given test suite**.

[Test.hs](/test/Test.hs) contains a very small suite of tests which gives you
a flavor of of these tests.  When you run

```shell
$ cabal test
```

Your last lines should have

```
All N tests passed (...)
OVERALL SCORE = ... / ...
```

**OR**

```
K out of N tests failed
OVERALL SCORE = ... / ...
```

**If your output does not have one of the above outputs your submission will receive a 0.**

If, for some problem, you cannot get the code to compile, leave it as is with
the `error ...` with your partial solution enclosed below as a comment.

The other lines will give you a readout for each test.
You are encouraged to try to understand the testing code, but you will not be
graded on this.