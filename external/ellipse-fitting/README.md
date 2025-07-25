# ellipse-fitting
C++ implementation of Least Squares Fitting of Ellipses

***Citation***: 

    @inproceedings{oy1998NUMERICALLYSD,
    title={NUMERICALLY STABLE DIRECT LEAST SQUARES FITTING OF ELLIPSES},
    author={Radim Hal oy and Jan Flusser},
    year={1998}
    }
    


***Prerequisite C++ Libs:*** <br />
    -*Eigen3* <br />
    -*Vector*

***The usage of the repository***: 

```
mkdir ellipse_fit
cd ellipse_fit
git clone https://github.com/mericdurukan/ellipse-fitting.git
mkdir build
cd build
cmake..(for release mode: cmake -DCMAKE_BUILD_TYPE=Release ..)
make 
./main_ellipse

```
***Note:*** An example ellipse points are created in the main of the repository. 

***Example code usage***: 

Create a two dimensional vector whose data type is double:

```C++
vector<vector<double> > my_vector;

```
Fill this vector with your data. (my_vector[i][0]= your x variable, my_vector[i][1] = your y variable)


Create variables of your ellipse:
```C++
double center_x, center_y, phi, width, height ; 

```
center_x : Center position of your ellipse for X plane. <br />
center_y : Center position of your ellipse for Y plane. <br />
phi: Rotation angle of your ellipse.<br />
width: Width of your ellipse.<br />
height: Height of your ellipse.<br />


Then, create an object with ellipse_fit:

```C++
ellipse_fit my_ellipse;

```

Set data for the fit:

```C++
my_ellipse.set(my_vector);

```

Fit your ellipse:

```
my_ellipse.fit(center_x, center_y, phi, width, height);

```


