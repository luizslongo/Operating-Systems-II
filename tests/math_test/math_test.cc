// EPOS Vector Utility Test Program

#include <utility/math.h>
#include <utility/random.h>

using namespace EPOS;

const int N = 10;

OStream cout;

int main()
{

    cout << "Math Utility Test\n" << endl;

    cout << "\n  -> Math::pi() = " << pi() << endl;
    cout << "\n  -> Math::e() = " << e() << endl;

    cout << "\n  -> Random::random():" << endl;
    for(int i = 0; i < N; i++)
        cout << "     = " << Random::random() << endl;

    cout << "\n  -> Math::pow<long>(x, y):" << endl;
    for(unsigned int y = 0; y < N; y++) {
        long x = Random::random() % 100;
        cout << "     pow(" << x << ", " << y << ") = " << pow(x, y) << endl;
    }

    cout << "\n  -> Math::pow<long long>(x, y):" << endl;
    for(unsigned int y = 0; y < N; y++) {
        long long x = Random::random() % 100;
        cout << "     pow(" << x << ", " << y << ") = " << pow(x, y) << endl;
    }

    cout << "\n  -> Math::pow<float>(x, y):" << endl;
    for(unsigned int y = 0; y < 3; y++) {
        float x = Random::random() % 100;
        cout << "     pow(" << x << ", " << y << ") = " << pow(x, y) << endl;
    }

    cout << "\n  -> Math::pow<double>(x, y):" << endl;
    for(unsigned int y = 0; y < 3; y++) {
        double x = Random::random() % 100;
        cout << "     pow(" << x << ", " << y << ") = " << pow(x, y) << endl;
    }

    cout << "\n  -> Math::sqrt<long>(x):" << endl;
    for(unsigned int y = 0; y < N; y++) {
        long x = abs(Random::random() % 100);
        cout << "     sqrt(" << x << ") = " << sqrt(x) << endl;
    }

    cout << "\n  -> Math::sqrt<long long>(x):" << endl;
    for(unsigned int y = 0; y < N; y++) {
        long long x = abs(Random::random() % 100);
        cout << "     sqrt(" << x << ") = " << sqrt(x) << endl;
    }

    cout << "\n  -> Math::sqrt<long>(pow<long>(x, 2)):" << endl;
    for(unsigned int y = 0; y < N; y++) {
        long x = Random::random() % 100;
        cout << "     sqrt(pow(" << x << ", " << 2 << ") = " << sqrt(pow(x, 2)) << endl;
    }

    cout << "\n  -> Math::sqrt<long long>(pow<long long>(x, 2)):" << endl;
    for(unsigned int y = 0; y < N; y++) {
        long long x = Random::random() % 100;
        cout << "     sqrt(pow(" << x << ", " << 2 << ") = " << sqrt(pow(x, 2)) << endl;
    }

    cout << "\n  -> Math::sin<float>(x):" << endl;
    for(unsigned int y = 0; y < N; y++) {
        float x = abs(Random::random()) % 360;
        cout << "     sin(" << x << ") = " << sin(deg2rad(x)) << endl;
    }

    cout << "\n  -> Math::sin<double>(x):" << endl;
    for(unsigned int y = 0; y < N; y++) {
        double x = abs(Random::random()) % 360;
        cout << "     sin(" << x << ") = " << sin(deg2rad(x)) << endl;
    }

    cout << "\n  -> Math::cos<float>(x):" << endl;
    for(unsigned int y = 0; y < N; y++) {
        float x = abs(Random::random()) % 360;
        cout << "     cos(" << x << ") = " << cos(deg2rad(x)) << endl;
    }

    cout << "\n  -> Math::cos<double>(x):" << endl;
    for(unsigned int y = 0; y < N; y++) {
        double x = abs(Random::random()) % 360;
        cout << "     cos(" << x << ") = " << cos(deg2rad(x)) << endl;
    }

    cout << "\n  -> Math::pow<float>(sin<float>(x), 2) + pow<float>(cos<float>(x), 2):" << endl;
    for(unsigned int y = 0; y < N; y++) {
        float x = abs(Random::random()) % 360;
        cout << "     pow(sin(" << x << "), 2) + pow(cos(" << x << "), 2) = " << pow(sin(deg2rad(x)), 2) + pow(cos(deg2rad(x)), 2) << endl;
    }

    cout << "\n  -> Math::pow<double>(sin<double>(x), 2) + pow<double>(cos<double>(x), 2):" << endl;
    for(unsigned int y = 0; y < N; y++) {
        double x = abs(Random::random()) % 360;
        cout << "     pow(sin(" << x << "), 2) + pow(cos(" << x << "), 2) = " << pow(sin(deg2rad(x)), 2) + pow(cos(deg2rad(x)), 2) << endl;
    }

    cout << "\nDone!" << endl;

    return 0;
}
