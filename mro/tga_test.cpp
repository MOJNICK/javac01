#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>

#include "Ttiming.h"
#include "TPGM.h"
#include <cmath>
#include <memory>
#include <algorithm>

TTiming tt;//klasa do mierzenia czasu wykonywania siê poszczególnych funkcji

bool replace(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

template <class DataType>
class Row
{
	DataType* row_;
public:
	Row(DataType* row) : row_{ row } {}
	DataType& operator[](int col) { return *(row_ + col); }
	const DataType& operator[](int col) const { return *(row_ + col); }
};

template <class DataType>
class Image
{
public:
	int rows_;
	int cols_;
private:
	std::unique_ptr<DataType[]> data_;
public:
	using DataPtrType = DataType*;

	Image(DataPtrType data, int rows, int cols) : data_{ data }, rows_{ rows }, cols_{cols} {}
	Image(std::unique_ptr<DataType[]> data, int rows, int cols) : data_{ std::move(data) }, rows_{ rows }, cols{ cols } {}
	Image(const Image& img) : data_{ new DataType[rows_ * cols_] }, rows_{ img.rows_ }, cols_{ img.cols_ }
	{
		std::copy(
			img.getDataPtr(),
			img(rows_, cols_),
			this->getDataPtr());//gdzies podczas kopiowania wyskakuje bug
	}

	Image(Image&& img) : data_{ std::move(img.data_) }, rows_{ img.rows_ }, cols_{ img.cols_ } {}

	DataPtrType operator()(int row, int col) { return data_.get() + row * cols_ + col; }
	const DataPtrType operator()(int row, int col) const { return data_.get() + row * cols_ + col; }
	Row<DataType> operator[](int row) { return Row<DataType>(this->operator()(row, 0)); }
	const Row<DataType> operator[](int row) const { return Row<DataType>(this->operator()(row, 0)); }

	DataPtrType getDataPtr() { return data_.get(); };
	const DataPtrType getDataPtr() const { return data_.get(); };
	
	DataPtrType begin() { return data_.get(); }
	DataPtrType end() { return data_.get() + rows_ * cols_; }
	const DataPtrType cbegin() const { return data_.get(); }
	const DataPtrType cend() const { return data_.get() + rows_ * cols_; }
};

Image<char> img(nullptr, 0, 0);

template<typename DataType>
Image<DataType> replicateBorders(const Image<DataType>& img, const unsigned int borderSize)
{
	int newImageRows = img.rows_ + 2 * borderSize;
	int newImageCols = img.cols_ + 2 * borderSize;
	int newImageDataSize = newImageRows * newImageCols;

	Image<DataType> newImage(new DataType[newImageDataSize], newImageRows, newImageCols);

	//replicate left and right
	for (int row = 0; row < img.rows_; ++row)
	{
		auto imgRow = img(row, 0);
		auto newImageRow = newImage(row, 0);

		std::reverse_copy(imgRow, imgRow + borderSize, newImageRow);
		std::copy(imgRow, imgRow + img.cols_, newImageRow + borderSize);
		std::reverse_copy(imgRow + img.cols_ - borderSize, imgRow + img.cols_, newImageRow);
	}

	//replicate up
	const auto first = newImage(borderSize, 0);
	for (unsigned int i = 0; i < borderSize; ++i)
	{
		auto last = newImage(borderSize + borderSize - i, 0);
		auto d_first = newImage(i,0);
		if (first == last) break;
		for (int col = 0; col < img.cols_; ++col)
		{
			*d_first = *last;
		}
	}

	//replicate down
	return newImage;
}


template Image<char> replicateBorders(const Image<char>&, const unsigned int);

template<typename DataType>
Image<double> integralImage(const Image<DataType>& data)
{
	//double** integrated = new double*[rows];
	Image<double> integrated{ new double[data.cols_ * data.rows_], data.rows_, data.cols_ };
    //integrated[0] = new double[cols*rows];

	/*for (auto i = 1; i < rows; ++i)//initialize pointers to pointer
	{
		integrated[i] = integrated[i - 1] + cols;
	}*/
	
	integrated[0][0] = data[0][0];

	for (auto row = 1; row < integrated.rows_; row++)
	{
		integrated[row][0] = data[row][0] + integrated[row - 1][0];
	}

	for (auto col = 1; col < integrated.cols_; col++)
	{
		integrated[0][col] = data[0][col] + integrated[0][col - 1];
	}

	for (auto row = 1; row < integrated.rows_; row++)
	{
		for (auto col = 1; col < integrated.cols_; col++)
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
template Image<double> integralImage(const Image<char>& data);

Image<double> createSquareIntegral(const Image<unsigned char>& integral)
{
	Image<double> squaredData{ new double[integral.rows_ * integral.cols_], integral.rows_, integral.cols_ };

	auto dataPtr = squaredData.getDataPtr();
	std::transform(integral.cbegin(), integral.cend(), squaredData.begin(), [](unsigned char& val) { return val * val; });
	
	auto integrateOfSquared = integralImage(squaredData);
	return integrateOfSquared;
}

double getSumUnderKernel(const Image<double>& integralImage, int row, int col, int kernelSize)
{
	//navigate to down right
	int ra = row + kernelSize;
	int ca = col + kernelSize;

	int rb = row - kernelSize - 1;
	int cb = col + kernelSize;

	int rc = row + kernelSize;
	int cc = col - kernelSize - 1;

	int rd = row - kernelSize - 1;
	int cd = col - kernelSize - 1;

	double a = integralImage[ra][ca];
	double b = integralImage[rb][cb];
	double c = integralImage[rc][cc];
	double d = integralImage[rd][cd];
	double result = a - b - c + d;
	return result;
}

double calcNeighboursAverage(const Image<unsigned char>& data, int row, int col, int kernelSize)
{
	double sum = 0;
	for (int i = -kernelSize; i <= kernelSize; ++i)
	{
		for (int j = -kernelSize; j <= kernelSize; ++j)
		{
			sum += data[row + i][col + j];
		}
	}
	auto avg = sum / ((2 * kernelSize + 1)*(2 * kernelSize + 1));
	return avg;
}

double slowSigma(const Image<unsigned char>& data, int row, int col, int kernelSize, double* avg)
{
	*avg = calcNeighboursAverage(data, row, col, kernelSize);
	
	double sum = 0;
	for (int i = -kernelSize; i <= kernelSize; ++i)
	{
		for (int j = -kernelSize; j <= kernelSize; ++j)
		{
			auto pixelValue = data[row + i][col + j];
			sum += (pixelValue - *avg) * (pixelValue - *avg);
		}
	}
	auto variance = sum / ((2 * kernelSize + 1)*(2 * kernelSize + 1));
	auto sigma = sqrt(variance);

	return sigma;
}

double naiveSigma(const Image<double>& integralImage, const Image<double>& integralImageOfSquared, int row, int col, int kernelSize)
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

double getNeighboursAverageFromIntegral(const Image<double>& integralImage, int row, int col, int kernelSize)
{
	double result = getSumUnderKernel(integralImage, row, col, kernelSize);
	result /= ((2 * kernelSize + 1)*(2 * kernelSize + 1));

	return result;
}

double calcAverage(const Image<unsigned char>& data, unsigned int length)
{
	double sum = 0;
	for (unsigned int i = 0; i < length; ++i)
	{
		sum += data.getDataPtr()[i];
	}
	auto avg = sum / length;
	return avg;
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

	Image<unsigned char> input{a[0], rows, cols};

	//przygotowanie czarno-bialej tablicy wyjsciowej
	unsigned char **b = new unsigned char*[rows];
	b[0] = new unsigned char[rows*cols];
	for (int i = 1; i < rows; i++)
		b[i] = b[i - 1] + cols;

	Image<unsigned char> output{ b[0], rows, cols };

	tt.Begin();		//start to measure the time

					//auto avg = calcAverage(a[0], rows*cols);//128;
	const int kernelSize = 7;
	//calcNeighboursAverage(unsigned char** data, int row, int col, int kernelSize)
	//a = addBlackBorders(a, rows, cols, kernelSize).data;
	Image<double> integratedImage = integralImage(input);
	Image<double> integratedImageOfSquared = createSquareIntegral(input);
	int RRR = 128; //maxvariance;
	for (i = kernelSize+1; i< rows - kernelSize; ++i) {
		for (j = kernelSize+1; j< cols - kernelSize; ++j) {
			
			/*//outAvgNonIntBradley
			double avg = calcNeighboursAverage(input, i, j, kernelSize);
			double threshold = 0.95 * avg;*/
			
			/*//outAVGIntegralBradley
			double avg = getNeighboursAverageFromIntegral(integratedImage, i, j, kernelSize);
			double threshold = 0.8 * avg;*/
			
			//outAvgIntegralSauvola
			double avg = getNeighboursAverageFromIntegral(integratedImage, i, j, kernelSize);
			auto sigma = naiveSigma(integratedImage, integratedImageOfSquared, i, j, kernelSize);
			double threshold = avg * (1+0.1*(((sigma/(RRR))-1)));

			/*//outAvgNonIntSauvola
			double avg;// = calcNeighboursAverage(a, i, j, kernelSize);
			auto sigma = slowSigma(input, i, j, kernelSize, &avg);
			double threshold = avg * (1+0.1*(((sigma/(RRR))-1)));
			*/
			output[i][j] = (input[i][j] > threshold) ? 255 : 0;

//			auto sigma = naiveSigma(integratedImage, integratedImageOfSquared, i, j, kernelSize);
//			auto sigma2 = slowSigma(a, i, j, kernelSize);
		}
	}

	double elapsed = tt.End();	//stop and read elapsed time in ms (miliseconds)

	std::string outfname = "2outAvgNonIntSauvola.pgm";
	//replace(outfname, ".ppm", "_col2gray_simple.pgm");

	if (writePGMB_image(outfname.c_str(), output.getDataPtr(), rows, cols, 255) == 0)	   exit(1);

	delete[] R[0]; delete[] R;
	delete[] G[0]; delete[] G;
	delete[] B[0]; delete[] B;
	delete[] a[0]; delete[] a;
	delete[] b[0]; delete[] b;

	printf("czas binaryzacji : %f ms", elapsed);
	getchar();

	return 0;
}
