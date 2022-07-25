#include "interpolation.h"
#include <cmath>
#include <iostream>

double func(int func_id, double x, double y)
{
    switch (func_id) {
    case 0:
        return 1.;
    case 1:
        return x;
    case 2:
        return y;
    case 3:
        return x + y;
    case 4:
        return std::sqrt(x * x + y * y);
    case 5:
        return x * x + y * y;
    case 6:
        return exp(x * x - y * y);
    case 7:
        return 1. / (25. * (x * x + y * y) + 1.);
    default:
        return 0.;
    }
}

double func_2derivative(int func_id, double x, double y)
{
    switch (func_id) {
    case 0:
        return 0.;
    case 1:
        return 0.;
    case 2:
        return 2.;
    case 3:
        return 6. * x;
    case 4:
        return 12. * x * x;
    case 5:
        return exp(x);
    case 6:
        return -50. / (25. * x * x + 1.) / (25. * x * x + 1.) +
               5000 * x * x / (25. * x * x + 1.) / (25. * x * x + 1.) /
                   (25. * x * x + 1.);
    }
    return 0.;
}

interpolation::interpolation(double new_ax, double new_bx, double new_ay,
                             double new_by, int new_nx, int new_ny,
                             int new_func_id)
{
    ax = new_ax;
    bx = new_bx;
    ay = new_ay;
    by = new_by;
    nx = new_nx;
    ny = new_ny;
    func_id = new_func_id;
    disturb = 0;

    x.resize(nx);
    y.resize(ny);
    F.resize(nx * ny);

    Gx.resize(nx * nx);
    Gy.resize(ny * ny);

    Fx.resize(nx * ny);
    Fy.resize(nx * ny);
    Fxy.resize(nx * ny);

    Fij.resize(4 * 4);
    bessel_coeffs.resize(4 * 4 * nx * ny);

    set_x();
    set_y();
    set_F();

    update_bessel_coeffs();
}

void interpolation::set_x()
{
    double step_x = (bx - ax) / (nx - 1);
    for (int i = 0; i < nx; i++) {
        x[i] = ax + i * step_x;
    }
}

void interpolation::set_y()
{
    double step_y = (by - ay) / (ny - 1);
    for (int i = 0; i < ny; i++) {
        y[i] = ay + i * step_y;
    }
}

void interpolation::set_F()
{
    Fmax = 0;
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            F[i * nx + j] = func(func_id, x[i], y[j]);
            Fmax = std::max(Fmax, F[i * nx + j]);
        }
    }
}

void interpolation::set_G(std::vector<double> &G, std::vector<double> points,
                          int n)
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            G[i * n + j] = 0.;
        }
    }
    for (int i = 1; i < n - 1; i++) {
        double tmp1 = (points[i + 1] - points[i]) / (points[i] - points[i - 1]);
        double tmp2 = (points[i + 1] - points[i - 1]);

        G[i * n + i + 0] = (tmp1 - 1. / tmp1) / tmp2;
        G[i * n + i - 1] = -tmp1 / tmp2;
        G[i * n + i + 1] = 1. / tmp1 / tmp2;
    }

    double tmp_1 = points[1] - points[0];
    double tmp_2 = points[n - 1] - points[n - 2];

    G[0 * n + 0] = -1. / tmp_1;
    G[0 * n + 1] = 1. / tmp_1;

    G[(n - 1) * n + n - 2] = -1. / tmp_2;
    G[(n - 1) * n + n - 1] = 1. / tmp_2;
}

void interpolation::set_Fx()
{
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            Fx[i * nx + j] = 0;
            for (int k = 0; k < nx; k++) {
                Fx[i * nx + j] += Gx[i * nx + k] * F[k * nx + j];
            }
        }
    }
}

void interpolation::set_Fy()
{
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            Fy[i * nx + j] = 0;
            for (int k = 0; k < ny; k++) {
                Fy[i * nx + j] += F[i * nx + k] * Gy[j * ny + k];
            }
        }
    }
}

void interpolation::set_Fxy()
{
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            Fxy[i * nx + j] = 0;
            for (int k = 0; k < nx; k++) {
                Fxy[i * nx + j] += Gx[i * nx + k] * Fy[k * nx + j];
            }
        }
    }
}

void interpolation::set_Fij(int i, int j)
{
    Fij[0 * 4 + 0] = F[i * nx + j];
    Fij[0 * 4 + 1] = Fy[i * nx + j];
    Fij[0 * 4 + 2] = F[i * nx + j + 1];
    Fij[0 * 4 + 3] = Fy[i * nx + j + 1];

    Fij[1 * 4 + 0] = Fx[i * nx + j];
    Fij[1 * 4 + 1] = Fxy[i * nx + j];
    Fij[1 * 4 + 2] = Fx[i * nx + j + 1];
    Fij[1 * 4 + 3] = Fxy[i * nx + j + 1];

    Fij[2 * 4 + 0] = F[(i + 1) * nx + j];
    Fij[2 * 4 + 1] = Fy[(i + 1) * nx + j];
    Fij[2 * 4 + 2] = F[(i + 1) * nx + j + 1];
    Fij[2 * 4 + 3] = Fy[(i + 1) * nx + j + 1];

    Fij[3 * 4 + 0] = Fx[(i + 1) * nx + j];
    Fij[3 * 4 + 1] = Fxy[(i + 1) * nx + j];
    Fij[3 * 4 + 2] = Fx[(i + 1) * nx + j + 1];
    Fij[3 * 4 + 3] = Fxy[(i + 1) * nx + j + 1];
}

void interpolation::set_bessel_coeffs()
{
    for (int i = 0; i < nx - 1; i++) {
        for (int j = 0; j < ny - 1; j++) {
            set_Fij(i, j);
            set_bessel_coeffs_ij(i, j);
        }
    }
}

void interpolation::set_bessel_coeffs_ij(int ii, int jj)
{
    std::vector<double> A_inv_x;
    std::vector<double> A_inv_y;
    std::vector<double> Tmp;

    A_inv_x.resize(4 * 4);
    A_inv_y.resize(4 * 4);
    Tmp.resize(4 * 4);

    A_inverse(A_inv_x, x[ii + 1] - x[ii]);
    A_inverse(A_inv_y, y[jj + 1] - y[jj]);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            Tmp[i * 4 + j] = 0;
            for (int k = 0; k < 4; k++) {
                Tmp[i * 4 + j] += Fij[i * 4 + k] * A_inv_y[j * 4 + k];
            }
        }
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            bessel_coeffs[ii * 4 * 4 * ny + jj * 4 * 4 + i * 4 + j] = 0;
            for (int k = 0; k < 4; k++) {
                bessel_coeffs[ii * 4 * 4 * ny + jj * 4 * 4 + i * 4 + j] +=
                    A_inv_x[i * 4 + k] * Tmp[k * 4 + j];
            }
        }
    }
}

void interpolation::update_bessel_coeffs()
{
    set_G(Gx, x, nx);
    set_G(Gy, y, ny);

    set_Fx();
    set_Fy();
    set_Fxy();

    set_bessel_coeffs();
}

void interpolation::A_inverse(std::vector<double> &A, double h)
{
    A[0 * 4 + 0] = 1;
    A[0 * 4 + 1] = 0;
    A[0 * 4 + 2] = 0;
    A[0 * 4 + 3] = 0;

    A[1 * 4 + 0] = 0;
    A[1 * 4 + 1] = 1;
    A[1 * 4 + 2] = 0;
    A[1 * 4 + 3] = 0;

    A[2 * 4 + 0] = (-3.) / pow(h, 3);
    A[2 * 4 + 1] = (-2.) / pow(h, 2);
    A[2 * 4 + 2] = (3.) / pow(h, 3);
    A[2 * 4 + 3] = (-1.) / pow(h, 2);

    A[3 * 4 + 0] = (2.) / pow(h, 3);
    A[3 * 4 + 1] = (1.) / pow(h, 2);
    A[3 * 4 + 2] = (-2.) / pow(h, 3);
    A[3 * 4 + 3] = (1.) / pow(h, 2);
}

double interpolation::bessel(double new_x, double new_y)
{
    int ii = 0;
    int jj = 0;

    for (int i = 0; i < nx - 1; i++) {
        if (new_x < x[i + 1] + 1.e-6) {
            ii = i;
            break;
        }
    }

    for (int j = 0; j < ny - 1; j++) {
        if (new_y < y[j + 1] + 1.e-6) {
            jj = j;
            break;
        }
    }

    double bessel_res = 0;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {

            double xx = 1;
            for (int k = 0; k < i; k++) {
                xx *= (new_x - x[ii]);
            }

            double yy = 1;
            for (int k = 0; k < j; k++) {
                yy *= (new_y - y[jj]);
            }

            double tmp =
                bessel_coeffs[ii * 4 * 4 * ny + jj * 4 * 4 + i * 4 + j];

            bessel_res += tmp * xx * yy;
        }
    }

    return bessel_res;
}

double interpolation::bessel_error(double x, double y)
{
    return func(func_id, x, y) - bessel(x, y);
}

void interpolation::change_func(int new_func_id)
{
    if (new_func_id == func_id) {
        return;
    }

    func_id = new_func_id;
    disturb = 0;

    set_F();
    update_bessel_coeffs();
}

void interpolation::change_n(int new_nx, int new_ny)
{
    if (new_nx == nx && new_ny == ny) {
        return;
    }

    nx = new_nx;
    ny = new_ny;

    x.resize(nx);
    y.resize(ny);
    F.resize(nx * ny);

    Gx.resize(nx * nx);
    Gy.resize(ny * ny);

    Fx.resize(nx * ny);
    Fy.resize(nx * ny);
    Fxy.resize(nx * ny);

    bessel_coeffs.resize(4 * 4 * nx * ny);

    set_x();
    set_y();
    set_F();

    update_bessel_coeffs();
    change_disturb(disturb);
}

void interpolation::change_disturb(int new_disturb)
{
    disturb = new_disturb;
    //    F[(nx / 2) * nx + (ny / 2)] =
    //        func(func_id, x[nx / 2], y[ny / 2]) + disturb * 0.1 * Fmax;

    double F_new = func(func_id, x[nx / 2], y[ny / 2]) + disturb * 0.1 * Fmax;

    //    for (int i = (ny / 2) - 1; i < (ny / 2) + 1; i++) {
    //        Fx[i * nx + (ny / 2)] -=
    //            Gx[i * nx + (nx / 2)] * F[(nx / 2) * nx + (ny / 2)];
    //        Fx[i * nx + (ny / 2)] += Gx[i * nx + (nx / 2)] * F_new;
    //    }

    Fx[(ny / 2) * nx + (ny / 2)] -=
        Gx[(ny / 2) * nx + (nx / 2)] * F[(nx / 2) * nx + (ny / 2)];
    Fx[(ny / 2) * nx + (ny / 2)] += Gx[(ny / 2) * nx + (nx / 2)] * F_new;

    Fx[(ny / 2 - 1) * nx + (ny / 2)] -=
        Gx[(ny / 2 - 1) * nx + (nx / 2)] * F[(nx / 2) * nx + (ny / 2)];
    Fx[(ny / 2 - 1) * nx + (ny / 2)] +=
        Gx[(ny / 2 - 1) * nx + (nx / 2)] * F_new;

    Fx[(ny / 2 + 1) * nx + (ny / 2)] -=
        Gx[(ny / 2 + 1) * nx + (nx / 2)] * F[(nx / 2) * nx + (ny / 2)];
    Fx[(ny / 2 + 1) * nx + (ny / 2)] +=
        Gx[(ny / 2 + 1) * nx + (nx / 2)] * F_new;

    for (int j = ny / 2 - 1; j <= ny / 2 + 1; j++) {
        Fxy[(nx / 2) * nx + j] -=
            Gx[(nx / 2) * nx + (nx / 2)] * Fy[(nx / 2) * nx + j];
    }

    Fy[(nx / 2) * nx + (ny / 2)] -=
        F[(nx / 2) * nx + (ny / 2)] * Gy[(ny / 2) * ny + (ny / 2)];
    Fy[(nx / 2) * nx + (ny / 2)] += F_new * Gy[(ny / 2) * ny + (ny / 2)];

    Fy[(nx / 2) * nx + (ny / 2 - 1)] -=
        F[(nx / 2) * nx + (ny / 2)] * Gy[(ny / 2 - 1) * ny + (ny / 2)];
    Fy[(nx / 2) * nx + (ny / 2 - 1)] +=
        F_new * Gy[(ny / 2 - 1) * ny + (ny / 2)];

    Fy[(nx / 2) * nx + (ny / 2 + 1)] -=
        F[(nx / 2) * nx + (ny / 2)] * Gy[(ny / 2 + 1) * ny + (ny / 2)];
    Fy[(nx / 2) * nx + (ny / 2 + 1)] +=
        F_new * Gy[(ny / 2 + 1) * ny + (ny / 2)];

    //    for (int j = ny / 2 - 1; j <= ny / 2 + 1; j++) {
    //        Fy[(nx / 2) * nx + j] -= F[(nx / 2) * nx + j] * Gy[(ny / 2) * ny +
    //        j]; Fy[(nx / 2) * nx + j] += F_new * Gy[(ny / 2) * ny + j];
    //    }

    for (int j = ny / 2 - 1; j <= ny / 2 + 1; j++) {
        Fxy[(nx / 2) * nx + j] +=
            Gx[(nx / 2) * nx + (nx / 2)] * Fy[(nx / 2) * nx + j];
    }

    F[(nx / 2) * nx + (ny / 2)] =
        func(func_id, x[nx / 2], y[ny / 2]) + disturb * 0.1 * Fmax;

    set_Fij(nx / 2, ny / 2);
    set_bessel_coeffs_ij(nx / 2, ny / 2);

    set_Fij(ny / 2, ny / 2);
    set_bessel_coeffs_ij(ny / 2, ny / 2);

    set_Fij(ny / 2 - 1, ny / 2);
    set_bessel_coeffs_ij(ny / 2 - 1, ny / 2);

    set_Fij(ny / 2 + 1, ny / 2);
    set_bessel_coeffs_ij(ny / 2 + 1, ny / 2);

    set_Fij(nx / 2, ny / 2 - 1);
    set_bessel_coeffs_ij(nx / 2, ny / 2 - 1);

    set_Fij(nx / 2, ny / 2 + 1);
    set_bessel_coeffs_ij(nx / 2, ny / 2 + 1);

    //    update_bessel_coeffs();
}

double interpolation::get_f_value(double x, double y,
                                  interpolation_method method)
{
    switch (method) {
    case interpolation_method::origin:
        return func(func_id, x, y);
    case interpolation_method::bessel:
        return bessel(x, y);
    case interpolation_method::error:
        return bessel_error(x, y);
    }
    return 0;
}

double interpolation::get_f_max()
{
    return Fmax;
}
