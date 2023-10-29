## Due Dates

Early turn-in: September 14, 23:59
Hard turn-in: September 17, 23:59

## Background information

For this assignment, we will use immutable lists. You should look at the
following docs to learn more about them:

https://immutable-js.com/docs/latest@main/List/

You don't need to understand this library in detail, but do need to understand
how to create lists and manipulate them. We are using this library instead of
JavaScript's built-in arrays because they are conceptually simpler. You will
not mutate lists, all functions on lists that return lists will return new
lists.

## Getting started

First download and install [Node.js](https://nodejs.org/en/). 
Either the LTS or Current versions are fine for this course.

You will need to install the immutable library (see above). To do this you
should run `npm install` in the top-level assignment directory. This will
install the dependency.

## Assignments

- Quicksort (`quick.js`): implement quicksort in a functional programming
  style. This problem asks that you implement various helper functions
  that do most of the heavy lifting (`fold_left`, `map`, etc.) for the actual
  quicksort implementation.

  To solve this problem: read and modify `quick.js` as instructed there.

- Extra credit (`extra.js`): various short exercises.  The point of the short
  exercises is to get you more familiar with immutable lists and high-order
  functions. We strongly recommend that you do these exercises, if only to test
  some of the things you implemented for the quicksort problem.

  This problem is optional, but any extra credits can be used as points towards
  mistakes you make in the other 2 problems.

  To solve this problem: read and modify `extra.js` as instructed there.

In all both files, we have removed the solutions and included a comment `/**
<FILL-IN> **/` where you are expected to fill in your answers. (You'll want to
remove the `undefined` values appropriately.) You should not need to add code
anywhere outside these comment blocks.

### Testing your solution

We have included simple tests in the files themselves. 
You can run the code by with the Node.js CLI tool:

For example:

```bash
node quick.js
```

Node will raise an exception due to the `assert` at the bottom of the file.
This is how we are testing your code locally. Once you work through the
assignment, you will stop receiving assertion errors.
If you don't get an assertion error, you are likely done. You may, of course,
want to add more tests.

#### No-nos

- In all cases we are exporting the answers (e.g., `exports.fold_left = ...`),
  please do not remove any of this code. If we can't test your code, you lose
  points.

- We will be loading your modules with `require()`. Please make sure that your
  modules can be loaded -- test them by loading them and checking to make sure
  `require()` doesn't fail (by throwing an exception). This is especially
  important if you don't solve everything. We will not be fixing your files to
  ensure they can be loaded.

#### Grading

The quicksort is worth 50 points. Each extra credit question is worth 2 points.

#### Submission

You must submit the assignment via gradescope. 
Submit your assignment by uploading the following files:

- quick.js
- extra.js

**NOTE:** Upload only these .js files, not a zip file containing your whole
project directry, not fewer files, not more files.
