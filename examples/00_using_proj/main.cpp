#include <stdio.h>
#include <proj.h>

int main(int argc, char *argv[])
{
    //???


    PJ_CONTEXT *C;
    PJ *P;
    PJ *norm;
    PJ_COORD a, b;

    C = proj_context_create(); //Create a new threading-context

    // Create a transformation object that is a pipeline between two known coordinate reference systems
    P = proj_create_crs_to_crs (C,
                                "EPSG:2157", // source crs
                                "EPSG:29902", //target crs
                                nullptr);

    if (0 == P) {
        fprintf(stderr, "Failed to create transformation object.\n");
        return 1;
    }

    proj_context_destroy(C);

}
