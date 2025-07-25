#include "ellipse_fit.h"
#include <ctime>
using namespace std; 

int main(){

    vector<vector< double>> A;
    A.resize(11 , std::vector<double>(2, 0));

    for(int j =0; j<11; j++){

        A[j][0] = j;
        A[j][1] = sqrt(400-j*j);


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




    return 0;
}
