#include "PiecewiseLinearFunction.hpp"

#include "check_macro.hpp"

#include <iostream>

template <int N>
void test() {
    std::cout << "block " << N << " ------------------------" << std::endl;
    using Float = double;

    std::array<Float,N> arr;
    for (int n = 0; n < N; n++) {
        arr[n] = Float(n+1) / Float{N+1};
    }

    constexpr int N_BINS = 8;
    PiecewiseLinearFunction<Float, N_BINS> func(arr);

    for (int n = 0; n < 100; n++) {
        Float x = Float(n) / Float(100);
        CHECK(x == func(x), "x == f(x)");
    }
}

int main() {
    test<1>();
    test<2>();
    test<3>();
    test<4>();
    test<5>();
    test<6>();
    test<7>();
    test<8>();
    test<9>();
}
