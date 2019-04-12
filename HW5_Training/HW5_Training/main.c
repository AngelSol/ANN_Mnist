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


void error_check(FILE* img, int* itemNumber, int* rows, int* cols);
void err_exit();
int readint(FILE* img);

double getrand();


int main()
{

	unsigned int sec = time(NULL);
	srand(sec);
	int select;

	int epochs = 10;
	int rows, cols, itemNumber;

	//initially high learning rate. then lower after first epoch
	double eta = .4; 

	uint8_t label;
	uint8_t *ins = calloc(INPUTS , sizeof(uint8_t) );

	FILE* img = fopen("train-images.idx3-ubyte", "rb");
	FILE* img_label = fopen("train-labels.idx1-ubyte", "rb");
	readint(img_label);
	readint(img_label);

	
	error_check(img, &itemNumber, &rows, &cols);

	int i, k, j, l;
	int m, n;

	int trainCorrect = 0;
	int testCorrect = 0;
	int maxloc = 0;
	double maxout = 0;


	double *** w = malloc(INPUTS*sizeof(double**));
	for (i = 0; i < INPUTS; i++)
	{
		w[i] = malloc(HIDDEN_NODES * sizeof(double*));
		for (j = 0; j < HIDDEN_NODES; j++)
		{
			w[i][j] = malloc(2*sizeof(double));
			w[i][j][0] = getrand();
		}
	}

	double *** u = malloc(HIDDEN_NODES * sizeof(double**));
	for (i = 0; i < HIDDEN_NODES; i++)
	{
		u[i] = malloc(OUTPUTS * sizeof(double*));
		for (j = 0; j < OUTPUTS; j++)
		{
			u[i][j] = malloc(2 * sizeof(double));
			u[i][j][0] = getrand();
		}
	}

	//hidden biases
	double** b = malloc(HIDDEN_NODES * sizeof(double*));
	for (i = 0; i < HIDDEN_NODES; i++)
	{
		b[i] = malloc(2 * sizeof(double));
		b[i][0] = getrand();
	}

	//output biases
	double** c = malloc(OUTPUTS * sizeof(double*));
	for (i = 0; i < OUTPUTS; i++)
	{
		c[i] = malloc(2 * sizeof(double));
		c[i][0] = getrand();
	}



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







	printf("Plese wait until training is finished.\n");

	printf("Sigmoid is being used \nSee weights file for outputs\nCopy that file into the  test & machine folders!\n\n");


		for (k = 0; k < epochs; k++)
		{
			for (i = 0; i < numberOfSamples; i++)
			{
				
				fread(ins, sizeof(uint8_t), INPUTS, img);				//get full array from image file
				for (l = 0; l < INPUTS; l++)
				{
					x[l] = ins[l] / 255.0;								//convert to double and normalize between [0,1]
				}

				fread(&label, sizeof(label), 1, img_label);				//get label
				
				for (l = 0; l < OUTPUTS; l++)
				{
					if (l == label)										//create output array
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
						s[j] += w[l][j][0] * x[l];						//sum s
					}
					s[j] += b[j][0];
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
						r[j] += u[l][j][0] * y[l];
					}
					r[j] += c[j][0];
				}

				for (j = 0; j < OUTPUTS; j++)
				{
					z[j] = 1 / (1 + exp(-r[j]));

				}


				//calculate partial part of ders and error

				for (j = 0; j < OUTPUTS; j++)
				{
					err[j] = z[j] - (double)t[j];					 //caculate errors
					dzdr[j] = z[j] * (1 - z[j]);

				}


				for (j = 0; j < HIDDEN_NODES; j++)
				{
					dyds[j] = y[j] * (1 - y[j]);
				}

				//back prop 

				for (m = 0; m < HIDDEN_NODES; m++)
				{
					der[m] = 0;
					for (n = 0; n < OUTPUTS; n++)
					{
						der[m] += err[n] * dzdr[n] * u[m][n][0] * dyds[m];

					}

					for (l = 0; l < INPUTS; l++)
					{
						w[l][m][1] = w[l][m][0] - eta * der[m] * x[l];
					}
					b[m][1] = b[m][0] - eta * der[m];

				}

				//update u and c change limits
				for (m = 0; m < OUTPUTS; m++)
				{
					for (l = 0; l < HIDDEN_NODES; l++)
					{
						u[l][m][1] = u[l][m][0] - eta * err[m] * dzdr[m] * y[l];
					}
					c[m][1] = c[m][0] - eta * err[m] * dzdr[m];
				}


				//replace old weights 
				for (m = 0; m < HIDDEN_NODES; m++)
				{
					for (l = 0; l < INPUTS; l++)
					{
						w[l][m][0] = w[l][m][1];

					}
					b[m][0] = b[m][1];
				}

				for (m = 0; m < OUTPUTS; m++)
				{
					for (l = 0; l < HIDDEN_NODES; l++)
					{
						u[l][m][0] = u[l][m][1];
					}
					c[m][0] = c[m][1];
				}

				//get output
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
					trainCorrect++;
				}


			}//end of samples

		//eta change!
			//.15 - .17???
			eta = .1;

			printf("Epoch = %i trainCorrect = %f\n", k, trainCorrect / (double)numberOfSamples *100);
			trainCorrect = 0;
			
			//testing phase
			fclose(img);
			fclose(img_label);

			img = fopen("t10k-images.idx3-ubyte", "rb");
			img_label = fopen("t10k-labels.idx1-ubyte", "rb");

			readint(img_label);
			readint(img_label);

			//remove unwanted numbers
			readint(img);
			readint(img);
			readint(img);
			readint(img);

			for (i = 0; i < numberOfSamples_test ; i++)
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
						s[j] += w[l][j][0] * x[l];
					}
					s[j] += b[j][0];
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
						r[j] += u[l][j][0] * y[l];
					}
					r[j] += c[j][0];
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
		printf("testCorrect = %f\n", testCorrect / (double) numberOfSamples_test * 100);
		testCorrect = 0;
		trainCorrect = 0;

		//restart files
		fclose(img);
		fclose(img_label);
		img = fopen("train-images.idx3-ubyte", "rb");
		img_label = fopen("train-labels.idx1-ubyte", "rb");

		readint(img_label);
		readint(img_label);

		error_check(img, &itemNumber, &rows, &cols);

		}//#noofepochs

		printf("printing outputs!\n");

		FILE* weights = fopen("weights", "wb");
		//print final weights to file:
		for (m = 0; m < HIDDEN_NODES; m++)
		{
			for (l = 0; l < INPUTS; l++)
			{
				//fprintf(weights, "%f", w[l][m][0]);
				fwrite(&w[l][m][0], sizeof(double), 1, weights);
			}

			//fprintf(weights, "%f", b[m][0]);
			fwrite(&b[m][0], sizeof(double), 1, weights);
		}

		//update u and c change limits
		for (m = 0; m < OUTPUTS; m++)
		{
			for (l = 0; l < HIDDEN_NODES; l++)
			{
				//fprintf(weights, "%f", u[l][m][0]);
				fwrite(&u[l][m][0], sizeof(double), 1, weights);
			}
			//fprintf(weights, "%f", c[m][0]);
			fwrite(&c[m][0], sizeof(double), 1, weights);


		}
		fclose(weights);


		printf("freeing dynamic memeory\n");
		
		for (i = 0; i < INPUTS; i++)
		{
			for (j = 0; j < HIDDEN_NODES; j++)
			{
				free(w[i][j]);

			}
			free(w[i]);
		}
		free(w);
		for (i = 0; i < HIDDEN_NODES; i++)
		{
			for (j = 0; j < OUTPUTS; j++)
			{
				free(u[i][j]);			
			}
			free(u[i]);
		}
		free(u);

		for (i = 0; i < HIDDEN_NODES; i++)
		{
			free(b[i]);
			
		}
		free(b);

		for (i = 0; i < OUTPUTS; i++)
		{
			free(c[i]);
		}
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

double getrand() {
	
		int i;
		double sum = 0.0;
		for (i = 0; i < 12; i++)
		{
			sum += (((double)(rand())) / ((double)(RAND_MAX)));
		}
		
	//return((((double)(rand())) / ((double)(RAND_MAX))) - 0.5);
	return(STD_DIV*(sum - 6.0));
}

void err_exit()
{
	printf("Press enter to exit\n");
	while (getchar() != '\n');
	exit(1);
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


void error_check(FILE* img, int* itemNumber, int* rows, int* cols)
{

	if (img == NULL)
	{
		printf("Error: File not found\n");
		err_exit();
	}


	if (readint(img) != magicNumber_train)
	{
		printf("Error: Wrong Magic number\n");
		err_exit();
	}

	*itemNumber = readint(img);
	if (*itemNumber != numberOfSamples)
	{
		printf("Error: Wrong number of items\n");
		err_exit();
	}

	*rows = readint(img);
	if (*rows != HIGHT)
	{
		printf("Error: Wrong number of rows\n");
		err_exit();
	}
	*cols = readint(img);
	if (*cols != WIDTH)
	{
		printf("Error: Wrong number of cols\n");
		err_exit();
	}

}

