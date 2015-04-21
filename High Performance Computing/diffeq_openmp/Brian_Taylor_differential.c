/*
 Brian Taylor

 Calculate a differential equation (Laplace)
 Usage: differential <array_dimension> <num_threads>
*/

#include <stdio.h>
#include <sys/time.h>
#include <omp.h>

#define ITER_LIMIT 150000

int main (int argc, char *argv[] )
{
  int i, j, k, iter;
  float** x ;
  float** xn ;
  float** temp;
  int conv;
  int n;
  int p;
  float error ;
  double time_start, time_end;
  struct timeval tv;
  struct timezone tz;

  /* the array size should be supplied as a command line argument */
  if(argc != 3) {printf("wrong number of arguments") ; exit(2) ;}
  n = atoi(argv[1]) ;
  p = atoi(argv[2]);
  printf("Array size = %d \n ", n );

  x = malloc(sizeof(float*)*(n+2));
  for (i = 0; i < n+2; i++) {
                x[i] = malloc(sizeof(float)*(n+2));
  }
  xn = malloc(sizeof(float*)*(n+2));
  for (i = 0; i < n+2; i++) {
                xn[i] = malloc(sizeof(float)*(n+2));
  }
  iter = 0;

  #pragma omp parallel num_threads(p) shared(x, xn, iter, conv) private(i, j, k, error)
  {
    /* initialize both matrices to 0, with boundary conditions.
       parallelized in an attempt to pre-load the cache */
    #pragma omp for schedule (static)
    for (i=1 ; i <= n ; i++)
    {
      for(j=1 ; j <= n ; j++)
      {
        x[i][j] = 0 ;
        xn[i][j] = 0;
      }
      x[0][i] = 0 ;
      x[i][0] = 0 ;
      x[n+1][i] = i ;
      x[i][n+1] = i ;
      xn[0][i] = 0 ;
      xn[i][0] = 0 ;
      xn[n+1][i] = i ;
      xn[i][n+1] = i ;
    }

    #pragma omp single
    {
      gettimeofday(&tv, &tz);
      time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
    }

    for (k = 1; k < ITER_LIMIT; k++)
    {
      #pragma omp barrier
      /* start with the assumption that we will converge this iteration */
      #pragma omp single
        conv = 1;

      /* the main work to parallelize */
      #pragma omp for schedule (static)
      for (i=1 ; i <= n ; i++)
      {
        for (j=1 ; j <= n ; j++)
        { xn[i][j] = 0.25 * (x[i-1][j]+x[i+1][j]+x[i][j-1]+x[i][j+1]) ;
          /* only bother checking if we still think we might converge */
          if (conv)
          {
            if(xn[i][j] <= x[i][j]) error = x[i][j] - xn[i][j] ;
                             else   error = xn[i][j] - x[i][j] ;
            if(error > 0.001)
            {
              /* this doesn't need to be a critical section, since it's
                 only just the setting of a flag, essentially, and not
                 a comparison or update of a value. since the iterations
                 should continue if even one value hasn't converged,
                 everyone can set this flag as soon as they get here */
              conv = 0 ;
            }
          }
        }
      }

      #pragma omp single
      {
        /* exchange the pointers to make the new calculated matrix
           the current matrix for the next iteration */
        temp = x;
        x = xn;
        xn = temp;
      }

      /* make sure all threads have finished this iteration */
      #pragma omp barrier

      /* the iteration has completed, if we still think
         we've converged at this point, then we have */
      if (conv == 1)
      {
          /* save the private value of k for printing */
          #pragma omp single
            iter = k;
          k = ITER_LIMIT;
      }
    }

    #pragma omp single
    {
      gettimeofday(&tv, &tz);
      time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
      if (!conv)
        iter = k;
    }
  }

  printf("number of iterations is %d \n", iter) ;
  printf("print some diagonal elements for checking results: \n");
  for (i=1 ; i <= n ; i = i + n/8)
    printf(" %f ", x[i][i]) ;
  printf("\n ") ;
  printf("time: %lf\n", time_end - time_start);

  return 0;
}
