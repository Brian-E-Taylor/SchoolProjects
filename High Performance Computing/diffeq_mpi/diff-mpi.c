/* Brian Taylor 

 Calculate a differential equation (Laplace)
 Usage: differential <array_dimension>
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mpi.h"

#define ITER_LIMIT 500000
#define CONV_CHECK 100

main (int argc, char *argv[] )
{
  int my_y, i, j, k, conv, allconv, check;
  int numtasks, rank;
  float** x ;
  float** xn ;
  float** temp;
  int n ;
  float error ;
  MPI_Status stat[4];
  MPI_Request req[4];
  double time_start, time_end;
  struct timeval tv;
  struct timezone tz;

  /* the array size should be supplied as a command line argument */
  if(argc != 2) {printf("wrong number of arguments") ; exit(2) ;}
  n = atoi(argv[1]) ;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0)
  {
    printf("Array size = %d \n", n );
    printf("Tasks: %d\n", numtasks);
  }
  my_y = n/numtasks;
  x = malloc(sizeof(float*)*(my_y + 2));
  for (i = 0; i < my_y + 2; i++)
    x[i] = calloc(n+2, sizeof(float));
  xn = malloc(sizeof(float*)*(my_y + 2));
  for (i = 0; i < my_y + 2; i++)
    xn[i] = calloc(n+2, sizeof(float));

  if (rank == 0)
  {
    for (j = 1; j <= n; j++)
    {
      x[0][j] = 0;
      xn[0][j] = 0;
    }
  }
  if (rank == numtasks - 1)
  {
    for (j = 1; j <= n; j++)
    {
      x[my_y+1][j] = j;
      xn[my_y+1][j] = j;
    }
  }

  for (j=1; j <= n; j++)
  {
    if (j > rank * my_y && j <= (rank + 1) * my_y)
    {
      if ((j % my_y) == 0)
      {
        x[my_y][0] = 0;
        xn[my_y][0] = 0;
        x[my_y][n+1] = j;
        xn[my_y][n+1] = j;
      }
      else
      {
        x[j % my_y][0] = 0;
        xn[j % my_y][0] = 0;
        x[j % my_y][n+1] = j;
        xn[j % my_y][n+1] = j;
      }
    }
  }
 
  MPI_Barrier(MPI_COMM_WORLD);
  if (rank == 0)
  {
    gettimeofday(&tv, &tz);
    time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
  }

  for(k=1 ; k < ITER_LIMIT; k++) /* put an upper bound to prevent infinite loops */
  {
    conv = allconv = 0;
    check = (k % CONV_CHECK) == 0;
    if (check)
      conv = 1;
    if (rank != 0)
    {
      // Send ABOVE
      MPI_Isend(&x[1][1], n, MPI_FLOAT, rank - 1, 0, MPI_COMM_WORLD, &req[0]);
      // Recv from ABOVE
      MPI_Irecv(&x[0][1], n, MPI_FLOAT, rank - 1, 1, MPI_COMM_WORLD, &req[3]);
    }
    if (rank != numtasks - 1)
    {
      // Send BELOW
      MPI_Isend(&x[my_y][1], n, MPI_FLOAT, rank + 1, 1, MPI_COMM_WORLD, &req[1]);
      // Recv from BELOW
      MPI_Irecv(&x[my_y+1][1], n, MPI_FLOAT, rank + 1, 0, MPI_COMM_WORLD, &req[2]);
    }

    // Do local calculations (not dependant on neighbor values)
    for (i=2 ; i <= my_y-1 ; i++)
    {
      for (j=1 ; j <= n ; j++)
      {
        xn[i][j] = 0.25 * (x[i-1][j]+x[i+1][j]+x[i][j-1]+x[i][j+1]) ;
        if (check)
        {
          if (conv)
          {
            /* only bother checking if we still think we might converge */
            if(xn[i][j] <= x[i][j]) error = x[i][j] - xn[i][j] ;
                             else   error = xn[i][j] - x[i][j] ;
            if(error > 0.001)
            {
              conv = 0;
            }
          }
        }
      }
    }

    // Make sure we have the data for neighbor calculation
    if (rank != 0)
    {
      MPI_Wait(&req[0], &stat[0]);
      MPI_Wait(&req[3], &stat[3]);
    }
    if (rank != numtasks - 1)
    {
      MPI_Wait(&req[1], &stat[1]);
      MPI_Wait(&req[2], &stat[2]);
    }

    // Do neighbor involved calculations
    // calculate the top row
    for (j=1 ; j <= n ; j++)
    {
      xn[1][j] = 0.25 * (x[0][j]+x[2][j]+x[1][j-1]+x[1][j+1]) ;
      if (check)
      {
        if (conv)
        {
          /* only bother checking if we still think we might converge */
          if(xn[1][j] <= x[1][j]) error = x[1][j] - xn[1][j] ;
                           else   error = xn[1][j] - x[1][j] ;
          if(error > 0.001)
          {
            conv = 0;
          }
        }
      }
    }
    // calculate the bottom row
    // if numtasks = n, this calculation would be repeating the same as above
    if (my_y != 1)
    {
      for (j=1 ; j <= n ; j++)
      {
        xn[my_y][j] = 0.25 * (x[my_y-1][j]+x[my_y+1][j]+x[my_y][j-1]+x[my_y][j+1]) ;
        if (check)
        {
          if (conv)
          {
            /* only bother checking if we still think we might converge */
            if(xn[my_y][j] <= x[my_y][j]) error = x[my_y][j] - xn[my_y][j] ;
                                   else   error = xn[my_y][j] - x[my_y][j] ;
            if(error > 0.001)
            {
              conv = 0;
            }
          }
        }
      }
    }

    if (check)
    {
      MPI_Allreduce(&conv, &allconv, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
      if (allconv == numtasks)
        break;
    }

    temp = x;
    x = xn;
    xn = temp;
  }

  if (rank == 0)
  {
    gettimeofday(&tv, &tz);
    time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

    printf("number of iterations is %d \n", k) ;
    printf("print some diagonal elements for checking results: \n");
  }

  MPI_Barrier(MPI_COMM_WORLD);
  for (i=1 ; i <= n ; i = i + n/8)  
  {
    if (i > rank * my_y && i <= (rank + 1) * my_y)
    {
      if (my_y == 1)
        printf("x[%d][%d] = %f\n", i, i, x[1][i]) ;
      else
        printf("x[%d][%d] = %f\n", i, i, x[i % my_y][i]) ;
    }
    MPI_Barrier(MPI_COMM_WORLD);
  }

  if (rank == 0)
  {
    printf("time: %lf\n", time_end - time_start);
  }

  MPI_Finalize();
}
