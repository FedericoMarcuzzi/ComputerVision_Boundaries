# ComputerVision_Boundaries
Given an image, the program finds the boundaries of the objects (C++ and OpenCV).

Second assignment for the *Computer Vision* [course](https://www.dsi.unive.it/~bergamasco/courses/computer_vision_2017_2018.html) of *Università Ca' Foscari Venezia*, academic year 2017/2018


Install OpenCV:

```console
$ git clone --depth 1 https://github.com/opencv/opencv.git
$ cd opencv
$ mkdir build
$ cd build

$ cmake ../ -DCMAKE_BUILD_TYPE="Release"
$ make -j 2

$ make install
```


How to build 'opencv_boundaries':

```console
$ mkdir build
$ cd build
$ cmake ../ -DOpenCV_DIR="<insert the path of your opencv/build directory>"
$ make
```


How to run 'opencv_boundaries':

```console
$ make install
$ cd ../dist/bin
$ ./boundaries input_filename output_filename min_perimeter_size max_perimeter_size
```


How to run the example:

```console
$ ./boundaries boundaries.png boundaries_out.png 100 200
```


Federico Marcuzzi, 2020


