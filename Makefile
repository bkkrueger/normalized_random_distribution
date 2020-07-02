CC := /usr/local/Cellar/gcc/9.3.0_1/bin/g++-9

NBINS := 8
NSUM := 3
#VALUES = -D N_BINS=${NBINS} -D N_SUM=${NSUM}
VALUES = 

CPP_FLAGS = -std=c++17

all: driver test_binned_pdf test_piecewise_linear_function

driver: driver.cpp ProbabilitySampler.hpp PiecewiseLinearFunction.hpp BinnedPDF.hpp
	${CC} -o driver ${CPP_FLAGS} ${VALUES} driver.cpp

test_binned_pdf: test_binned_pdf.cpp BinnedPDF.hpp check_macro.hpp
	${CC} -o test_binned_pdf ${CPP_FLAGS} ${VALUES} test_binned_pdf.cpp

test_piecewise_linear_function: test_piecewise_linear_function.cpp PiecewiseLinearFunction.hpp check_macro.hpp
	${CC} -o test_piecewise_linear_function ${CPP_FLAGS} ${VALUES} test_piecewise_linear_function.cpp

