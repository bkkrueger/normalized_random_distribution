#include "BinnedPDF.hpp"

#include "check_macro.hpp"

#include <iostream>

int main() {
    using Float = double;
    using Integer = int;
    constexpr int N_BINS = 8;

    BinnedPDF<Float, Integer, N_BINS> pdf;

    std::cout << "block 1 ------------------------" << std::endl;
    CHECK(pdf.count() == 0, "count == 0");
    for (int n = 0; n < N_BINS; n++) {
        Float x = (Float(n) + Float{0.5}) / Float{N_BINS};
        pdf.deposit(x);
    }
    CHECK(pdf.count() == N_BINS, "count == N_BINS");

    auto bins = pdf.get_all_bins();
    for (auto & bin : bins) {
        CHECK(bin == 1, "bin == 1");
    }

    std::cout << "block 2 ------------------------" << std::endl;
    pdf.clear();
    CHECK(pdf.count() == 0, "count == 0");
    for (int n = 0; n < N_BINS; n++) {
        Float x = (Float(n) + Float{0.5}) / Float{N_BINS};
        for (int m = 0; m < n; m++) {
            pdf.deposit(x);
        }
    }
    CHECK(pdf.count() == N_BINS * (N_BINS - 1) / 2, "count == N(N-1)/2");

    for (int n = 0; n < N_BINS; n++) {
        CHECK(pdf.get_bin(n) == n, "bin == n");
    }
}
