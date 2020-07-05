#include "PiecewiseLinearFunction.hpp"
#include "ProbabilitySampler.hpp"

#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

template <typename Float>
auto func(Float x) {
    return Float{0.5} + (x < Float{0.5} ? 1 : -1) *
        (Float{2} * x * (Float{1} - x) - Float{0.5});
    constexpr Float k{-1};
    return x*(1 - k*(x - 1)*(x - 1/2));
    return (x*(6 + 6*k - 6*x - 3*k*x + 2*x*x))/(2 + 3*k);
    return x * (Float{2} - x);
    return x;
}

template<bool deposit_all>
void do_stuff() {

    using Float = double;
    constexpr int N_BINS{USER_N_BINS};
    constexpr int N_SUM{USER_N_SUM};

    std::array<Float,N_BINS-1> inverse_cdf_bins;
    using Function = PiecewiseLinearFunction<Float, N_BINS>;
    auto bin_edges = Function::get_bin_edges();
    for (int n = 0; n < inverse_cdf_bins.size(); n++) {
        inverse_cdf_bins[n] = func(bin_edges[n]);
    }

    Function inverse_cdf(inverse_cdf_bins);

    ProbabilitySampler<deposit_all, Float, N_SUM, N_BINS> sampler(inverse_cdf);

    auto pdf = sampler.generate();
    auto x = pdf.get_bin_centers();
    auto y = pdf.get_pdf();
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

