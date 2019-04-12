#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h> //allow for uint8_t

// stops warnings about file opening
#pragma warning(disable : 4996)

#define STD_DIV .2


#define WIDTH 28
#define HIGHT 28
#define INPUTS WIDTH * HIGHT
#define HIDDEN_NODES 40
#define OUTPUTS 10
#define numberOfSamples 60000
#define numberOfSamples_test 10000
#define numberOfSamples_test 10000
#define magicNumber_train 2051

int readint(FILE* img);

int main()
{
	uint8_t label;
	uint8_t *ins = calloc(INPUTS, sizeof(uint8_t));



	int i, j, l;
	int m;
	int testCorrect = 0;
	int maxloc = 0;
	double maxout = 0;


	double ** w = malloc(INPUTS * sizeof(double*));
	for (i = 0; i < INPUTS; i++)
	{
		w[i] = malloc(HIDDEN_NODES * sizeof(double));
	}

	double ** u = malloc(HIDDEN_NODES * sizeof(double*));
	for (i = 0; i < HIDDEN_NODES; i++)
	{
		u[i] = malloc(OUTPUTS * sizeof(double));
	}

	//hidden biases
	double* b = malloc(HIDDEN_NODES * sizeof(double));

	//output biases
	double* c = malloc(HIDDEN_NODES * sizeof(double));

	double * s = malloc(HIDDEN_NODES * sizeof(double));
	double * y = malloc(HIDDEN_NODES * sizeof(double));
	double * r = malloc(HIDDEN_NODES * sizeof(double));
	double * z = malloc(OUTPUTS * sizeof(double));
	double * err = malloc(OUTPUTS * sizeof(double));

	double * der = malloc(HIDDEN_NODES * sizeof(double));
	double * dzdr = malloc(OUTPUTS * sizeof(double));
	double * dyds = malloc(HIDDEN_NODES * sizeof(double));

	double * x = malloc(INPUTS * sizeof(double));
	int * t = malloc(OUTPUTS * sizeof(int));



	//read inputs
	printf("Reading Weights!\n");

	FILE* weights = fopen("weights", "rb");


	//print final weights to file:
	for (m = 0; m < HIDDEN_NODES; m++)
	{
		for (l = 0; l < INPUTS; l++)
		{
			fread( &(w[l][m]),sizeof(double),1, weights);
		}
		fread(&(b[m]), sizeof(double), 1, weights);
	}

	//update u and c change limits
	for (m = 0; m < OUTPUTS; m++)
	{
		for (l = 0; l < HIDDEN_NODES; l++)
		{
			
			fread(&(u[l][m]), sizeof(double), 1, weights);
		}
		
		fread(&(c[m]), sizeof(double), 1, weights);
	}
	fclose(weights);


	printf("Testing Runing\n");

	printf("Please copy weights file into this folder!\n\n");

	//testing phase

	FILE* img = fopen("t10k-images.idx3-ubyte", "rb");
	FILE* img_label = fopen("t10k-labels.idx1-ubyte", "rb");

	readint(img_label);
	readint(img_label);

	//remove unwanted numbers
	readint(img);
	readint(img);
	readint(img);
	readint(img);

	for (i = 0; i < numberOfSamples_test; i++)
	{

		fread(ins, sizeof(uint8_t), INPUTS, img);
		for (l = 0; l < INPUTS; l++)
			x[l] = ins[l] / 255.0;

		fread(&label, sizeof(label), 1, img_label);
		for (l = 0; l < OUTPUTS; l++)
		{
			if (l == label)
				t[l] = 1;
			else
				t[l] = 0;

		}


		//forward prop

		for (j = 0; j < HIDDEN_NODES; j++)
		{
			s[j] = 0;
			for (l = 0; l < INPUTS; l++)
			{
				s[j] += w[l][j] * x[l];
			}
			s[j] += b[j];
		}

		for (j = 0; j < HIDDEN_NODES; j++)
		{
			y[j] = 1 / (1 + exp(-s[j]));
		}

		for (j = 0; j < OUTPUTS; j++)
		{
			r[j] = 0;
			for (l = 0; l < HIDDEN_NODES; l++)
			{
				r[j] += u[l][j] * y[l];
			}
			r[j] += c[j];
		}

		for (j = 0; j < OUTPUTS; j++)
		{
			z[j] = 1 / (1 + exp(-r[j]));
		}

		maxloc = 0;
		maxout = z[0];
		for (m = 0; m < OUTPUTS; m++)
		{
			if (z[m] > maxout)
			{
				maxloc = m;
				maxout = z[m];
			}
		}

		if (maxloc == label)
		{
			testCorrect++;
		}


	}
	printf("testCorrect = %f\n", testCorrect / (double)numberOfSamples_test * 100);
	testCorrect = 0;




	printf("freeing dynamic memeory\n");

	for (i = 0; i < INPUTS; i++)
	{
		free(w[i]);
	}
	free(w);
	for (i = 0; i < HIDDEN_NODES; i++)
	{
		free(u[i]);
	}
	free(u);
	free(b);
	free(c);
	free(s);
	free(y);
	free(r);
	free(z);
	free(err);
	free(der);
	free(dzdr);
	free(dyds);
	free(x);
	free(t);


	system("pause");
	return(0);
}

int readint(FILE* img)
{
	uint8_t n;
	int x = 0;
	int i = 0;
	for (; i < 4; i++)
	{
		fread(&n, sizeof(n), 1, img);
		x |= n << (8 * (3 - i));
	}
	return(x);
}