#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <assert.h>

//area of complex space to investigate
double X1 = -1.8;
double X2 = 1.8;
double Y1 = -1.8;
double Y2 = 1.8;
double c_real = -0.62772;
double c_imag = -0.42193;

int * calculate_z(int maxiter, int width, double complex *zs, double complex *cs) {
  //Calculate output list using Julia update rule
  int n;
  double complex z, c, ab;
  int calc = 0;
  int *output = (int *)calloc(width, sizeof(int));
  double two = 2.0, csq;

  for (int i = 0; i < width; i++) {
    n = 0;
    z = zs[i];
    c = cs[i];
    while (cabs(z) < 2.0 && n < maxiter) {
      z = (z*z) + c;
      n += 1;
    }
    output[i] = n;
  }
  return output;
};

int sum(int *array, int width){
  int output_sum = 0;
  for (int i = 0; i < width; i++)
      output_sum += array[i];

  return output_sum;
}

void calc_pure_c(int desired_width, int max_iterations)
{
  //Create a list of complex coordinates (zs) and complex parameters (cs), build Julia set, and display
  double x_step = (X2 - X1) / (double) desired_width;
  double y_step = (Y1 - Y2) / (double) desired_width;

  double *x = (double *)malloc((desired_width+1) * sizeof(double));
  double *y = (double *)malloc((desired_width+1) * sizeof(double));
 
  int total_elements = 0;
  int *output;
  int size_y = 0, size_x = 0;

  double ycoord = Y2;
  double xcoord = X1;


  while (ycoord > Y1) {
    y[size_y] = ycoord;
    ycoord += y_step;
    size_y++;
  }

  while (xcoord < X2) {
    x[size_x] = xcoord;
    xcoord += x_step;
    size_x++;
  }

  double complex *zs = (double complex *)malloc((size_x * size_y) * sizeof(double complex));
  double complex *cs = (double complex *)malloc((size_x * size_y) * sizeof(double complex));
 
  // Build a list of coordinates and the initial condition for each cell.
  // Note that our initial condition is a constant and could easily be removed;
  // we use it to simulate a real-world scenario with several inputs to
  // our function.
  for (int i = 0; i < size_y; i++) {
    for (int j = 0; j < size_x; j++) {
      zs[(i*size_x) + j] = x[j] + y[i]*I;
      cs[(i*size_x) + j] = c_real + c_imag*I;
    }
  }

  printf("Length of x:%d\n", size_x);
  printf("Total elements:%d\n", size_y * size_x);
  output = calculate_z(max_iterations, (size_y*size_x), zs, cs);

  // This sum is expected for a 1000^2 grid with 300 iterations.
  // It catches minor errors we might introduce when were
  // working on a fixed set of inputs.
  assert(sum(output, (size_y*size_x)) == 33219980);
  //printf("output: %d\n", sum(output, (size_y*size_x)));

};

int main(int argc, char** argv)
{
    int desired_width=1000, max_iterations=300;

    // obtener argumentos proporcionados en tiempo de ejecucion
    if (argc>1) {  desired_width  = atoi(argv[1]); }
    if (argc>2) {  max_iterations = atoi(argv[2]); }

    // Calculate the Julia set using a pure C solution
    calc_pure_c(desired_width, max_iterations);
}
