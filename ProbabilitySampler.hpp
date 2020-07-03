#ifndef PROBABILITY_SAMPLER_HPP
#define PROBABILITY_SAMPLER_HPP

#include "BinnedPDF.hpp"
#include "PiecewiseLinearFunction.hpp"

#include <array>
#include <cassert>
#include <random>
#include <type_traits>

// ============================================================================

namespace {

// Get the RNG based on the type of Float
template <typename Float_t>
auto get_rng_engine_();

// single-precision
template <>
auto get_rng_engine_<float>() {
    std::random_device rd;
    std::mt19937 gen(rd());
    return gen;
}

// double-precision
template <>
auto get_rng_engine_<double>() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    return gen;
}

} // end namespace

// ============================================================================

template <
    bool deposit_all,
    typename Float,
    typename std::size_t N_SUM,
    typename std::size_t N_BINS>
class ProbabilitySampler {

    // ------------------------------------------------------------------------
    // Constructors

public:

    // Default-constructs the object (mostly for testing).
    //ProbabilitySampler() = default;

    // Takes inverse CDF
    template <typename Function>
    ProbabilitySampler(Function && inverse_cdf)
        : inverse_cdf_(inverse_cdf)
        , uniform_(Float{0}, Float{1})
    {
    }

    // ------------------------------------------------------------------------
    // Continue condition -- how long to keep sampling?

private:

    bool continue_condition_() {
        constexpr int N_ITER = 1000000;
        return pdf_.count() < N_ITER;
    }

    // ------------------------------------------------------------------------
    // Initialize / reset the random number generator data

private:

    auto set_up_rng_() {
        engine_ = get_rng_engine_<Float>();
        uniform_.reset();
    }

    // ------------------------------------------------------------------------
    // Generate a random number drawn from the input distribution

private:

    auto generate_random_number() {
        auto y = uniform_(engine_);
        return inverse_cdf_(y);
    }

    // ------------------------------------------------------------------------
    // Generate N_SUM random numbers a normalize their sum to one

private:

    auto generate_normalized_values_() {
        std::array<Float, N_SUM> values;
        Float sum{0};
        for (auto & x : values) {
            x = generate_random_number();
            sum += x;
        }
        // If N_SUM == 1, we're just testing the how sampling from the input
        // distribution gets mapped to the output distribution without any
        // transformations.  This shows if the sampling process itself is
        // flawed.
        if constexpr (N_SUM > 1) {
            Float denom = Float{1} / sum;
            for (auto & x : values) {
                x *= denom;
            }
        }
        if constexpr (deposit_all) {
            return values;
        } else {
            return values[0];
        }
    }

    // ------------------------------------------------------------------------
    // Deposit the normalized random number(s) to the output PDF

private:

    template <typename Value>
    void deposit_values_(Value && v) {
        if constexpr (deposit_all) {
            for (auto & x : v) {
                pdf_.deposit(x);
            }
        } else {
            pdf_.deposit(v);
        }
    }

    // ------------------------------------------------------------------------
    // Generate the output distribution from the input distribution

public:

    void generate() {
        // Zero out PDF
        pdf_.clear();
        // Set up random number generator
        set_up_rng_();
        // Sampling loop
        while (continue_condition_()) {
            // TODO: parallelize?
            auto values = generate_normalized_values_();
            deposit_values_(std::move(values));
        }
    }

    // ------------------------------------------------------------------------
    // Get the PDF

public:

    auto & get_pdf() {
        return pdf_;
    }

    // ------------------------------------------------------------------------
    // Get the bin edges
    // TODO: This should be an operation of the BinnedPDF class.

public:

    auto get_bin_edges() {
        std::array<Float, N_BINS+1> edges;
        for (int n = 0; n <= N_BINS; n++) {
            edges[n] = Float(n) / Float(N_BINS);
        }
        return edges;
    }

    // ------------------------------------------------------------------------
    // Get the bin centers
    // TODO: This should be an operation of the BinnedPDF class.

public:

    auto get_bin_centers() {
        std::array<Float, N_BINS> centers;
        for (int n = 0; n < N_BINS; n++) {
            centers[n] = (Float(n) + Float{0.5}) / Float(N_BINS);
        }
        return centers;
    }

    // ------------------------------------------------------------------------
    // Private data

private:

    // Inverse CDF of input distribution
    PiecewiseLinearFunction<Float, N_BINS> inverse_cdf_;

    // PDF of output distribution
    BinnedPDF<Float, int, N_BINS> pdf_;

    // Random number generator data
    decltype(get_rng_engine_<Float>()) engine_;
    std::uniform_real_distribution<Float> uniform_;

    // ------------------------------------------------------------------------
    // Notes

    // Adjust by fraction (in case optimizer can constrain values)
    // Adjust by angle (calls adjust by fraction)
    // -- Actually I probably have those names backwards?

    // Termination condition
    // -- At first, probably just a fixed count
    // -- More elaborate:
    //    -- We know we want the output distribution to be a uniform
    //       distribution, and that we're using uniform binning, so we can
    //       simply constrain it on the number of counts in each bin in order
    //       to have sufficient statistics
    //    -- If we have a bad initial guess, then the output distribution may
    //       be skewed far from a uniform distribution, so we may also want a
    //       condition that checks that we're getting a very bad answer and
    //       stops early
    //       -- This may not be ideal if we're running this through an
    //          optimizer.  But if it only gives rough corrections, that might
    //          be enough to get close and let the closer answer give a better
    //          set of corrections.  If we can do that, then this will
    //          accelerate the process.

    // Get output PDF

    // May introduce an operator() that meets the needs of the optimizer.

    // For the optimizer, the error should be computed on the output PDF (not
    // the output CDF or the output inverse CDF).  The desired output PDF is a
    // uniform PDF, and we're using uniform binning, so each bin should have
    // the same number of samples in it.  Thus we get a uniform error.  If we
    // used the CDF, a higher value literally means more counts, which means
    // better statistics, so whichever side we begin integrating from has worse
    // statistics than the opposite side.

};

#endif // PROBABILITY_SAMPLER_HPP
