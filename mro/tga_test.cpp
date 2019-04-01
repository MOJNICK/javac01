#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>

#include "Ttiming.h"
#include "TPGM.h"
#include <cmath>
#include <memory>

TTiming tt;//klasa do mierzenia czasu wykonywania siê poszczególnych funkcji

bool replace(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

template <class DataType>
class Image
{
	std::unique_ptr<DataType> data_;
	int rows_;
	int cols_;
public:
	Image(DataType* data, int rows, int cols)
	{
		data_.reset(data);
		rows_ = rows;
		cols_ = cols;

	}
};

template<typename T>
double** integrate_image(T** data, int rows, int cols)
{
	double** integrated = new double*[rows];
    integrated[0] = new double[cols*rows];

	for (auto i = 1; i < rows; ++i)//initialize pointers to pointer
	{
		integrated[i] = integrated[i - 1] + cols;
	}

	//memcpy(integrated[0], data[0], rows*cols);
	//std::copy(data[0], data[0] + rows*cols, integrated[0]);
	
	integrated[0][0] = data[0][0];

	//init row0 and col0
	for (auto row = 1; row < rows; row++)
	{
		integrated[row][0] = data[row][0] + integrated[row - 1][0];
	}

	for (auto col = 1; col < cols; col++)
	{
		integrated[0][col] = data[0][col] + integrated[0][col - 1];
	}

	for (auto row = 1; row < rows; row++)
	{
		for (auto col = 1; col < cols; col++)
		{
			integrated[row][col] =
				+data[row][col]
				+ integrated[row - 1][col]
				+ integrated[row][col - 1]
				- integrated[row - 1][col - 1];
		}
	}
	return integrated;
}

double** createSquareIntegral(unsigned char** data, int rows, int cols)
{
	double** squaredData = new double*[rows];
	squaredData[0] = new double[rows*cols];
	for (auto i = 1; i < rows; ++i)//initialize pointers to pointer
	{
		squaredData[i] = squaredData[i - 1] + cols;
	}

	for (int i = 0; i < rows*cols; i++)
	{
		squaredData[0][i] = data[0][i] * data[0][i];
	}

	auto integrateOfSquared = integrate_image(squaredData, rows, cols);
	return integrateOfSquared;
}

double getSumUnderKernel(double** integralImage, int row, int col, int kernelSize)
{
	//navigate to down right
	int ra = row + kernelSize;
	int ca = col + kernelSize;

	int rb = row - kernelSize;
	int cb = col + kernelSize;

	int rc = row + kernelSize;
	int cc = col - kernelSize;

	int rd = row - kernelSize;
	int cd = col - kernelSize;

	double a = *(integralImage[ra] + ca);
	double b = *(integralImage[rb] + cb);
	double c = *(integralImage[rc] + cc);
	double d = *(integralImage[rd] + cd);
	double result = a - b - c + d;
	return result;
}

double naiveSigma(double** integralImage, double** integralImageOfSquared, int row, int col, int kernelSize)
{
	double variance = 0;
	double kernelArea = (2 * kernelSize + 1)*(2 * kernelSize + 1);
	auto medianOryginal = getSumUnderKernel(integralImage, row, col, kernelSize) / kernelArea;
	auto medianSquared  = getSumUnderKernel(integralImageOfSquared, row, col, kernelSize) / kernelArea;
	auto medianOryginalSQ = medianOryginal * medianOryginal;

	variance = medianSquared - (medianOryginalSQ);
	auto sigma = sqrt(variance);
	return sigma;
}

double getNeighboursAverageFromIntegral(double** integralImage, int row, int col, int kernelSize)
{
	double result = getSumUnderKernel(integralImage, row, col, kernelSize);
	result /= ((2 * kernelSize + 1)*(2 * kernelSize + 1));

	return result;
}

double calcAverage(unsigned char* data, unsigned int length)
{
	double sum = 0;
	for (unsigned int i = 0; i < length; ++i)
	{
		sum += data[i];
	}
	auto avg = sum / length;
	return avg;
}

double calcNeighboursAverage(unsigned char** data, int row, int col, int kernelSize)
{
	double sum = 0;
	for (int i = -kernelSize; i < kernelSize; ++i)
	{
		for (int j = -kernelSize; j < kernelSize; ++j)
		{
			sum += data[row + i][col + j];
		}
	}
	auto avg = sum / ((2 * kernelSize + 1)*(2 * kernelSize + 1));
	return avg;
}



auto addBlackBorders(unsigned char** data, int rows, int cols, int kernelSize)
{
	int newImageRows = rows + 2 * kernelSize;
	int newImageCols = rows + 2 * kernelSize;
	int newImageDataSize = newImageRows * newImageCols;
	unsigned char** newImage = new unsigned char*[newImageRows];
	newImage[0] = new unsigned char[newImageDataSize]();

	for (int i = 1; i < newImageRows; i++)
		newImage[i] = newImage[i - 1] + newImageCols;

	//copy data to newImage
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			newImage[i + kernelSize][j + kernelSize] = data[i][j];
		}
	}

	Image<unsigned char> image(newImage[0], newImageRows, newImageCols);
	return image;
}

int main(int argc, char **argv)
{
	int rows, cols;
	int max_color;
	int hpos, i, j;

	std::string infname = "016_oyginal.ppm";

	if ((hpos = readPPMB_header(infname.c_str(), &rows, &cols, &max_color)) <= 0)	   exit(1);

	unsigned char **R = new unsigned char*[rows];
	R[0] = new unsigned char[rows*cols];
	for (int i = 1; i < rows; i++)
		R[i] = R[i - 1] + cols;

	unsigned char **G = new unsigned char*[rows];
	G[0] = new unsigned char[rows*cols];
	for (int i = 1; i < rows; i++)
		G[i] = G[i - 1] + cols;

	unsigned char **B = new unsigned char*[rows];
	B[0] = new unsigned char[rows*cols];
	for (int i = 1; i < rows; i++)
		B[i] = B[i - 1] + cols;

	if (readPPMB_data(R[0], G[0], B[0], infname.c_str(), hpos, rows, cols, max_color) == 0)	   exit(1);

	//zamiana obrazu kolorowego na skalê szaroœci

	//przzygotowanie obrazu grayscale
	unsigned char **a = new unsigned char*[rows];
	a[0] = new unsigned char[rows*cols];
	for (int i = 1; i < rows; i++)
		a[i] = a[i - 1] + cols;

	unsigned char _r, _g, _b, gray_value;

	for (i = 0; i< rows; ++i) {
		for (j = 0; j< cols; ++j) {
			_r = R[i][j];
			_g = G[i][j];
			_b = B[i][j];
			gray_value = unsigned char((0.299 * _r) + (0.587 * _g) + (0.114 * _b));
			a[i][j] = gray_value;
		}
	}


	//przygotowanie czarno-bialej tablicy wyjsciowej
	unsigned char **b = new unsigned char*[rows];
	b[0] = new unsigned char[rows*cols];
	for (int i = 1; i < rows; i++)
		b[i] = b[i - 1] + cols;

	tt.Begin();		//start to measure the time

					//auto avg = calcAverage(a[0], rows*cols);//128;
	const int kernelSize = 7;
	//calcNeighboursAverage(unsigned char** data, int row, int col, int kernelSize)
	//a = addBlackBorders(a, rows, cols, kernelSize).data;
	double** integratedImage = integrate_image(a, rows, cols);
	double** integratedImageOfSquared = createSquareIntegral(a, rows, cols);
	int RRR = 128; //maxvariance;
	for (i = kernelSize; i< rows - kernelSize; ++i) {
		for (j = kernelSize; j< cols - kernelSize; ++j) {
			
			/*double avg = calcNeighboursAverage(a, i, j, kernelSize);
			double threshold = avg;*/
			double avg = getNeighboursAverageFromIntegral(integratedImage, i, j, kernelSize);
			double threshold = avg;
			/*
			double avg = getNeighboursAverageFromIntegral(integratedImage, i, j, kernelSize);
			auto variance = naiveSigma(integratedImage, integratedImageOfSquared, i, j, kernelSize);
			double threshold = avg * (1+0.1*(((variance/(RRR))-1)));*/

			b[i][j] = (a[i][j] > threshold) ? 255 : 0;
		}
	}

	double elapsed = tt.End();	//stop and read elapsed time in ms (miliseconds)

	std::string outfname = "ouTBRADLEY.pgm";
	//replace(outfname, ".ppm", "_col2gray_simple.pgm");

	if (writePGMB_image(outfname.c_str(), b[0], rows, cols, 255) == 0)	   exit(1);

	delete[] R[0]; delete[] R;
	delete[] G[0]; delete[] G;
	delete[] B[0]; delete[] B;
	delete[] a[0]; delete[] a;
	delete[] b[0]; delete[] b;

	printf("czas binaryzacji : %f ms", elapsed);
	getchar();

	return 0;
}
