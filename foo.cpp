#include<iostream>
#include<cmath>

using namespace std;

int main() {
        float l = 0, r = 2.1;
        float x;
        cin >> x;

        while (abs(r - l) >= 1e-3) {
                float mid = (l+r)/2;
                if (log2(mid) > x)
                        r = mid;
                else
                        l = mid;
        }
        cout << l << ' ' << pow(2, x) << '\n';
}
