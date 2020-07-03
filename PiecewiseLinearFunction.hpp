#ifndef PIECEWISE_LINEAR_FUNCTION_HPP
#define PIECEWISE_LINEAR_FUNCTION_HPP

#include <array>
#include <cassert>
#include <type_traits>

// ============================================================================
// Constexpr functions for SFINAE

// Constexpr function that checks if this is the right number of points for an
// exact-resolution inverse CDF.
// -- If there are N bins, there will be N-1 points in the inverse CDF.
constexpr bool R_exact_(
        typename std::size_t const N_PTS,
        typename std::size_t const N_BINS) {
    return N_PTS + 1 == N_BINS;
}

// Constexpr function that checks if this is the right number of points for a
// half-resolution inverse CDF.
// -- If there are N bins, there will be N/2 bins in the half-resolution
//    inverse CDF, which means there will be N/2-1 points in the
//    half-resolution inverse CDF.
constexpr bool R_half_(
        typename std::size_t const N_PTS,
        typename std::size_t const N_BINS) {
    return 2 * (N_PTS + 1) == N_BINS;
}

// Constexpr function that checks if doesn't have the right number of points
// for either an exact-resolution or a half-resolution inverse CDF.
constexpr bool R_other_(
        typename std::size_t const N_PTS,
        typename std::size_t const N_BINS) {
    return !R_exact_(N_PTS,N_BINS) && !R_half_(N_PTS,N_BINS);
}

// ============================================================================

template <typename Float, typename std::size_t N_BINS>
class PiecewiseLinearFunction {

    // ------------------------------------------------------------------------
    // Private data

private:

    // Coefficients
    using CoefficientPair = std::array<Float, 2>;
    std::array<CoefficientPair, N_BINS> coefficients_;

    // Indices
    static constexpr int SLOPE_ = 0;
    static constexpr int INTERCEPT_ = 1;

    // ------------------------------------------------------------------------

    static Float bin_edge_(std::size_t const & n) {
        assert(n >= 0);
        assert(n <= N_BINS);
        return Float(n) / Float{N_BINS};
    }

    // ------------------------------------------------------------------------

    void construct_coefficients_(std::array<Float, N_BINS+1> const & points) {
        assert(points.front() == 0);
        for (int n = 0; n < N_BINS; n++) {
            assert(points[n] <= points[n+1]);
        }
        assert(points.back() == 1);
        Float x0, y0;
        Float x1{bin_edge_(0)};
        Float y1{points[0]};
        for (int n = 0; n < N_BINS; n++) {
            // Cycle points
            x0 = x1;
            y0 = y1;
            // Fetch new points
            x1 = bin_edge_(n+1);
            y1 = points[n+1];
            // Compute coefficients for y = m x + b
            Float m = (y1 - y0) / (x1 - x0);
            Float b = y1 - m * x1;
            coefficients_[n][SLOPE_] = m;
            coefficients_[n][INTERCEPT_] = b;
        }
    }

    // ------------------------------------------------------------------------
    // Constructors

public:

    // Default-constructs the object (mostly for testing).
    PiecewiseLinearFunction() = default;

    // Takes the exact-resolution bin-edge function values
    // -- Assumes uniform binning and implies the points (0,0) and (1,1).
    template <typename std::size_t N_PTS,
        typename std::enable_if<R_exact_(N_PTS,N_BINS), bool>::type = true> 
    PiecewiseLinearFunction(std::array<Float,N_PTS> const & points_in) {
        std::array<Float, N_BINS+1> all_points;
        all_points[0] = Float{0};
        for (int n = 0; n < N_PTS; n++) {
            all_points[n+1] = points_in[n];
        }
        all_points[N_BINS] = Float{1};
        construct_coefficients_(all_points);
    }

    // Takes half-resolution bin-edge function values
    // -- Assumes uniform binning and implying the points (0,0) and (1,1).
    // -- Linearly interpolates the missing points.
    template <typename std::size_t N_PTS,
        typename std::enable_if<R_half_(N_PTS,N_BINS), bool>::type = true> 
    PiecewiseLinearFunction(std::array<Float,N_PTS> const & points_in) {
        std::array<Float, N_BINS+1> all_points;
        all_points[0] = Float{0};
        for (int n = 0; n < N_PTS; n++) {
            auto & y1 = all_points[2*n];
            auto & y2 = points_in[n];
            all_points[2*n+1] = Float{0.5} * (y1 + y2);
            all_points[2*n+2] = y2;
        }
        all_points[N_BINS-1] = Float{0.5} * (points_in.back() + Float{1});
        all_points[N_BINS] = Float{1};
        construct_coefficients_(all_points);
    }

    // Takes any other resolution, bin-edge function values
    // -- Assumes uniform binning and implying the points (0,0) and (1,1).
    // -- Linearly interpolates all points.
    // -- If the previous constraints fail, SFINAE will cause the program to
    //    fall back to this constructor.
    template <typename std::size_t N_PTS,
        typename std::enable_if<R_other_(N_PTS,N_BINS), bool>::type = true> 
    PiecewiseLinearFunction(std::array<Float,N_PTS> const & points_in) {
        PiecewiseLinearFunction<Float,N_PTS+1> temp_func(points_in);

        std::array<Float, N_BINS+1> all_points;
        all_points.front() = Float{0};
        for (int n = 1; n < N_BINS; n++) {
            auto x = bin_edge_(n);
            all_points[n] = temp_func(x);
        }
        all_points.back() = Float{1};

        // Construct coefficients off of the newly-interpolated points.
        construct_coefficients_(all_points);
    }

    // ------------------------------------------------------------------------
    // Get bin edges (helps with constructing function)

public:

    static auto get_bin_edges() {
        std::array<Float, N_BINS-1> bin_edges;
        for (int n = 0; n < N_BINS; n++) {
            bin_edges[n] = bin_edge_(n+1);
        }
        return bin_edges;
    }

    // ------------------------------------------------------------------------
    // Call operator to evaluate function

public:

    auto operator() (Float const & x) {
        assert(x >= Float{0});
        assert(x <  Float{1});
        std::size_t index = std::size_t(x * N_BINS);
        auto & coefs = coefficients_[index];
        auto & m = coefs[SLOPE_];
        auto & b = coefs[INTERCEPT_];
        return m * x + b;
    }

};

#endif // PIECEWISE_LINEAR_FUNCTION_HPP
