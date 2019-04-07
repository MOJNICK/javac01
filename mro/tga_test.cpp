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
#include <iostream>
#include <utility>
#include <type_traits>

TTiming tt;//klasa do mierzenia czasu wykonywania siê poszczególnych funkcji

bool replace(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

template<class T>
class Image;


template<class T>
struct is_image : std::false_type {};

template<class T>
struct is_image<Image<T>> : std::true_type {};

template <class DataType>
class Image
{
public:
	unsigned int rows_;
	unsigned int cols_;
private:
	std::unique_ptr<DataType[]> data_;
public:
	using DataPtrType = DataType*;

	Image(unsigned int rows, unsigned int cols) : data_{ new DataType[rows * cols] }, rows_{ rows }, cols_{cols} { std::cout << "cons_"; }
	template<class T>
	Image(const Image<T>& img) : data_{ new DataType[img.rows_ * img.cols_] }, rows_{ img.rows_ }, cols_{ img.cols_ }
	{
		std::cout << "cp_";
		std::copy(
			img.cbegin(),
			img.cend(),
			this->begin());
	}

	Image(Image&& img) : data_{ std::move(img.data_) }, rows_{ img.rows_ }, cols_{ img.cols_ } { std::cout << "mv_"; }

	Image& operator=(const Image& img) = delete;
	Image& operator=(Image&& img) { data_ = std::move(img.data_); rows_ = img.rows_; cols_ = img.cols_; std::cout << "mv="; return *this; }
	DataPtrType operator()(int row, int col) { return this->getDataPtr() + row * cols_ + col; }
	const DataPtrType operator()(int row, int col) const { return this->getDataPtr() + row * cols_ + col; }
	DataPtrType operator[](int row) { return (this->operator()(row, 0)); }
	const DataPtrType operator[](int row) const { return (this->operator()(row, 0)); }

	DataPtrType getDataPtr() { return data_.get(); };
	const DataPtrType getDataPtr() const { return data_.get(); };
	
	DataPtrType begin() { return data_.getDataPtr(); }
	DataPtrType end() { return data_.getDataPtr() + rows_ * cols_; }
	const DataPtrType cbegin() const { return data_.getDataPtr(); }
	const DataPtrType cend() const { return data_.getDataPtr() + rows_ * cols_; }

	void replicateBorders(const unsigned int borderSize)
	{
		//replicate left and right
		for (unsigned int row = borderSize; row < this->rows_ - borderSize; ++row)
		{
			auto imgRow = this->operator()(row, 0);
			std::reverse_copy(imgRow + borderSize, imgRow + borderSize + borderSize, imgRow);
			std::reverse_copy(imgRow + this->cols_ - borderSize - borderSize, imgRow + this->cols_ - borderSize, imgRow + this->cols_ - borderSize);
		}

		//replicate up
		for (unsigned int i = 0; i < borderSize; ++i)
		{
			auto src = this->operator()(borderSize + borderSize - i, 0);
			auto dst = this->operator()(i, 0);
			std::copy_n(src, this->cols_, dst);
		}

		//replicate down
		for (unsigned int i = 0; i < borderSize; ++i)
		{
		    auto src = this->operator()((this->rows_ - 1) - borderSize - i, 0);
		    auto dst = this->operator()(this->rows_ - borderSize + i, 0);
		    std::copy_n(src, this->cols_, dst);
		}
	}

	Image cropp(const unsigned int croppSize) const
	{
		Image<DataType> cropped{ this->rows_ - 2 * croppSize, this->cols_ - 2 * croppSize };
		for (auto row = 0u; row < cropped.rows_; ++row)
		{
			auto src = this->operator()(row + croppSize, croppSize);
			auto dst = cropped(row, 0);
			std::copy_n(src, cropped.cols_, dst);
		}
		return cropped;
	}
};


template<typename T>
Image<double> integralImage(T&& data)
{
	Image<double> integrated(std::forward<T>(data));
	
	integrated[0][0] = integrated[0][0];

	for (auto row = 1u; row < integrated.rows_; row++)
	{
		integrated[row][0] += integrated[row - 1][0];
	}

	for (auto col = 1u; col < integrated.cols_; col++)
	{
		integrated[0][col] += integrated[0][col - 1];
	}

	for (auto row = 1u; row < integrated.rows_; row++)
	{
		for (auto col = 1u; col < integrated.cols_; col++)
		{
			integrated[row][col] =
				+ integrated[row][col]
				+ integrated[row - 1][col]
				+ integrated[row][col - 1]
				- integrated[row - 1][col - 1];
		}
	}
	return integrated;
}

Image<double> createSquareIntegral(const Image<unsigned char>& integral)
{
	Image<double> squaredData{ integral.rows_, integral.cols_ };

	std::transform(integral.cbegin(), integral.cend(), squaredData.begin(), [](unsigned char& val) { return val * val; });
	
	auto integrateOfSquared = integralImage(std::move(squaredData));
	return integrateOfSquared;
}

double getSumUnderKernel(const Image<double>& integralImage, unsigned row, unsigned col, unsigned kernelSize)
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

double calcNeighboursAverage(const Image<unsigned char>& data, unsigned row, unsigned col, int kernelSize)
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

double slowSigma(const Image<unsigned char>& data, unsigned row, unsigned col, int kernelSize, double& avg)
{
	avg = calcNeighboursAverage(data, row, col, kernelSize);
	
	double sum = 0;
	for (int i = -kernelSize; i <= kernelSize; ++i)
	{
		for (int j = -kernelSize; j <= kernelSize; ++j)
		{
			auto pixelValue = data[row + i][col + j];
			sum += (pixelValue - avg) * (pixelValue - avg);
		}
	}
	auto variance = sum / ((2 * kernelSize + 1)*(2 * kernelSize + 1));
	auto sigma = sqrt(variance);

	return sigma;
}

double naiveSigma(const Image<double>& integralImage, const Image<double>& integralImageOfSquared, unsigned row, unsigned col, int kernelSize)
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

double getNeighboursAverageFromIntegral(const Image<double>& integralImage, unsigned row, unsigned col, unsigned kernelSize)
{
	double result = getSumUnderKernel(integralImage, row, col, kernelSize);
	result /= ((2 * kernelSize + 1)*(2 * kernelSize + 1));

	return result;
}

template<class In, class Out>
void sauvolaNonIntegral(const Image<In>& input, unsigned kernelSize, Image<Out>& output)
{
	int RRR = 128; //maxvariance;

	for (unsigned i = kernelSize; i < input.rows_ - kernelSize; ++i) {
		for (unsigned j = kernelSize; j < input.cols_ - kernelSize; ++j) {
			double avg;
			auto sigma = slowSigma(input, i, j, kernelSize, avg);
			double threshold = avg * (1 + 0.1*(((sigma / (RRR)) - 1)));

			output[i][j] = (input[i][j] > threshold) ? 255 : 0;
		}
	}
}

template<class In, class Out>
void sauvolaIntegral(const Image<In>& input, const unsigned kernelSize, Image<Out>& output)
{
	Image<double> integratedImage = integralImage(input);
	Image<double> integratedImageOfSquared = createSquareIntegral(input);
	int RRR = 128; //maxvariance;

	for (unsigned i = kernelSize + 1; i < input.rows_ - kernelSize; ++i) {
		for (unsigned j = kernelSize + 1; j < input.cols_ - kernelSize; ++j) {
			double avg = getNeighboursAverageFromIntegral(integratedImage, i, j, kernelSize);
			auto sigma = naiveSigma(integratedImage, integratedImageOfSquared, i, j, kernelSize);
			double threshold = avg * (1 + 0.1*(((sigma / (RRR)) - 1)));

			output[i][j] = (input[i][j] > threshold) ? 255 : 0;
		}
	}
}

template<class In, class Out>
void bradleyIntegral(const Image<In>& input, unsigned kernelSize, Image<Out>& output)
{
	Image<double> integratedImage = integralImage(input);

	for (unsigned i = kernelSize + 1; i < input.rows_ - kernelSize; ++i) {
		for (unsigned j = kernelSize + 1; j < input.cols_ - kernelSize; ++j) {
			double avg = getNeighboursAverageFromIntegral(integratedImage, i, j, kernelSize);
			double threshold = 0.8 * avg;

			output[i][j] = (input[i][j] > threshold) ? 255 : 0;
		}
	}
}

template<class In, class Out>
void bradleyNonIntegral(const Image<In>& input, unsigned kernelSize, Image<Out>& output)
{
	for (unsigned i = kernelSize; i < input.rows_ - kernelSize; ++i) {
		for (unsigned j = kernelSize; j < input.cols_ - kernelSize; ++j) {
			double avg = calcNeighboursAverage(input, i, j, kernelSize);
			double threshold = 0.8 * avg;

			output[i][j] = (input[i][j] > threshold) ? 255 : 0;
		}
	}
}

int main(int argc, char **argv)
{
	unsigned rows, cols;
	int max_color;
	int hpos;

	std::string infname = "016_oyginal.ppm";

	if ((hpos = readPPMB_header(infname.c_str(), &rows, &cols, &max_color)) <= 0)	   exit(1);

	Image<unsigned char> R{ rows, cols };
	Image<unsigned char> G{ rows, cols };
	Image<unsigned char> B{ rows, cols };

	if (readPPMB_data(R.getDataPtr(), G.getDataPtr(), B.getDataPtr(), infname.c_str(), hpos, rows, cols, max_color) == 0)	   exit(1);

	//przzygotowanie obrazu grayscale
	const unsigned kernelSize = 7;
	Image<unsigned char> input{ rows + 2*kernelSize, cols + 2*kernelSize };

	unsigned char _r, _g, _b, gray_value;

	for (auto i = 0u; i< rows; ++i) {
		for (auto j = 0u; j< cols; ++j) {
			_r = R[i][j];
			_g = G[i][j];
			_b = B[i][j];
			gray_value = unsigned char((0.299 * _r) + (0.587 * _g) + (0.114 * _b));
			input[i+kernelSize][j+kernelSize] = gray_value;
		}
	}

	input.replicateBorders(kernelSize);
	//if (writePGMB_image("asd.pgm", input.getDataPtr(), input.rows_, input.cols_, 255) == 0)	   exit(1);

	Image<unsigned char> output{ input.rows_, input.cols_ };
	
	tt.Begin();		//start to measure the time
	
	std::string outfname = "sauvolaIntegral.pgm";
	sauvolaIntegral(input, kernelSize, output);
	auto cropped{ output.cropp(kernelSize) };
	if (writePGMB_image(outfname.c_str(), cropped.getDataPtr(), cropped.rows_, cropped.cols_, 255) == 0)	   exit(1);

	outfname = "sauvolaNonIntegral.pgm";
	sauvolaNonIntegral(input, kernelSize, output);
	cropped = output.cropp(kernelSize);
	if (writePGMB_image(outfname.c_str(), cropped.getDataPtr(), cropped.rows_, cropped.cols_, 255) == 0)	   exit(1);

	outfname = "bradleyIntegral.pgm";
	bradleyIntegral(input, kernelSize, output);
	cropped = output.cropp(kernelSize);
	if (writePGMB_image(outfname.c_str(), cropped.getDataPtr(), cropped.rows_, cropped.cols_, 255) == 0)	   exit(1);

	outfname = "bradleyNonIntegral.pgm";
	bradleyNonIntegral(input, kernelSize, output);
	cropped = output.cropp(kernelSize);
	if (writePGMB_image(outfname.c_str(), cropped.getDataPtr(), cropped.rows_, cropped.cols_, 255) == 0)	   exit(1);

	double elapsed = tt.End();	//stop and read elapsed time in ms (miliseconds)
	printf("czas binaryzacji : %f ms", elapsed);

	getchar();

	return 0;
}