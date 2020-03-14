#include <opencv2/core/core.hpp>
#include <iostream>

#define BLACK 0
#define GREY 128
#define WHITE 255

inline void print_ocv_version()
{
    std::cout << "Template project using OpenCV " << CV_VERSION_MAJOR << "." << CV_VERSION_MINOR << "." << CV_VERSION_REVISION << std::endl;
}

/* the function 'bulid_image_histogram' compute the intensity histogram af a given image.
 * INPUT  -> Image : the input image.
 * OUTPUT -> array : the intensity histogram of the given image.
 */
void build_image_histogra(cv::Mat Image, int *array) {
	/* if the image is continuous */
	if(Image.isContinuous()) {
		unsigned char * pIgData = Image.ptr(0);
		unsigned char * pIgDataEnd = pIgData + Image.cols * Image.rows;

		/* use the intensity of the image as index of the image
		 * histogram and increase the value in that position.
		 */
		while(pIgData < pIgDataEnd) {
			array[*pIgData]++;
			pIgData++;
		}

	} else {/* if the image isn't continuous */
		for(int i =0; i<Image.rows; ++i) {
			unsigned char * pIgData = Image.ptr(i);
			unsigned char * pIgDataEnd = pIgData + Image.cols;

			/* use the intensity of the image as index of the image
			 * histogram and increase the value in that position.
			 */
			while(pIgData < pIgDataEnd) {
				array[*pIgData]++;
				pIgData++;
			}
		}
	}
}

/* the function 'otsu_global_thresholding_method' compute the
 * threshold with otsu global thresholding method.
 * INPUT  -> image_histogram : an image histogram.
 * OUTPUT -> threshold : a threshold computed with otsu global thresholding method.
 */
int otsu_global_thresholding_method(int *image_histogram) {
	int n =0, t =0;
	double variance =0;
	double cumulative_sum =0;
	double cumulative_mean =0;
	double global_intensity_mean =0;
	double between_class_variance =0;
	double normalized_histogram[256];

	for(int i =0; i<256; ++i) {
		normalized_histogram[i] = image_histogram[i];
		n += image_histogram[i];
	}

	/* normalize histogram */
	for(int i =0; i<256; ++i) {
		normalized_histogram[i] /= n;
		global_intensity_mean += i * normalized_histogram[i];
	}

	/* compute the threshold */
	for(int i =0; i<256;++i) {
		cumulative_mean += i * normalized_histogram[i];
		cumulative_sum += normalized_histogram[i];

		/* if the cumulative sum is between 0 and 1 compute the between class variance */
		if(0<cumulative_sum && cumulative_sum <1) {
			variance = pow((global_intensity_mean * cumulative_sum - cumulative_mean),2) / (cumulative_sum * (1 - cumulative_sum));
			
			if (variance>between_class_variance) {
				between_class_variance = variance;
				t = i;
			}
		}
	}

	return t;
}

/* the function 'apply_threshold' apply a threshold to a given image.
 * INPUT  -> ImageInPut : the given image.
 *        -> threshold : the threshold.
 * OUTPUT -> ImageOutPut : image to which the threshold has been applied.
 */
void apply_threshold(cv::Mat ImageInPut, cv::Mat ImageOutPut, int threshold) {
	for(int i =0; i<ImageInPut.rows; ++i) {
		unsigned char * pIgInPut = ImageInPut.ptr(i);
		unsigned char * pIgInPutEnd = pIgInPut + ImageInPut.cols;

		unsigned char * pIgOutPut = ImageOutPut.ptr(i);

		while(pIgInPut<pIgInPutEnd) {
			*pIgOutPut = (threshold < *pIgInPut) * WHITE;
			pIgInPut++;
			pIgOutPut++;
		}
	}
}

/* the function 'mix_image' marge together ImageWhite and ImageBlack with respect the ImageBlackWhite.
 * INPUT  -> ImageWhite : the image that will be applied with respect to white.
 *        -> ImageBlack : the image that will be applied with respect to black.
 		  -> ImageBlackWhite : threshold image.
 * OUTPUT -> ImageMix : the merged image.
 */
void mix_image(cv::Mat ImageWhite, cv::Mat ImageBlack, cv::Mat ImageBlackWhite, cv::Mat ImageMix) {
	for(int i =0; i<ImageBlack.rows; ++i) {
		unsigned char * pIgWh = ImageWhite.ptr(i);

		unsigned char * pIgBl = ImageBlack.ptr(i);
		unsigned char * pIgBlEnd = pIgBl + ImageBlack.cols;

		unsigned char * pIgMi = ImageMix.ptr(i);

		unsigned char * pIgBlWh = ImageBlackWhite.ptr(i);

		while(pIgBl < pIgBlEnd) {
			/* if the value in '*pIgBlWh' is 0 (black) put in
			 * '*pIgMi' the value in '*pIgBl' otherwise '*pIgWh'
			 */
			*pIgMi = ((*pIgBlWh)==BLACK) ? *pIgBl : *pIgWh;
			pIgWh++;
			pIgBl++;
			pIgMi++;
			pIgBlWh++;
		}
	}
}