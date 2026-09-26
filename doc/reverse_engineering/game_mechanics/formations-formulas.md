# Units formations

Military units can be arranged in various formations. Here's an overview on how this may be implemented

### Line formation

If you table the number of units in the first row, in the second row and so on accordingly to the number of units in the group, you'll find a pattern in the units number intervals for a given row count: namely, the length of the first interval `(0...6)` is `6`, the second `(7...15)` is `9 = 6+3 + 0`, the third `(16...28)` is `13 = (6+3) + 3+1`, the fourth (maybe) `(29...46)` is `18 = ((6+3) + 3+1 ) + 3+2`.

The value of `46` is chosen by observing that the difference between the last value for a certain row count and the first value for the following row count is always `3`; in this case, with `46` units `4` rows are formed: the first 3 with 12 units and the last with 10. For the next value, `47`, we guess `5` rows with `10, 10, 10, 10, 7`.

Of course, since there's a limit of 40 to the number of selectable units, there's unfortunately no way to check this guess.

##### Formulas

If we neglect the first interval `(0...6)` from above, we can define a recurrence equation between the others.
It looks like this:

``` python
def f(n):
    if n == 0:
    	return 9
    return f(n-1) + n + 3
```

In order to find the nth last value before the row count changes, we just need to sum f(i) from 0 up to n. Then, in order to get the row count for a specified numOfUnits, we can iteratively check which of the values is the upper bound of the interval where our numOfUnits lies:

``` python
def findRowCount(numOfUnits):
    # the last numOfUnits before the row count changes
    lastBeforeInc = 6  
    # iteration counter
    i = 0

    # for the first row
    if numOfUnits <= 6:
        return 1

    while numOfUnits > lastBeforeInc: # is numOfUnits greater than last bound?
        lastBeforeInc += f(i) # no, increase the bound
        i += 1

    rowCount = i + 1
    return (lastBeforeInc, rowCount)
```

Finally, we have to find the number of units for each row. The first row from the list we got from experimentation leads to the following formula:

``` python
rows[0] = math.ceil(numOfUnits / findRowCount(numOfUnits))
```

This seems to agree with experimental data and nicely extends to values over 40.
Rows other from the last one have the same value of the first. The last row is simply:

``` python
rows[rowCount - 1] = numOfUnits - (rowCount-1)*rows[0]
```

or, if you care about performance:

``` python
rows[rowCount - 1] = numOfUnits % (rowCount-1)
```

**Considerations** : for performance sake (or just out of curiosity), you may want to find a better, non recursive function for `f`. It turns out that the recurrence equation can be solved to

        f(n) = (18 + 7*n + n^2)/2

using Mathematica's

``` mathematica
        RSolve[{f[n] == f[n-1] + n + 3, f[0] == 9}, f[n], n]
``` 

We can separate `6 + f(0) + f(1) + f(2) + ... + f(n)` (the formula for `lastBeforeInc`) and make it a function of the upper bound `n`:

``` python
def findNthLastBeforeInc(n): 
    return (90 + 65*n + 12*n^2 + n^3)/6
``` 

The above is the result of Mathematica's:

``` mathematica
        6 + Sum[1/2 (18+7 i+i^2), {i, 0, n}]
``` 

The `while` loop in `findRowCount` simply becomes:

``` python
    while numOfUnits > lastBeforeInc: # numOfUnits greater than last bound?
        lastBeforeInc = findNthLastBeforeInc(i) # no, get the next bound
        i += 1
``` 

This allows us to get rid of the **O(n)** recursive function. However, it is a third-degree function.

The best would be to have an explicit function for `n`. This is easily achieved by imposing:

        findNthLastBeforeInc(n) + 1 <= numOfUnits < findNthLastBeforeInc(n + 1)
        
with some constraints over `numOfUnits` and `n`.
So, by doing:
``` mathematica
assumptions := 
 n ∈ Integers && numOfUnits > 6 && numOfUnits ∈ Integers && n >= 0;

Reduce[{findNthLastBeforeInc[n] + 1 <= numOfUnits < findNthLastBeforeInc[n + 1], assumptions}, n]
```
we find that (for `numOfUnits >= 16`), `n` must lie between 0 and the first root of a third-degree polynomial, namely:
``` mathematica
        Root[90 - 6 numOfUnits + 65 x + 12 x^2 + x^3 &, 1]
```
For `numOfUnits == 15`, `n` is `0`.
Since we're looking for natural values, we just need the `ceil()` of this function of `numOfUnits`.
The root can be shown explicitly using `ToRadicals[]`. Unfortunately, it turns out to be a very complex function, unsuitable for real-time computing. The plot of the values looks like that of a square-root-like function.
The function

``` 
        a*x^(1/6) + b*x^(1/3) + c*x + d
```

with parameters

``` mathematica
{a -> -7.622169906683245, 
b -> 3.6545500464975955, 
c -> -0.002901063925120857, 
d -> 4.129231118574412}
```

found using

``` mathematica
    FindFit[values, a x^(1/6) + b x^(1/3) + c*x + d, {a, b, c, d}, x]
```

may be a good choice.

Finally, we rewrite our `findRowCount` as

``` python
import math

def findRowCount(numOfUnits):
    if numOfUnits <= 6:
        return 1
    if 7 <= numOfUnits and numOfUnits <= 15:
        return 2

    a = -7.622169906683245
    b = 3.6545500464975955
    c = -0.002901063925120857
    d = 4.129231118574412
    approx = a*pow(numOfUnits, 1.0/6) + b*pow(numOfUnits, 1.0/3) + c*numOfUnits + d
    rowCount = math.ceil(approx) + 1
    return (approx, rowCount)
```

# Improved algorithm

If you look back at the experimental data, an educated guess may be that the points where the slopes change may be expressed out as `5*i - (i-1)`: for the n-th point we have 

``` mathematica
        Sum[(5 i - (i - 1)), {i, 1, n}] + 1
``` 

Mathematica solves this as 

``` mathematica
        1 + 3 n + 2 n^2
``` 

The first few values are `6, 15, 28, 45, 66`, which prove to be correct.

Now we must find where our `numOfUnits` is greater or equal to the above

``` mathematica
        Reduce[1 + 3 n + 2 n^2 <= N, n, Integers]
``` 

And of course we take the upper bound of the interval found, namely 

``` mathematica
        -(3/4) + 1/4 Sqrt[1 + 8 N]
``` 

We need the next integer, so we `Ceil` the above,

``` mathematica
        Ceiling[-(3/4) + 1/4 Sqrt[1 + 8 N]]
``` 

The expression found is the `rowCount` of above, in a saner form.

Again, the first row is

``` python
        rows[0] = math.ceil(numOfUnits / findRowCount(numOfUnits))
```

and the other rows are unchanged.

The final Python code may be:

``` python
import math

def findRowCount(numOfUnits):
    return math.ceil((math.sqrt(1 + 8*numOfUnits) - 3) / 4)
```

## Performance considerations

Turns out that the expression is equivalent to 

``` mathematica
    -Quotient[N, Quotient[3 - Sqrt[1+8*N], 4]]
``` 

which contains less divisions and `Quotient` is directly mapped to the `IDIV` Intel x86 instruction.

More, it is equal to

``` mathematica
    -Quotient[N, Quotient[3 - Sqrt[BitShiftLeft[N, 3]], 4]]
``` 

which not contains the addition and uses a left shift to do the multiplication, thus being faster.
Or one may use the following

``` mathematica
    -Quotient[N, Quotient[2 - Floor[Sqrt[BitShiftLeft[N, 3]]], 4]]
``` 
where `Floor[Sqrt[n]]` is the [integer square root function](https://en.wikipedia.org/wiki/Integer_square_root) and there exist plenty of optimized algorithms to do it.


## BONUS: Hackish stuff

The general formula is `M*i - (i-u)` with some constraints on `M` and `u`.
This gives us:

``` mathematica
        Ceiling[N / Ceiling[(1-M-2*u + Sqrt[(-1+M)*(-9+M+ 8*N) + 4*(-1+M) u+4 u^2]) / (2*(-1+M))]]
``` 

The constraints on N, M and u are `(N | M | u) ∈ ℤ && u ≥ 1 && M ≥ 2 && N ≥ 2`.
This is useful if we're not satisfied with the default game behavior :smile:
