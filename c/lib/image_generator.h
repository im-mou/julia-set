#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

// Ref: https://github.com/nothings/stb/blob/master/tests/image_write_test.c
#define STB_IMAGE_WRITE_STATIC
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// funcion para generar un array con colores aleatorios en formato RGB
unsigned int * rgb_array(int iter)
{
    srand(cabs(-1.8+1.8*I));
    // srand(time(0));
    unsigned int * _rbg_ouput = (unsigned int *)malloc((iter * 3) * sizeof(unsigned int));
    for (int i = 0; i < iter; i++)
    {
        // Push random integer between 0 and 255
        _rbg_ouput[i * 3 + 0] = floor(((double)rand() / (double)RAND_MAX) * 256);
		_rbg_ouput[i * 3 + 1] = floor(((double)rand() / (double)RAND_MAX) * 256);
		_rbg_ouput[i * 3 + 2] = floor(((double)rand() / (double)RAND_MAX) * 256);
    }

    return _rbg_ouput;
}


// funcio para crear una imagen
void generate_image(int size_x, int size_y, int iter, int *output_array, int image_color)
{
	// make a RGB version of the template image
	unsigned char rgb_pixel[size_x * size_y * 3];

	// populate an array with rgb colors to generate the image
	unsigned int * rgb_colors;
	if(image_color == 1) { // colors
		rgb_colors = rgb_array(iter);
	} 
	if(image_color == 2) { // black and white
		rgb_colors = (unsigned int *)malloc((iter * 3) * sizeof(unsigned int));
		for (int i = 0; i < iter*3; i++) {
			rgb_colors[i] = 255;
		};
	}

	// crear directorio output si no exsiste
	struct stat st = {0};
	if (stat("./output", &st) == -1)
	{
		mkdir("./output", 0700);
		printf("Created new \"output\" directory\n");
	}

	// generar pixeles de la imagen
	printf("Generating \"output/image.png\" image...\n");
	for (int i = 0; i < size_x * size_y; i++)
	{
		//if(output_array[i]==2){
			rgb_pixel[i * 3 + 0] = rgb_colors[(output_array[i]*3) + 0];
			rgb_pixel[i * 3 + 1] = rgb_colors[(output_array[i]*3) + 1];
			rgb_pixel[i * 3 + 2] = rgb_colors[(output_array[i]*3) + 2];
		//}
	}

	// generar el output de la imagen en el formato deseado
	stbi_write_png("output/image.png", size_x, size_y, 3, rgb_pixel, size_x * 3);
	//stbi_write_bmp("output/image.bmp", size_x, size_y, 3, rgb_pixel);
	//stbi_write_jpg("output/image.jpg", size_x, size_y, 3, img_rgb, 95);

	printf("output/image.png generated Successfully!\n");
}