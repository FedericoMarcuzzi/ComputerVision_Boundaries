#include <iostream>
#include <fstream>
#include <sstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <math.h>

#include "include/misc.h"

/* the classs 'Blob' contains all the information about the blob: its size and the points that make it up 
 * the classs 'Blob' have the following function
 * add_point(int,int) -> void : the as input the abscissa 'x' and the ordinate 'y' and increase the size of the blob.
 * get_size() -> int : return the size of the blob, it's perimeter.
 * get_points() -> std::vector<cv::Point2i> : return the list of all the point of the blob.
 */
class Blob {
	public:
		/* adds a coordinate */
		void add_point(int x, int y) {
			points.push_back(cv::Point2i(x,y));
			size++;
		}

		/* get the size of the blob (perimeter) */
		int get_size() {
			return size;
		}

		/* returns the list of blob points */
		std::vector<cv::Point2i> get_points() {
			return points;
		}

		Blob() {
			size = 0;
		}

	private:
		std::vector<cv::Point2i> points;
		int size;
};

/* the function 'add_span' adds a border of 'span_size' pixel to each side of the input image
 * INPUT  -> IgInPut : the original image.
 *        -> span_size : the size of the border.
 * OUTPUT -> IgSpn : the image with the border
 */
cv::Mat add_span(cv::Mat IgInPut, int span_size) {

	/* create the output image with the correct shape */
	cv::Mat IgSpn = cv::Mat(IgInPut.rows+span_size*2,IgInPut.cols+span_size*2,CV_8U);

	IgSpn = 0;

	for(int i =0; i<IgInPut.rows; ++i) {
		unsigned char * pIgInPut = IgInPut.ptr(i);
		unsigned char * pIgInPutEnd = pIgInPut + IgInPut.cols;

		unsigned char * pIgSpn = IgSpn.ptr(i+span_size) + span_size;

		/* copy the original image in the center of the new one */
		while(pIgInPut<pIgInPutEnd) {
			*pIgSpn = *pIgInPut;
			pIgInPut++;
			pIgSpn++;
		}
	}

	return IgSpn;
}

/* the function 'find_blob' searches for all the points that make up the blob starting from x and y.
 * INPUT  -> IgInPut : the original image.
 *        -> x : abscissa of the starting point of the blob.
 *		  -> y : ordinate of the starting point of the blob.
 * OUTPUT -> Blob : an object containing all the blob information. See also class comment.
 */
Blob find_blob(cv::Mat IgInPut, int x, int y) {
	int b0x = x, b0y = y;
	int bx = x, by = y;
	int clock_pos = 0;

	/* clock phase, all possible points around the center */
	int clock_bx[8] = {-1,-1,-1,0,1,1,1,0};
	int clock_by[8] = {-1,0,1,1,1,0,-1,-1};
	/* start clock positions */
	int array[8] = {6,0,0,2,2,4,4,6};
	Blob blob;

	IgInPut.at<unsigned char>(b0x, b0y) = GREY;
	blob.add_point(b0x-1,b0y-1);

	do {
		/* try all eight clock phase */
		for(int i=0;i<8;i++) {
			/* if it isn't black then it is a part of a blob */
			if(IgInPut.at<unsigned char>(bx+clock_bx[clock_pos],by+clock_by[clock_pos])!=0) {
				/* update position */
				bx = bx + clock_bx[clock_pos];
				by = by + clock_by[clock_pos];

				if(IgInPut.at<unsigned char>(bx,by)==WHITE)
					/* add coordinates to the blob */
					blob.add_point(bx-1,by-1);
				
				/* mark the position with the color GREY */
				IgInPut.at<unsigned char>(bx, by) = GREY;
				
				/* update the starting point of the next research */
				clock_pos = array[clock_pos];
				break;
				
			} else
				/* try next clock phase */
				clock_pos = (clock_pos + 1) % 8;
		}
	/* loop until it reach the starting point */
	} while(b0x != bx || b0y != by);

	return blob;
}

/* the function 'find_contours' takes an image as input and finds all the contours.
 * INPUT  -> IgInPut : the original image.
 * INPUT  -> lower_bound : the lower bound of the blobs perimeter.
 * INPUT  -> upper_bound : the upper bound of the blobs perimeter.
 * OUTPUT -> IgOutPut : image with highlighted contours.
 */
void find_contours(cv::Mat IgInPut,cv::Mat IgOutPut,int lower_bound, int upper_bound) {

	Blob b;
	std::vector<Blob> blobs;
	cv::Vec3b color(0,0,255);

	int threshold;
	bool flag = true;

	/* create the array for the image histogram and initialize it at 0 */
	int image_histogram[256] ={};

	cv::Mat IgGry = IgInPut.clone();
	cv::cvtColor(IgInPut, IgGry, cv::COLOR_RGB2GRAY);

	/* compute the image histogram of 'IgGry' */
	build_image_histogra(IgGry,image_histogram);

	/* compute the otsu of 'IgGry' */
	threshold = otsu_global_thresholding_method(image_histogram);

	/* apply the threshold to 'IgGry' */
	apply_threshold(IgGry,IgGry,threshold);
	
	/* add the span to 'IgGry' */
	cv::Mat IgSpn = add_span(IgGry,1);

	/* start to search the blob */
	for(int i=1;i<IgSpn.rows-1;i++) {
		for(int j=1;j<IgSpn.cols-1;j++) {
			/* if it's white it is a starting point of a blob */
			if(flag && IgSpn.at<unsigned char>(i,j)==WHITE) {
				/* find the blob from the starting point 'i' and 'j' */
				b = find_blob(IgSpn,i,j);
				/* keeps only the blobs with a perimeter between 100 and 200 pixels */
				if(b.get_size() >= lower_bound && b.get_size()<=upper_bound)
					blobs.push_back(b);
			}
			/* if it is not white or black then it is a pixel of a blob already
			 * found so I do not have to look for it anymore, 'flag = false'.
			 */
			if(IgSpn.at<unsigned char>(i,j)!=BLACK && IgSpn.at<unsigned char>(i,j)!=WHITE)
				flag = false;

			/* if it finds black it is out of a blob, then it can start the search for blobs again. */
			if(IgSpn.at<unsigned char>(i,j)==BLACK)
				flag = true;
		}
	}

	/* for each blob it adds a red border around it in the image. */
	for(std::vector<Blob>::iterator blob = blobs.begin(); blob!=blobs.end();blob++) {
		/* print the size of the blob */
		std::vector<cv::Point2i> points = (*blob).get_points();
		for(std::vector<cv::Point2i>::iterator point = points.begin(); point!=points.end();point++)
			IgOutPut.at<cv::Vec3b>((*point).x,(*point).y) = color;
	}
}

int main(int argc, char* argv[])
{
	if(argc < 5)
		throw std::invalid_argument("./boundaries <input_filename> <output_filename> <lower_bound_perimeter_size> <upper_bound_perimeter_size>\n\tinput_filename: name of the imput image\n\toutput_filename: name of the output image\n\tmin_size: minimum size of the perimeter of the blob\n\tmax_size: maximum size of the perimeter of the blob");

	/* load image bacteria.png */
	cv::Mat Ibct = cv::imread(argv[1]);
	cv::Mat Ibnd = Ibct.clone();

	/* find all the contours */
	find_contours(Ibct,Ibnd,atoi(argv[3]),atoi(argv[4]));

	cv::imshow(argv[1],Ibct);
	cv::waitKey();

	cv::imshow(argv[2],Ibnd);
	cv::waitKey();

	/* save image boundaries.png */
	cv::imwrite(argv[2],Ibnd);
	//26,2434
	return 0;
}