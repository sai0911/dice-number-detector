
#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;
using namespace std;


class dices
{
public:
    Mat loadimage(string filename)
    {
        // LOAD image
        Mat image_original;
        image_original = imread(filename);   // Read the file "image.jpg".


        if(! image_original.data )  // Check for invalid input
        {
               cout <<  "Could not open or find the image" << std::endl ;
               return image_original;
        }
        return image_original;
    }

    void colorconvert(Mat *image, Mat *image_original){
        cvtColor(*image_original,*image,CV_BGR2GRAY);
    }

    void imagethreshold(Mat *image){
        // Binary Threshold

           double thresh = 200;
           double maxValue = 255;

           GaussianBlur(*image,*image,Size(5,5),1.5,1.5);
           threshold(*image,*image, thresh, maxValue,  ADAPTIVE_THRESH_MEAN_C);
    }





    void imagemorph(Mat *image){
        //           morphological operations


                   int dilSize = 5;
                   Mat kern = cv::getStructuringElement( CV_SHAPE_ELLIPSE, cv::Size( dilSize , dilSize  ) );

                   morphologyEx(*image,*image,MORPH_CLOSE,kern);
    }




    void imageedge(Mat *image, Mat *detected_edges){
                   int lowThreshold=30;
                   double ratio=1.6;
                   Mat dst;



                   // Reduce noise with a kernel 3x3
                    blur( *image, dst, Size(3,3) );

                    // Canny detector
                    Canny( dst,*detected_edges, lowThreshold, lowThreshold*ratio);
    }

    vector<vector<Point>> image_findcountours(Mat *detected_edges){
        vector<vector<Point>> contours;
                   findContours(*detected_edges,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
                   return contours;
    }

    void image_contourthreshold(vector<vector<Point>> *contours){
                   int cmin=100;
                   int cmax=1000;
                   vector<vector<Point>>::const_iterator itc=contours->begin();

                   while(itc!=contours->end())
                   {
                       if((int)(itc->size())<cmin || (int)(itc->size())>cmax)
                           itc=contours->erase(itc);
                       else
                           ++itc;
                   }
    }

    void draw_contour(Mat *image_original, vector<vector<Point>> *contours){
                  Mat contourImage(image_original->size(), CV_8UC3, Scalar(0,0,0));
                       Scalar colors[3];
                       colors[0] = Scalar(255, 0, 0);
                       colors[1] = Scalar(0, 255, 0);
                       colors[2] = Scalar(0, 0, 255);
                       for (size_t idx = 0; idx < contours->size(); idx++)
                       {
                           drawContours(*image_original, *contours, idx,colors[idx % 5],4);
                       }

    }

    void detect_circle_contour(vector<vector<Point>> contours, Mat image_original, Mat *actual_image){
                       vector<Mat> subregions;
                           // contours_final is as given above in your code
                           for (int i = 0; i < (int)(contours.size()); i++)
                           {
                               // Get bounding box for contour
                               Rect roi = boundingRect(contours[i]);

                               // Create a mask for each contour to mask out that region from image.
                               Mat mask = Mat::zeros(image_original.size(), CV_8UC1);
                               drawContours(mask, contours, i, Scalar(255), CV_FILLED);

                               // At this point, mask has value of 255 for pixels within the contour and value of 0 for those not in contour.

                               // Extract region using mask for region
                               Mat contourRegion;
                               Mat imageROI;
                               image_original.copyTo(imageROI, mask); // 'image' is the image you used to compute the contours.
                               contourRegion = imageROI(roi);

                               subregions.push_back(contourRegion);

                               vector<Vec3f> circles;
                               int count=0;

                                              HoughCircles(imageROI,circles, CV_HOUGH_GRADIENT,2,5,50,45,5,19);


                                              vector<Vec3f>::const_iterator itc1=circles.begin();

                                              while(itc1!=circles.end())
                                              {
                                                  circle(*actual_image,Point((*itc1)[0],(*itc1)[1]),(*itc1)[2],Scalar(255),2);

                                                  ++itc1;
                                                  ++count;
                                              }
                                              cout<<"Die number "<<(i+1)<<" has value: "<<count<<endl;
                           }
                           if(contours.size()==0)
                               cout<<"No dices are present"<<endl;
                           imshow( "window", *actual_image ); // Show our image inside it.
                           cvMoveWindow("window", 200, 0);
                           waitKey(0);
    }
};


int main( )
{
    Mat image,dst,image_original,detected_edges;
    vector<vector<Point>> contours;
    dices d;
           string filename = "/home/sai/Desktop/dice/1 - Count Dice Dots/dice6.png";
           image_original = d.loadimage(filename);
           if(image_original.empty())
               return -1;
           d.colorconvert(&image, &image_original);
           d.imagethreshold(&image);
           d.imagemorph(&image);
           d.imageedge(&image, &detected_edges);
           contours = d.image_findcountours(&detected_edges);
           d.image_contourthreshold(&contours);

           d.draw_contour(&image_original,&contours);
           d.detect_circle_contour(contours, detected_edges, &image_original);
}
