#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <complex.h>    /* Standard Library of Complex Numbers */
#include <assert.h>

//area of complex space to investigate
float X1 = -1.8;
float X2 = 1.8;
float Y1 = -1.8;
float Y2 = 1.8;
float c_real = -0.62772;
float c_imag = -0.42193;

int * calculate_z(int maxiter, int width, float complex *zs, float complex *cs) {
  //Calculate output list using Julia update rule
  int n;
  float complex z, c, aux;
  int *output = (int *)calloc(width, sizeof(int));

  for (int i = 0; i < width; i++)
  {
    n = 0;
    z = zs[i];
    c = cs[i];
    while (cabsf(z) < 2 && n < maxiter) {
      z = z * z + c;
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
  float x_step = (X2 - X1) / desired_width;
  float y_step = (Y1 - Y2) / desired_width;

  float *x = (float *)malloc((desired_width+100) * sizeof(float));
  float *y = (float *)malloc((desired_width+100) * sizeof(float));
  float complex *zs = (float complex *)malloc((desired_width) * sizeof(float complex));
  float complex *cs = (float complex *)malloc((desired_width) * sizeof(float complex));
  
  int total_elements = 0;
  int *output;
  int _iy = 0, _ix = 0;

  float ycoord = Y2;
  // for (int i = 0; i < desired_width; i++) {
  //   y[i] = ycoord;
  //   ycoord += y_step;
  // }
  while (ycoord > Y1) {
    y[_iy] = ycoord;
    ycoord += y_step;
    _iy++;
  }

  float xcoord = X1;
  // for (int i = 0; i < desired_width; i++) {
  //   x[i] = xcoord;
  //   xcoord += x_step;
  // }
  // @ToDo: En comparacion al codigo python, la precision de la 
  // suma interna del bucle no es muy precisa.
  while (xcoord < X2) {
    x[_ix] = xcoord;
    xcoord += x_step;
    _ix++;
    printf("xcoord: %f",xcoord);
  }

  // Build a list of coordinates and the initial condition for each cell.
  // Note that our initial condition is a constant and could easily be removed;
  // we use it to simulate a real-world scenario with several inputs to
  // our function.
  for (int i = 0; i < _iy; i++) {
    for (int j = 0; j < _ix; j++) {
      zs[i] = x[j] + y[i]*I;
      cs[i] = c_real + c_imag*I;
    }
  }

  printf("Length of x:%d\n", _ix);
  printf("Total elements:%d\n", desired_width * desired_width);
  output = calculate_z(max_iterations, desired_width, zs, cs);

  // This sum is expected for a 1000^2 grid with 300 iterations.
  // It catches minor errors we might introduce when were
  // working on a fixed set of inputs.
  // assert(sum(output,desired_width) == 33219980);
  printf("output: %d\n", sum(output,desired_width));

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