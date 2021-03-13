#include <cmath>
#include <iostream>
#include <random>

#include <Eigen/Dense>
#include <opencv2/core.hpp>

using namespace std;
using namespace Eigen;

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

    // newton's method
    Vector3d theta;
    theta << ae, be, ce;
    int iter = 0;
    while (iter < 100) {
        iter++;
        Matrix3d H = Matrix3d::Zero(); // 3 paramaters;
        Vector3d Je = Vector3d::Zero();
        for (auto i = 0; i < N; ++i) {
            double x = x_data[i];
            double y = y_data[i];
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

    std::cout << "真值: " << ar << ", " << br << ", " << cr << std::endl;
    std::cout << "估计: " << theta.transpose() << std::endl;
}