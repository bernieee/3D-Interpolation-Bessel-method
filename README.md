<h1 align="center">3D Interpolation. Bessel method</h1>
<p>
</p>

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Qt](https://img.shields.io/badge/Qt-%23217346.svg?style=for-the-badge&logo=Qt&logoColor=white)
![OpenGL](https://img.shields.io/badge/OpenGL-%23FFFFFF.svg?style=for-the-badge&logo=opengl)

## Overview
This Qt-based application renders 3D visualizations of mathematical functions using Bessel interpolation for smooth surfaces. It leverages the graphical power of OpenGL for rendering and provides an interactive interface with Qt for users to explore and manipulate mathematical landscapes in real-time.


<img src="img/photo_2023-11-08 16.49.02.jpeg" width="595"/> <img src="img/photo_2023-11-08 16.49.04.jpeg" width="595"/> 
<img src="img/photo_2023-11-08 16.49.06.jpeg" width="595"/> <img src="img/photo_2023-11-08 16.49.00.jpeg" width="595"/> 


## Key Features
- **3D Rendering**: Visualizes mathematical functions in a 3D space.
- **Interactive Control**: Rotate, scale, and translate the view using mouse inputs.
- **Dynamic Visualization**: Change the rendered functions and methods via GUI controls.
- **User Interface**: Utilizes Qt's `QAction`, `QMenuBar`, and `QToolBar` for in-app options.
- **Scene Customization**: Adjust parameters such as function type, disturbance levels, number of points.
- **Command-Line Arguments**: Supports initialization parameters through the command line for advanced users.


## Supported Functions

The application can visualize the following mathematical functions in 3D space, selectable by the `func_id` parameter:

- `func_id = 0`: `f(x, y) = 1`
- `func_id = 1`: `f(x, y) = x`
- `func_id = 2`: `f(x, y) = y`
- `func_id = 3`: `f(x, y) = x + y`
- `func_id = 4`: `f(x, y) = sqrt(x^2 + y^2)`
- `func_id = 5`: `f(x, y) = x^2 + y^2`
- `func_id = 6`: `f(x, y) = exp(x^2 - y^2)`
- `func_id = 7`: `f(x, y) = 1 / (25(x^2 + y^2) + 1)`
  
## Install

```sh
git clone git@github.com:olyandrevn/3D-Interpolation-Bessel-method.git
```

## Usage

```sh
./3D_interpolation [nx] [ny] [func_id] [eps] [ax] [by] [bx] [ay]
```

```[ax] [bx], [ay] [by]```: intervals for interpolation

```[nx] [ny]```: numbers of points

```[func_id]```: function identifier for selection


## Run on default parameters

```sh
./3D_interpolation 5 5 0 1e-6 -1 1 1 -1
```

## Author

👤 **Olga Kolomyttseva**

* Github: [@olyandrevn](https://github.com/olyandrevn)
* LinkedIn: [@olyandrevn](https://linkedin.com/in/olyandrevn)
