#ifndef BUTTERWORTHFILTER_H
#define BUTTERWORTHFILTER_H

#include <vector>
#include <Eigen/Dense>

class ButterworthFilter
{
public:
    ButterworthFilter(double cutoff, double sampleRate, int order);

    std::vector<double> filter(const std::vector<double> &data);

private:
    void calculateCoefficients();

    double cutoff;
    double sampleRate;
    int order;
    Eigen::VectorXd b;
    Eigen::VectorXd a;
};

#endif // BUTTERWORTHFILTER_H
