#include "ProbabilitySampler.hpp"

template <int N>
void test() {
    std::array<double,N> arr;
    double x{0.5};
    for (auto & element : arr) {
        element = x / double{N};
        x++;
    }
    ProbabilitySampler<false,double,3,8> ps(arr);
    if constexpr (N == 7) {
        ps.generate();
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
