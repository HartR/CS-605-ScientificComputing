#include <bits/stdc++.h> 
#include <sys/time.h>

static long MULTIPLIER = 1366; 
static long ADDEND = 150889; 
static long PMOD = 714025; 
long random_last = 0; 
using namespace std; 
int circle_points = 0, square_points = 0; 
double pi;

//PseudoRandom Function Provided by professor
#pragma omp threadprivate(random_last) 
double randomFunction () 
{ 
    long random_next; 
    random_next = (MULTIPLIER*random_last + ADDEND)%PMOD; 
    random_last = random_next; 
    return ((double)random_next/(double)PMOD); 
}

int main(int argc,char* argv[]) 
{ 
    int iterations = atoi(argv[1]); 
	double rand_x, rand_y, origin_dist;

    struct timeval start, end;

    gettimeofday(&start, NULL);
	// Total Random numbers generated = possible x 
	// values * possible y values 
    #pragma omp threadprivate(pi, circle_points, square_points) 
	for (int i = 0; i < iterations; i++) 
    { 

		// Randomly generated x and y values 
		//rand_x = double(rand() % (INTERVAL + 1)) / INTERVAL; 
		//rand_y = double(rand() % (INTERVAL + 1)) / INTERVAL;
        rand_x = randomFunction();
        rand_y = randomFunction(); 

		// Distance between (x, y) from the origin 
		origin_dist = rand_x * rand_x + rand_y * rand_y; 

		// Checking if (x, y) lies inside the define 
		// circle with R=1 
		if (origin_dist <= 1) 
			circle_points++; 

		// Total number of points generated 
		square_points++; 

		// estimated pi after this iteration 
		pi = double(4 * circle_points) / square_points; 

		// For visual understanding (Optional) 
		//cout << rand_x << " " << rand_y << " " << circle_points 
			//<< " " << square_points << " - " << pi << endl << endl;
	} 
    gettimeofday(&end, NULL);
    double runtime = (end.tv_sec - start.tv_sec) * 1e6;
    runtime = (runtime + (end.tv_usec - start.tv_usec)) * 1e-6; 


	// Final Estimated Value 
	cout << "\nFinal Estimation of Pi = " << pi << endl; 
    cout << "Runtime :" << runtime << endl;

	return 0; 
} 
