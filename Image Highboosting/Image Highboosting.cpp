
/* -----------------------------------------------------------------------------------------------
IMAGE HIGHBOOSTING IN FREQUENCY DOMAIN USING DISCRETE FOURIER TRANSFORMATION AND IDEAL FILTER.
IMAGE HIGHBOOSTING IN FREQUENCY DOMAIN USING DISCRETE FOURIER TRANSFORMATION AND GAUSSIAN FILTER.
IMAGE HIGHBOOSTING IN FREQUENCY DOMAIN USING DISCRETE FOURIER TRANSFORMATION AND BUTTERWORTH FILTER.
IMAGE HIGHBOOSTING IN FREQUENCY DOMAIN USING DISCRETE FOURIER TRANSFORMATION AND LoG FILTER.
Frequency Domain Highboost Filter.cpp : Defines the entry point for the console application.
.................................................................................................*/

/*--------------------------------------HEADER FILES --------------------------------------------*/
// Include Necessary Header Files 
// [both in Std and Non Std Path]
#include "stdafx.h"
#include<stdio.h>
#include<math.h>
#include<time.h>
#include<opencv\cv.h>
#include<opencv\highgui.h>
using namespace std;
using namespace cv;

/*----------------------------------------- END -------------------------------------------------*/

/*------------------------------ IDEAL HIGHBOOST FILTER -----------------------------------------*/
// Highboost Ideal Filter Construction Function Defination
// Input Parameter: 2D FILTER , CUTOFF FREQUENCY
void HB_Ideal_Filter(Mat &HBFilter, int CUTOFF)
{
	// Temporary Mat Variable to Store Ideal Coefficients
	// Calculate Mid Point of Mat Variable [HBFilter]
	Mat Filter = Mat(HBFilter.rows, HBFilter.cols, CV_32F);
	Point Mid = Point(HBFilter.rows / 2, HBFilter.cols / 2);
	double D;

	// Compute Point Wise Highpass Ideal Coefficients and Store in Filter Variable
	for (int i = 0; i < HBFilter.rows; i++)
	{
		for (int j = 0; j < HBFilter.cols; j++)
		{
			D = pow(Mid.x - abs(i - Mid.x), 2) + pow(Mid.y - abs(j - Mid.y), 2);
			Filter.at<float>(i, j) = 1.0 + ((sqrt(D) > CUTOFF) ? 1.0 : 0.0);
		}
	}

	// Construct A Filter of DFTTrans_Image[0] Size from Filter Variable
	Mat tmp[] = { Filter, Filter };
	merge(tmp, 2, HBFilter);
}
/*----------------------------------------- END --------------------------------------------------*/

/*------------------------------ GAUSSIAN HIGHBOOST FILTER ---------------------------------------*/
// Highboost Gaussian Filter Construction Function Defination
// Input Parameter: 2D FILTER , CUTOFF FREQUENCY
void HB_Gaussian_Filter(Mat &HBFilter, int CUTOFF)
{
	// Temporary Mat Variable to Store Gaussian Coefficients
	// Calculate Mid Point of Mat Variable [HBFilter]
	Mat Filter = Mat(HBFilter.rows, HBFilter.cols, CV_32F);
	Point Mid = Point(HBFilter.rows / 2, HBFilter.cols / 2);
	double D;

	// Compute Point Wise Highboost Gaussian Coefficients and Store in Filter Variable
	for (int i = 0; i < HBFilter.rows; i++)
	{
		for (int j = 0; j < HBFilter.cols; j++)
		{
			D = pow(Mid.x - abs(i - Mid.x), 2) + pow(Mid.y - abs(j - Mid.y), 2);
			Filter.at<float>(i, j) = 2 - (float)exp(-1 * D / (2 * pow(CUTOFF, 2)));
		}
	}

	// Construct A Filter of DFTTrans_Image[0] Size from Filter Variable
	Mat tmp[] = { Filter, Filter };
	merge(tmp, 2, HBFilter);

}
/*--------------------------------------- END ---------------------------------------------------*/

/*------------------------------ BUTTERWORTH HIGHBOOST FILTER -------------------------------------*/
// Highboost Butterworth Filter Construction Function Defination
// Input Parameter: 2D FILTER , CUTOFF FREQUENCY, ORDER of FILTER
void HB_Butterworth_Filter(Mat &HBFilter, int CUTOFF, int ord)
{
	// Temporary Mat Variable to Store Butterworth Coefficients
	// Calculate Mid Point of Mat Variable [HBFilter]
	Mat Filter = Mat(HBFilter.rows, HBFilter.cols, CV_32F);
	Point Mid = Point(HBFilter.rows / 2, HBFilter.cols / 2);
	double D;

	// Compute Point Wise Highboost Butterworth Coefficients and Store in Filter Variable
	for (int i = 0; i < HBFilter.rows; i++)
	{
		for (int j = 0; j < HBFilter.cols; j++)
		{
			D = pow(Mid.x - abs(i - Mid.x), 2) + pow(Mid.y - abs(j - Mid.y), 2);
			Filter.at<float>(i, j) = 1 + 1 / (1 + pow(CUTOFF / sqrt(D), 2 * ord));
		}
	}

	// Construct A Filter of DFTTrans_Image[0] Size from Filter Variable
	Mat tmp[] = { Filter, Filter };
	merge(tmp, 2, HBFilter);

}
/*--------------------------------------- END ---------------------------------------------------*/

/*------------------------------ LoG HIGHBOOST FILTER ---------------------------------------*/
// Highboost LoG Filter Construction Function Defination
// Input Parameter: 2D FILTER , CUTOFF FREQUENCY
void HB_LoG_Filter(Mat &HBFilter, int CUTOFF)
{
	// Temporary Mat Variable to Store LoG Coefficients
	// Calculate Mid Point of Mat Variable [HBFilter]
	Mat Filter = Mat(HBFilter.rows, HBFilter.cols, CV_32F);
	Point Mid = Point(HBFilter.rows / 2, HBFilter.cols / 2);
	int Freq = pow(CUTOFF, 2);
	double D;

	// Compute Point Wise Highboost LoG Coefficients and Store in Filter Variable
	for (int i = 0; i < HBFilter.rows; i++)
	{
		for (int j = 0; j < HBFilter.cols; j++)
		{
			D = pow(Mid.x - abs(i - Mid.x), 2) + pow(Mid.y - abs(j - Mid.y), 2);
			Filter.at<float>(i, j) = 2 - (float) (1 - D/Freq)*exp(-1 * D / (2 * Freq));
		}
	}

	// Construct A Filter of DFTTrans_Image[0] Size from Filter Variable
	Mat tmp[] = { Filter, Filter };
	merge(tmp, 2, HBFilter);

}
/*--------------------------------------- END ---------------------------------------------------*/

/*------------------------------------- MAIN PROGRAM --------------------------------------------*/
int main(void)
{
	// Read Image Data from Image File
	Mat Image = imread("Pois.bmp", CV_LOAD_IMAGE_COLOR);

	// Check, Whether Program Open Image File
	if (!Image.data)
	{
		printf("COULDN'T OPEN OR READ Noise_Gaus.bmp FILE");
		return -1;
	}

	// Display The Input Image
	namedWindow("BLURRED IMAGE", WINDOW_NORMAL);
	imshow("BLURRED IMAGE", Image);

	// 'Select', 'ord' Variables for selection of Filter and Its Order Incase Butterworth Filter 
	// Filter 'HBFilter' Variable to Store 2D Filter's Coefficients.
	// CUTOFF Frequency to Retain Amount of High Frequency Information in An Image
	Mat HBFilter; int Select, ord, CUTOFF;

	// Initialize Clock Variable to compute Time Taken in millisecs
	clock_t start, end;
	float Time_Used;

	// Create Variables to Store Actual and Its DFT Transfomed Values of [R, G, B] Cheannels.
	// Copy Image.data values to RGB_Image for Extracting All Three Frames [R, G, B] 
	Mat RGB_Image[3], DFTTrans_Image[3];
	split(Image, RGB_Image);

	// Convert Each Channel Pixel Values from 8U to 32F
	// Discreate Fourier Transformation of Each Channel
	for (int i = 0; i < 3; i++)
	{
		RGB_Image[i].convertTo(RGB_Image[i], CV_32F);
		dft(RGB_Image[i], DFTTrans_Image[i], DFT_COMPLEX_OUTPUT);
	}

	// Make Filter Variable Same Size of DFTTrans_Image & Input The CUTOFF Frequency
	HBFilter = DFTTrans_Image[0].clone();
	printf("Enter The Cutoff Frequency and Order of The Filter: \n");
	scanf("%d %d", &CUTOFF, &ord);		
	printf("Selection of Filter [1]Ideal [2]Gaussian [3]ButterWorth [4]LoG:\n");
	scanf("%d", &Select);

	// Start The Timer
	start = clock();

	// Selection of Low Pass Filter
	switch (Select)
	{
	case 1: printf("You Selected Ideal Filter \n");
			HB_Ideal_Filter(HBFilter, CUTOFF);
			break;
	case 2: printf("You Selected Gaussian Filter \n");
			HB_Gaussian_Filter(HBFilter, CUTOFF);
			break;
	case 3:	printf("You Selected Butterworth \n");
			HB_Butterworth_Filter(HBFilter, CUTOFF, ord);
			break;
	case 4: printf("You Selected LoG \n");
			HB_LoG_Filter(HBFilter, CUTOFF);
			break;
	default: printf("Wrong Selection \n");
			break;
	}

	// Improve Information Using Highboost Filter By Applying It on Channels Separately
	// Inverse Discrete Fourier Transformation of Each Channel
	// Convert Each Chanel Pixel Values from 32F to 8U
	for (int i = 0; i < 3; i++)
	{
		Mat chan[2];
		mulSpectrums(DFTTrans_Image[i], HBFilter, DFTTrans_Image[i], 0);
		idft(DFTTrans_Image[i], DFTTrans_Image[i], DFT_SCALE);
		split(DFTTrans_Image[i], chan);
		chan[0].convertTo(RGB_Image[i], CV_8U);
	}

	//Stop the Timer
	end = clock();

	// Calculate Time Taken for Image Highboosting by a Highboost Filter in millisecs.
	Time_Used = (float)(end - start) / CLOCKS_PER_SEC;
	printf("Time Taken for Image smoothing : %.3f", Time_Used);

	// Merge All Three Channels to Construct Final Image
	merge(RGB_Image, 3, Image);

	// Display Final Highboosted Image By Highboost FIlter
	namedWindow("HIGHBOOSTED IMAGE", WINDOW_NORMAL);
	imshow("HIGHBOOSTED IMAGE", Image);

	// Write 2D Array to Image File (.bmp)
	imwrite("Impulse_LoG_50.bmp", Image);

	waitKey(0);
	return 0;
}
/*----------------------------------- END ------------------------------------------------------*/
 