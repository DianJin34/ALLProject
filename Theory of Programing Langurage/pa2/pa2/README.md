## Due Dates

Early turn-in: September 21, 23:59
Hard turn-in: September 24, 23:59

## Getting started

Once again you will need Node for this assignment.
Follow instructions in PA1 for setup.

## Assignments

- Module system (`module` directory): implement various parts of a module
  system. In this problem you will implement various components that will be
  together used to build a module `require()`-function that can be used to load
  JavaScript Node.js modules.

  To solve this problem: read the problem description and instructions given
  `module/require.js`.

We have removed the solutions and included a comment `/**
<FILL-IN> **/` where you are expected to fill in your answers. (You'll want to
remove the `undefined` values appropriately.) You should not need to add code
anywhere outside these comment blocks.

### Testing your solution

We have included simple tests in the files themselves and/or (in the module
system problem, specifically) in separate problems. In the module system
problem, we have also included some examples in the `examples` subdirectory.
You can run the code by with the Node.js CLI tool:

For example:

```bash
node module/tests/require.js
```

If you don't get an assertion error, you are likely set. You may, of course,
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

The module assignments is worth 50 points.

#### Submission

You must submit the assignment via gradescope. 
Submit your assignment by uploading the following files:

- module/builtins.js
- module/helpers.js
- module/require-json.js


**NOTE:** Upload only these .js files, not a zip file containing your whole
project directry, not fewer files, not more files.
