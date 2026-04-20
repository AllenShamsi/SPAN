#include "ButterworthFilter.h"

#include <cmath>
#include <algorithm>
#include <complex>
#include <limits>
#include <Eigen/Dense>

ButterworthFilter::ButterworthFilter(double cutoff, double sampleRate, int order)
    : cutoff(cutoff), sampleRate(sampleRate), order(order) {
    calculateCoefficients();
}

// Function to round a number to 4 decimal places
double roundToFourDecimalPlaces(double value) {
    return std::round(value * 10000.0) / 10000.0;
}

void ButterworthFilter::calculateCoefficients() {
    int n = order;
    double Wc = 2 * sampleRate * tan(M_PI * cutoff / sampleRate);
    double T = 2 * sampleRate;

    std::vector<std::complex<double>> poles;
    for (int k = 1; k <= n; ++k) {
        double theta = (2 * k + n - 1) * M_PI / (2 * n);
        std::complex<double> pole = Wc * std::exp(std::complex<double>(0, theta));
        poles.push_back(pole);
    }

    std::vector<std::complex<double>> digital_poles;
    for (const auto& p : poles) {
        digital_poles.push_back((T + p) / (T - p));
    }

    // Compute the denominator coefficients (A)
    Eigen::VectorXd A(order + 1);
    std::vector<std::complex<double>> poly_coeffs = {1.0};
    for (const auto& p : digital_poles) {
        std::vector<std::complex<double>> temp = poly_coeffs;
        for (auto& coeff : poly_coeffs) {
            coeff *= -p;
        }
        poly_coeffs.insert(poly_coeffs.begin(), 0);
        for (size_t i = 0; i < temp.size(); ++i) {
            poly_coeffs[i] += temp[i];
        }
    }
    for (size_t i = 0; i < poly_coeffs.size(); ++i) {
        A[i] = roundToFourDecimalPlaces(poly_coeffs[i].real());
    }

    // Compute the numerator coefficients (B)
    Eigen::VectorXd B = Eigen::VectorXd::Ones(1);
    for (int k = 0; k < order; ++k) {
        Eigen::VectorXd temp = Eigen::VectorXd::Zero(B.size() + 1);
        for (int i = 0; i < B.size(); ++i) {
            temp[i] += B[i];
            temp[i + 1] += B[i];
        }
        B = temp;
    }

    // Normalize the filter coefficients
    double sumA = A.sum();
    double sumB = B.sum();
    B = B * (sumA / sumB);

    // Round the coefficients in B
    for (int i = 0; i < B.size(); ++i) {
        B[i] = roundToFourDecimalPlaces(B[i]);
    }

    a = A;
    b = B;
}



std::vector<double> ButterworthFilter::filter(const std::vector<double> &data) {
    int n = data.size();
    int nb = b.size();      // Number of numerator coefficients
    int na = a.size();      // Number of denominator coefficients
    int pad_len = n;

    // Create padded data: original data in the middle, reversed copies before and after
    std::vector<double> padded_data(3 * n);
    std::copy(data.begin(), data.end(), padded_data.begin() + n);
    std::reverse_copy(data.begin(), data.end(), padded_data.begin());
    std::reverse_copy(data.begin(), data.end(), padded_data.begin() + 2 * n);

    std::vector<double> filtered_data(3 * n, 0.0);
    // Initialize the filter state vector of length (na - 1) = (nb - 1)
    std::vector<double> zi(na - 1, 0.0);

    // First pass: forward filtering
    for (int i = 0; i < 3 * n; i++) {
        filtered_data[i] = b(0) * padded_data[i] + zi[0];
        for (int j = 1; j < nb; j++) {
            // Use zi[j] if it exists; otherwise (for j == nb-1) use 0.0.
            double z_next = (j < nb - 1) ? zi[j] : 0.0;
            zi[j - 1] = b(j) * padded_data[i] + z_next - a(j) * filtered_data[i];
        }
    }

    // Reverse the filtered data for reverse filtering
    std::reverse(filtered_data.begin(), filtered_data.end());
    std::fill(zi.begin(), zi.end(), 0.0);

    // Second pass: reverse filtering
    for (int i = 0; i < 3 * n; i++) {
        filtered_data[i] = b(0) * filtered_data[i] + zi[0];
        for (int j = 1; j < nb; j++) {
            double z_next = (j < nb - 1) ? zi[j] : 0.0;
            zi[j - 1] = b(j) * filtered_data[i] + z_next - a(j) * filtered_data[i];
        }
    }
    std::reverse(filtered_data.begin(), filtered_data.end());

    // Extract the central part which corresponds to the filtered original data
    std::vector<double> result(filtered_data.begin() + pad_len, filtered_data.begin() + 2 * pad_len);

    return result;
}



