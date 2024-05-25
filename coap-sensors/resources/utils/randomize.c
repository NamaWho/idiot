#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


double generate_gaussian(double mean, double stddev) {
    static int haveSpare = 0;
    static double rand1, rand2;

    // set the seed
    srand(time(NULL));

    if(haveSpare) {
        haveSpare = 0;
        return mean + stddev * sqrt(rand1) * sin(rand2);
    }

    haveSpare = 1;

    rand1 = rand() / ((double) RAND_MAX);
    if(rand1 < 1e-100) rand1 = 1e-100;  // Evita il log(0)
    rand1 = -2 * log(rand1);
    rand2 = (rand() / ((double) RAND_MAX)) * 2 * M_PI;

    return mean + stddev * sqrt(rand1) * cos(rand2);
}