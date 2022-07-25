#ifndef INTERPOLATION_H
#define INTERPOLATION_H

//#include <QGLWidset>
#include <cmath>
#include <cstdio>
#include <vector>

enum class interpolation_method {
    origin,
    bessel,
    error,
};

double func(int func_id, double x, double y);

class interpolation
{
  private:
    const double eps = 1e-14;

    double ax = 0.;
    double bx = 0.;
    double ay = 0.;
    double by = 0.;
    int nx = 0;
    int ny = 0;
    int func_id = 0;
    int disturb = 0;
    double Fmax;

    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> F;

    std::vector<double> Gx;
    std::vector<double> Gy;

    std::vector<double> Fx;
    std::vector<double> Fy;
    std::vector<double> Fxy;

    std::vector<double> Fij;

    std::vector<double> bessel_coeffs;
    void update_bessel_coeffs();

  public:
    interpolation(double ax, double bx, double ay, double by, int nx, int ny,
                  int func_id);
    ~interpolation() = default;

    void set_x();
    void set_y();
    void set_F();

    void set_G(std::vector<double> &G, std::vector<double> points, int n);
    void set_Fx();
    void set_Fy();
    void set_Fxy();

    void set_Fij(int i, int j);

    void set_bessel_coeffs();
    void set_bessel_coeffs_ij(int ii, int jj);
    void A_inverse(std::vector<double> &A, double h);

    double bessel(double x, double y);
    double bessel_error(double x, double y);

    double max_value() const;
    void change_n(int nx, int ny);
    void change_func(int func_id);
    void change_disturb(int disturb);
    //    void decrease_disturb();
    double get_f_value(double x, double y, interpolation_method method);
    double get_f_max();
};

#endif // INTERPOLATION_H
