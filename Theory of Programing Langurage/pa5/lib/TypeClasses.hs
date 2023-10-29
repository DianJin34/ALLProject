{-# LANGUAGE ImportQualifiedPost #-}

module TypeClasses where

import Data.List (sortBy)
import Prelude hiding (Ord (..))
import Prelude qualified

-- In this lab, we will investigate explicit dictionary passing
-- style for the Ord typeclass, and in this setting investigate the
-- consequences of an extended form of type classes where instances can
-- be defined locally.  The answers in this section should be very
-- short.

-- In this lab, we'll assume the Ord type class is defined as follows:
--
--      data Ordering = LT | EQ | GT
--      class Ord a where
--          compare :: a -> a -> Ordering
--
-- (The real Ord type class is a bit more complicated, but we'll use
-- this approximation for the lab at hand.)
--
-- With this definition, we can write down the definition for a
-- dictionary as follows:

data Ord a = MkOrd {compare :: a -> a -> Ordering}

-- Int has a built-in Ord instance, using some built-in comparison
-- primitives.
--
--      instance Ord Int where
--          ...
--
-- We've provided the dictionary for it here:

dOrdInt :: Ord Int
dOrdInt = MkOrd Prelude.compare

-- We can also define a sorting function which uses our dictionary,
-- using the 'sortBy :: (a -> a -> Ordering) -> [a] -> [a]' function
-- provided by the standard library.

sort :: Ord a -> [a] -> [a]
sort d xs = sortBy (compare d) xs

-- Here's a list of Ints:

exIntList :: [Int]
exIntList = [2, 3, 1]

-- In standard Haskell, you would just write:
--
--      sort exIntList
--
-- Translate this to dictionary passing style.

exSortInt :: [Int]
-- <FILL-IN>
exSortInt = sort dOrdInt exIntList

-- </FILL-IN>

-- Pairs (a, b) also have an Ord instance. It looks like this:
--
--      instance (Ord a, Ord b) => Ord (a, b) where
--          compare (a1, b1) (a2, b2) =
--              case compare a1 a2 of
--                  EQ -> compare b1 b2
--                  r -> r
--
-- i.e., it sorts coordinates from x to y.
-- Please translate this instance into a dictionary.

dOrdXY :: Ord a -> Ord b -> Ord (a, b)
-- <FILL-IN>
dOrdXY ordA ordB = MkOrd (\(a1, b1) (a2, b2) ->
    case compare ordA a1 a2 of
        EQ -> compare ordB b1 b2
        r -> r
    )

-- </FILL-IN>

-- Here's a list of pairs of Ints:

exIntPairList :: [(Int, Int)]
exIntPairList = [(2, 1), (1, 2)]

-- In standard Haskell, you would just write:
--
--      sort exIntPairList
--
-- Translate this to dictionary passing style.

exSortIntPair :: [(Int, Int)]
-- <FILL-IN>
exSortIntPair = sort (dOrdXY dOrdInt dOrdInt) exIntPairList

-- </FILL-IN>

-- The previous ordering instance we gave is a bit arbitrary;
-- we could have instead defined it to sort by y-coordinate.
-- Write the dictionary for this version here (it should look
-- very similar to dOrdXY):

dOrdYX :: Ord a -> Ord b -> Ord (a, b)
-- <FILL-IN>
dOrdYX ordA ordB = MkOrd (\(a1, b1) (a2, b2) ->
    case compare ordB b1 b2 of
        EQ -> compare ordA a1 a2
        r -> r
    )

-- </FILL-IN>

-- With type-classes, it is not possible to define both of these
-- instances simultaneously.  We can see why if we look at some code
-- which wants to sort points:
--
--      sort [(1,2), (2,1)]
--
-- When we translate to dictionary style, do we use dOrdXY or dOrdYX?
-- Type-class resolution is TYPE directed, and since these dictionaries
-- both apply to the same types, we can't tell if we want one or the
-- other.
--
-- The problem is that when I define an instance in Haskell, it becomes
-- "globally" available: anywhere I have a type class constraint, I am
-- allowed to use the instance.  Unfortunately, with Ord it is a common
-- occurrence to want to use alternate instances for objects. Here is
-- another example: the default sorting order for integers is ascending;
-- however, you might also like to sort integers descending.  Here's an
-- alternate dictionary which reverses the sense of comparison:

dOrdIntDesc :: Ord Int
dOrdIntDesc = MkOrd $ \x y -> Prelude.compare y x

-- ... but you can't define an instance for both ascending and
-- descending comparison at the same time, for the very same reason.

-- An often proposed extension to type classes is to allow them to
-- be defined locally.  Here is some example (not actual Haskell!) syntax:
--
--      exSortIntDesc :: [Int]
--      exSortIntDesc =
--          let instance Ord Int where
--                  compare x y = Prelude.compare y x
--          in sort exIntList
--
-- This code produces the reverse-sorted exIntList.  Translate
-- this into dictionary passing style (it should look very familiar!):

exSortIntDesc :: [Int]
-- <FILL-IN>
exSortIntDesc = sort dOrdIntDesc exIntList

-- </FILL-IN>

-- Local instance declarations don't have any effect on runtime,
-- besides influencing what DICTIONARY is chosen to be filled in.

-- With local instances, we can even specialize the sort function:
--
--      sortIntDesc :: [Int] -> [Int]
--      sortIntDesc xs =
--          let instance Ord Int where
--                  compare x y = Prelude.compare y x
--          in sort xs
--
-- Translate this to dictionary passing style, using your preexisting
-- 'sort' function.  (This is simple, not a trick question!) (Hint:
-- first write down what the TYPE of the converted function should
-- be, and then fill in the implementation.)

-- BEGIN sortIntDesc (DO NOT DELETE THIS LINE)
sortIntDesc :: [Int] -> [Int]
-- <FILL-IN>
sortIntDesc xs = sort dOrdIntDesc xs

-- </FILL-IN>

-- However, local instances can give rise to some odd behavior.  This is
-- precisely why Haskell doesn't have them. For example, here is another
-- valid, "more general" type signature for sortIntDesc:
--
--      sortIntDesc' :: Ord a => [a] -> [a]
--      sortIntDesc' xs =
--          let instance Ord Int where
--                  compare x y = Prelude.compare y x
--          in sort xs
--
-- This code does in fact typecheck, but it does something
-- different from sortIntDesc.  Please translate it to dictionary
-- passing style.  (Hint: first write down what the TYPE of the
-- converted function should be, and then fill in the implementation.
-- Remember that class constraints get translated into dictionary
-- arguments.)

-- <FILL-IN>
-- sortIntDesc' :: ???
-- sortIntDesc' x = question "COMPLETE THE DEFINITION"
sortIntDesc' :: Ord a -> [Int] -> [Int]
sortIntDesc' _ xs =
  let dIntDict = MkOrd (flip Prelude.compare)
   in sort dIntDict xs

-- </FILL-IN>

-- In fact, with local instances, we lose "principal types", an
-- observation that was first made in Wadler and Blott.  The chosen
-- type of an expression can change the meaning of the expression,
-- which means that there is no most general choice: the choices
-- are incompatible and do different things.  This means we cannot
-- rely on type inference: with local instances, we are, in many cases,
-- REQUIRED to give explicit type signatures.

-- Here is another instance of strange behavior.  Suppose that we
-- have written a function isSorted:
--
--      isSorted :: Ord a -> [a] -> Bool
--      isSorted []  = True
--      isSorted [_] = True
--      isSorted (x:x':xs) =
--          case compare x x' of
--              GT -> False
--              _ -> isSorted (x':xs)
--
-- Convert this function to dictionary passing style.

-- <FILL-IN>
isSorted :: Ord a -> [a] -> Bool 
isSorted _ [] = True
isSorted _ [_] = True
isSorted dict (x:x':xs) = 
    case compare dict x x' of 
        GT -> False 
        _ -> isSorted dict (x':xs)

-- </FILL-IN>

-- Helper pedantic function (ignore)
question = error