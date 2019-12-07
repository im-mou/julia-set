#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define STB_IMAGE_WRITE_STATIC
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

//area of complex space to investigate
double X1 = -1.8;
double X2 = 1.8;
double Y1 = -1.8;
double Y2 = 1.8;
double c_real = -0.62772;
double c_imag = -0.42193;

int size_y = 0;
int size_x = 0;

int *calculate_z(int maxiter, int width, double complex *zs, double complex *cs)
{
	//Calculate output list using Julia update rule
	int n;
	double complex z, c;
	int *output = (int *)calloc(width, sizeof(int));
	double abs = 0;
	for (int i = 0; i < width; i++)
	{
		n = 0;
		z = zs[i];
		c = cs[i];
		while (cabs(z) < 2.0 && n < maxiter)
		{
			z *= z;
			z += c;
			n += 1;
		};
		output[i] = n;
	}
	return output;
};

int sum(int *output_array, int width)
{
	int output_sum = 0;
	for (int i = 0; i < width; i++)
	{
		output_sum += output_array[i];
	}

	return output_sum;
};

int *calc_pure_c(int desired_width, int max_iterations)
{

	// Create a list of complex coordinates (zs) and complex parameters (cs),
	// build Julia set, and display
	double x_step = (X2 - X1) / (double)desired_width;
	double y_step = (Y1 - Y2) / (double)desired_width;

	double *x = (double *)malloc((desired_width + 1) * sizeof(double));
	double *y = (double *)malloc((desired_width + 1) * sizeof(double));

	int total_elements = 0;
	int *output;

	double ycoord = Y2;
	double xcoord = X1;

	while (ycoord > Y1)
	{
		y[size_y] = ycoord;
		ycoord += y_step;
		size_y++;
	}

	while (xcoord < X2)
	{
		x[size_x] = xcoord;
		xcoord += x_step;
		size_x++;
	}

	double complex *zs = (double complex *)malloc((size_x * size_y) * sizeof(double complex));
	double complex *cs = (double complex *)malloc((size_x * size_y) * sizeof(double complex));

	// Build a list of coordinates and the initial condition for each cell.
	// Note that our initial condition is a constant and could easily be removed;
	// we use it to simulate a real-world scenario with several inputs to
	// our function
	double complex fixed_val = c_real + c_imag * I;
	double complex temp;
	for (int i = 0; i < size_y; i++)
	{
		temp = y[i] * I;
		for (int j = 0; j < size_x; j++)
		{
			zs[(i * size_x) + j] = x[j] + temp;
		}
	}

	for (int i = 0; i < size_x * size_y; i++)
	{
		cs[i] = fixed_val;
	}
	printf("Length of x:%d\n", size_x);
	printf("Total elements:%d\n", size_y * size_x);

	// calcular la Z
	output = calculate_z(max_iterations, (size_y * size_x), zs, cs);

	// This sum is expected for a 1000^2 grid with 300 iterations.
	// It catches minor errors we might introduce when were
	// working on a fixed set of inputs.
	//assert(sum(output, (size_y*size_x)) == 33219980);
	printf("Sum:%d\n", sum(output, (size_y * size_x)));

	return output;
};

// funcio para crear una imagen
// Ref: https://github.com/nothings/stb/blob/master/tests/image_write_test.c
void write_image(int size_x, int size_y, int iter, int *output_array)
{
	// make a RGB version of the template image
	unsigned char img_rgb[size_x * size_y * 3];
	int i;

	// crear directorio output si no exsiste
	struct stat st = {0};

	if (stat("./output", &st) == -1)
	{
		mkdir("./output", 0700);
		printf("Created new \"output\" directory\n");
	}

	// generar pixeles de la imagen
	printf("Generating \"output/image.png\" image...\n");
	for (i = 0; i < size_x * size_y; i++)
	{
		int rgb_val = ceil((output_array[i] / iter) * 255);
		img_rgb[i * 3 + 0] = rgb_val;
		img_rgb[i * 3 + 1] = rgb_val;
		img_rgb[i * 3 + 2] = rgb_val;
	}

	// generar el output de la imagen
	stbi_write_png("output/image.png", size_x, size_y, 3, img_rgb, size_x * 3);
	//stbi_write_bmp("output/image.bmp", size_x, size_y, 3, img_rgb);
	//stbi_write_jpg("output/image.jpg", size_x, size_y, 3, img_rgb, 95);

	printf("output/image.png generated Successfully!\n");
}

int main(int argc, char **argv)
{
	int desired_width = 1000, max_iterations = 300, generate_image = 0;
	int *output;

	// obtener argumentos proporcionados en tiempo de ejecucion
	if (argc > 1)
	{
		desired_width = atoi(argv[1]);
	}
	if (argc > 2)
	{
		max_iterations = atoi(argv[2]);
	}
	if (argc > 3)
	{
		generate_image = atoi(argv[3]);
	}

	// Calculate the Julia set using a pure C solution
	output = calc_pure_c(desired_width, max_iterations);

	//generate output image
	if (generate_image){
		if(desired_width > 1670) {
			printf("Max image size permited 1670x1670 \n");
		}
		write_image(size_x, size_y, max_iterations, output);
	}
}
