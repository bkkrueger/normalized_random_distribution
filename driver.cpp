#include "PiecewiseLinearFunction.hpp"
#include "ProbabilitySampler.hpp"

#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

template<bool deposit_all>
void do_stuff() {

    using Float = double;
    constexpr int N_BINS{USER_N_BINS};
    constexpr int N_SUM{USER_N_SUM};

    std::array<Float,N_BINS-1> inverse_cdf_bins;
    Float denom = Float{1} / Float(N_BINS);
    for (int n = 0; n < inverse_cdf_bins.size(); n++) {
        inverse_cdf_bins[n] = Float(n+1) * denom;
    }

    PiecewiseLinearFunction<Float, N_BINS> inverse_cdf(inverse_cdf_bins);

    ProbabilitySampler<deposit_all, Float, N_SUM, N_BINS> sampler(inverse_cdf);

    auto x = sampler.get_bin_centers();
    sampler.generate();
    auto y = sampler.get_pdf().get_pdf();
    assert(x.size() == y.size());

    std::stringstream ss;
    ss << "pdf_" << (deposit_all ? "true" : "false") << ".txt";
    std::ofstream fout(ss.str());
    for (int n = 0; n < x.size(); n++) {
        fout << std::right << std::setw(6) << n;
        fout << "   ";
        fout << std::fixed << std::setw(8) << x[n];
        fout << "   ";
        fout << std::right << std::setw(9) << y[n];
        fout << std::endl;
    }

}


int main() {
    do_stuff<true>();
    do_stuff<false>();
}

