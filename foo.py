l = 0
r = 2.1

from math import log2

x = float(input())

while abs(r - l) >= 1e-3:
    mid = (l+r)/2
    y = log2(mid)
    
    if y > x:
        r = mid
    else:
        l = mid
        
print((l), 2**x)
