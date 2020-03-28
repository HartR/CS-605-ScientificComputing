#include <omp.h>
#include <bits/stdc++.h> 
#include <sys/time.h>

using namespace std;

static long MULTIPLIER = 1366; 
static long ADDEND = 150889; 
static long PMOD = 714025; 
long random_last = 0;

//Pseudo Random Function Provided by Professor
#pragma omp threadprivate(random_last) 
double RandomFunction () 
{ 
    long random_next; 
    random_next = (MULTIPLIER*random_last + ADDEND)%PMOD; 
    random_last = random_next; 
    return ((double)random_next/(double)PMOD); 
}

//function that returns number of points within a unit circle
int CheckInCircle (int thread_iterations){
	int in_circle_count = 0;
	double x_coordinate, y_coordinate;
	for (int i = 0; i < thread_iterations; i++) {
	  x_coordinate = RandomFunction();
	  y_coordinate = RandomFunction();
    //test whether coordinate is within the unit circle by calculating distance from center
	  if(x_coordinate*x_coordinate + y_coordinate*y_coordinate <= 1.0 ) 
      in_circle_count++;
  }
  return in_circle_count;
}

int main(int argc, char* argv[])
{
  //get input, first argument is # of threads, second is amount of times we want to calculate
  int number_of_threads =  atoi(argv[1]);
  int iterations = atoi(argv[2]);

  //initialize vars
  int iterations_per_thread, number_of_hits = 0;
  double pi_estimate;
  struct timeval start, end;

  //divide work amongst worker threads
  iterations_per_thread = iterations/number_of_threads;

  //get start time
  gettimeofday(&start, NULL);

  //perform threaded workload
  #pragma omp parallel reduction(+:number_of_hits) num_threads(number_of_threads)
	{
		number_of_hits += CheckInCircle(iterations_per_thread);
  }

  //get end time and subtract start from end to get runtime
	gettimeofday(&end, NULL);
  double runtime = (end.tv_sec - start.tv_sec) * 1e6;
  runtime = (runtime + (end.tv_usec - start.tv_usec)) * 1e-6; 

  //need to cast iterations to double to prevent truncation
  pi_estimate = 4 * number_of_hits / (double) iterations;
  cout << "Runtime :" << runtime << endl;
  cout << "Number of threads: " << number_of_threads << endl;
  cout << "Estimate of pi: " << pi_estimate << endl << endl;
  return 0;
}


