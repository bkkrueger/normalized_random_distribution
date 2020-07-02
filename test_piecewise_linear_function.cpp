#include "PiecewiseLinearFunction.hpp"

#include "check_macro.hpp"

#include <iostream>

template <int N>
void test() {
    std::cout << "block " << N << " ------------------------" << std::endl;
    using Float = double;

    std::array<Float,N> arr;
    for (int n = 1; n < N; n++) {
        arr[n] = Float(n) / Float{N};
    }

    constexpr int N_BINS = 8;
    PiecewiseLinearFunction<Float, N_BINS> func(arr);

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
