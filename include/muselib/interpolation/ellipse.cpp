#include "ellipse.h"

inline
void fit_ellipse (std::vector<double> &vec_x, std::vector<double> &vec_y)
{
    std::vector<std::vector<double>> A;
    A.resize(vec_x.size(), std::vector<double>(2, 0));

    for(size_t j =0; j<vec_x.size(); j++)
    {
        A[j][0] = vec_x.at(j);
        A[j][1] = vec_y.at(j);
    }

    double center_x, center_y, phi, width, height;
    clock_t begin = clock();

    ellipse_fit my_ellipse;
    my_ellipse.set(A);
    my_ellipse.fit(center_x, center_y, phi, width, height);

    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout<<" elapsed time: "<<elapsed_secs *1000<<" ms"<<endl;


    cout<<"center x: "<<center_x<<endl;
    cout<<"center y: "<<center_y<<endl;
    cout<<"phi: "<<phi<<endl;
    cout<<"width: "<<width<<endl;
    cout<<"height: "<<height<<endl;
}
