#ifndef BINNED_PDF_HPP
#define BINNED_PDF_HPP

#include <array>
#include <cassert>

// ============================================================================

template <typename Float, typename Integer, typename std::size_t N_BINS>
class BinnedPDF {

    // ------------------------------------------------------------------------
    // Private data

private:

    // PDF of output distribution
    std::array<Integer, N_BINS> pdf_;

    // Count
    Integer count_;

    // ------------------------------------------------------------------------
    // Constructors

public:

    // Default-constructs the object.
    BinnedPDF() {
        clear();
    }

    // ------------------------------------------------------------------------
    // Add a value to the PDF

public:

    // Deposit a value in the PDF.
    void deposit(Float const & x) {
        assert(x >= Float{0});
        assert(x < Float{1});
        std::size_t index = std::size_t(x * N_BINS);
        pdf_[index]++;
        count_++;
    }

    // ------------------------------------------------------------------------
    // Clear the PDF

public:

    void clear() {
        for (auto & x : pdf_) {
            x = Integer{0};
        }
        count_ = 0;
    }

    // ------------------------------------------------------------------------
    // How many samples have been deposited?

public:

    auto count() const {
        return count_;
    }

    // ------------------------------------------------------------------------
    // Get the PDF

public:

    auto get_pdf() {
        std::array<Float, N_BINS> pdf_norm;
        Float denom = Float{1} / Float(count_);
        for (int n = 0; n < N_BINS; n++) {
            pdf_norm[n] = pdf_[n] * denom;
        }
        return pdf_norm;
    }

    // ------------------------------------------------------------------------
    // Get the values in all bins

public:

    auto get_all_bins() {
        return pdf_;
    }

    // ------------------------------------------------------------------------
    // Get the values in a specified bin

public:

    auto get_bin(std::size_t const & index) {
        assert(index >= 0);
        assert(index < N_BINS);
        return pdf_[index];
    }

    // ------------------------------------------------------------------------
    // Get the bin edges

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

public:

    auto get_bin_centers() {
        std::array<Float, N_BINS> centers;
        for (int n = 0; n < N_BINS; n++) {
            centers[n] = (Float(n) + Float{0.5}) / Float(N_BINS);
        }
        return centers;
    }

};

#endif // BINNED_PDF_HPP
