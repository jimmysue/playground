#include <cmath>
#include <iostream>
#include <random>

#include <Eigen/Dense>
#include <ceres/ceres.h>
#include <opencv2/core.hpp>

using namespace std;
using namespace Eigen;
double w_sigma = 1.0;
double inv_sigma = 1.0 / w_sigma;
struct ExponentialResidual {
    ExponentialResidual(double x, double y) : x_(x), y_(y) {}
    template <typename T>
    bool operator()(const T *const theta, T *redidual) const {
        redidual[0] = y_ - exp(theta[0] * x_ * x_ + theta[1] * x_ + theta[2]);
        return true;
    }

  private:
    const double x_; // observation;
    const double y_;
};

void solveByCeres(const std::vector<double> &xdata,
                  const std::vector<double> &ydata, double ae, double be,
                  double ce) {
    using namespace ceres;
    ceres::Problem problem;
    Vector3d theta;
    theta << ae, be, ce;
    for (int i = 0; i < xdata.size(); ++i) {
        ceres::CostFunction *cost_function =
            new AutoDiffCostFunction<ExponentialResidual, 1, 3>(
                new ExponentialResidual(xdata[i], ydata[i]));
        problem.AddResidualBlock(cost_function, nullptr, theta.data());
    }

    Solver::Options options;
    options.linear_solver_type = ceres::DENSE_QR;
    options.minimizer_progress_to_stdout = true;
    Solver::Summary summary;
    ceres::Solve(options, &problem, &summary);
    std::cout << summary.BriefReport() << "\n";
    std::cout << "estimations: " << theta.transpose() << std::endl;
}

void solveByScratch(const std::vector<double> &xdata,
                    const std::vector<double> &ydata, double ae, double be,
                    double ce) {
    // newton's method
    Vector3d theta;
    theta << ae, be, ce;
    const int N = xdata.size();
    int iter = 0;
    while (iter < 100) {
        iter++;
        Matrix3d H = Matrix3d::Zero(); // 3 paramaters;
        Vector3d Je = Vector3d::Zero();
        for (auto i = 0; i < N; ++i) {
            double x = xdata[i];
            double y = ydata[i];
            double yhat = std::exp(theta[0] * x * x + theta[1] * x + theta[2]);
            double e = y - yhat;
            Vector3d Ji;
            Ji << -yhat * x * x, -yhat * x, -yhat;

            Matrix3d Hi = Ji * Ji.transpose() * (inv_sigma * inv_sigma);
            H += Hi;
            Je -= Ji * inv_sigma * e;
        }

        // solve H * delta = Jb
        Vector3d delta = H.ldlt().solve(Je);
        if (!delta.allFinite()) {
            std::cerr << "出现异常" << std::endl;
            break;
        }

        theta += delta;
        double diff = delta.norm();
        std::cout << "迭代(" << iter << "diff: " << diff
                  << "): delta: " << delta.transpose() << std::endl;
        std::cout << "       参数值: " << theta.transpose() << std::endl;
        if (diff < 1e-6) {
            std::cout << "收敛" << std::endl;
            break;
        }
    }
    std::cout << "估计: " << theta.transpose() << std::endl;
}
int main() {
    double ar = 1.0, br = 2.0, cr = 1.0;  // ground truth
    double ae = 2.0, be = -1.0, ce = 5.0; // estimation
    int N = 100;
    double w_sigma = 1.0;
    double inv_sigma = 1.0 / w_sigma;
    cv::RNG rng;

    vector<double> x_data, y_data; // data points

    for (auto i = 0; i < N; ++i) {
        double x = i / 100.0;
        x_data.push_back(x);
        y_data.push_back(std::exp(ar * x * x + br * x + cr) +
                         rng.gaussian(w_sigma * w_sigma));
    }

    solveByScratch(x_data, y_data, ae, be, ce);
    solveByCeres(x_data, y_data, ae, be, ce);
}