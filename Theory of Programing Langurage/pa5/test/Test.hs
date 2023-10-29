{-# LANGUAGE ExistentialQuantification #-}
{-# LANGUAGE ScopedTypeVariables #-}

import Data.Biapplicative (Biapplicative (bipure, (<<*>>)))
import Data.Bifoldable (Bifoldable (bifold))
import Data.Bifunctor (bimap)
import Data.Function (on)
import qualified Data.List as L
import Data.Maybe (fromMaybe)
import Data.Tuple (swap)
import Test.QuickCheck (Property, (===))
import Test.QuickCheck.Property (Property, (===))
import Test.Tasty
  ( TestTree,
    defaultIngredients,
    defaultMainWithIngredients,
    localOption,
    mkTimeout,
    testGroup,
  )
import Test.Tasty.HUnit (testCase, (@?=))
import Test.Tasty.Options (IsOption)
import Test.Tasty.QuickCheck
  ( QuickCheckMaxSize (QuickCheckMaxSize),
    QuickCheckTests (QuickCheckTests),
    testProperty,
  )
import Test.Tasty.Runners (listingTests)
import TypeClasses
  ( dOrdInt,
    dOrdXY,
    dOrdYX,
    exIntList,
    exIntPairList,
    exSortInt,
    exSortIntDesc,
    exSortIntPair,
    isSorted,
    sortIntDesc,
  )
import qualified TypeClasses as TC

data Option = forall a. IsOption a => Option a

withOptions :: [Option] -> TestTree -> TestTree
withOptions = flip (foldr (\(Option opt) -> localOption opt))

main :: IO ()
main = defaultMainWithIngredients defaultIngredients (withOptions opts tests)
  where
    opts =
      [ Option $ QuickCheckTests 250,
        Option $ QuickCheckMaxSize 10,
        Option $ mkTimeout 10000000 -- 10 seconds
      ]

tests :: TestTree
tests = testGroup "All tests" [typeclassProperties]

typeclassProperties =
  testGroup
    "Typeclasses"
    [ testCase "exSortInt = sort dOrdInt exIntList" $
        exSortInt @?= L.sort exIntList,
      testCase "exSortIntPair = sort (dOrdXY dOrdInt dOrdInt) exIntPairList" $
        exSortIntPair @?= L.sort exIntPairList,
      testCase "exSortIntDesc = sort dOrdIntDesc exIntList" $
        exSortIntDesc @?= L.reverse (L.sort exIntList),
      testProperty "dOrdXY compares correctly" prop_dOrdXY_correct,
      testProperty "dOrdYX compares correctly" prop_dOrdYX_correct,
      testProperty "sortIntDesc is correct" prop_sortIntDesc_correct,
      testProperty "isSorted is correct" prop_isSorted_correct
    ]

dOrdGeneric :: Ord a => TC.Ord a
dOrdGeneric = TC.MkOrd compare

compareTuple :: (Ord a, Ord b) => (a, b) -> (a, b) -> Ordering
compareTuple x y = bifold $ bipure compare compare <<*>> x <<*>> y

prop_dOrdXY_correct :: (Int, Bool) -> (Int, Bool) -> Property
prop_dOrdXY_correct p1 p2 = compareTuple p1 p2 === TC.compare (dOrdXY dOrdGeneric dOrdGeneric) p1 p2

prop_dOrdYX_correct :: (Int, Bool) -> (Int, Bool) -> Property
prop_dOrdYX_correct p1 p2 = compareTuple (swap p1) (swap p2) === TC.compare (dOrdYX dOrdGeneric dOrdGeneric) p1 p2

prop_sortIntDesc_correct :: [Int] -> Property
prop_sortIntDesc_correct xs = sortIntDesc xs === L.reverse (L.sort xs)

prop_isSorted_correct :: [Int] -> Property
prop_isSorted_correct xs = isSorted dOrdInt xs === (L.sort xs == xs)