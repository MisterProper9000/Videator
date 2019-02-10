//
// ��������� �������� ����� � ������ � ���������� � avi ����
//

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/ml.hpp>

using namespace cv;
using namespace cv::ml;


Mat src; Mat src_gray; Mat drawing;
int thresh = 9;
int max_thresh = 255;
RNG rng(12345);

void thresh_callback(int, void*);

int main(int argc, char* argv[])
{

  cvNamedWindow("capture");

  // �������� ����
  CvCapture* capture = cvCreateFileCapture("Putin.avi");
  assert(capture != 0);

  const char *filename = "vidOUT.avi";

  // ������� ������
  //double fps = cvGetCaptureProperty (capture, CV_CAP_PROP_FPS);
  double fps = 10;

  // ������ ��������
  CvSize size = cvSize((int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH), (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT));
  //CvSize size = cvSize(640, 480);

  CvVideoWriter *writer = cvCreateVideoWriter(filename, CV_FOURCC('X', 'V', 'I', 'D'), fps, size, 1);
  assert(writer != 0);

  IplImage *frame = 0;
  IplImage* canny = 0;

  while (true) {
    using namespace cv;
    using namespace std;
    Mat canny_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    // �������� ����
    frame = cvQueryFrame(capture);
    if (!frame) break;
    cv::Mat src = cv::cvarrToMat(frame);


    cvtColor(src, src_gray, CV_BGR2GRAY);
    blur(src_gray, src_gray, Size(9, 9));

    char* source_window = "Source";
    namedWindow(source_window, CV_WINDOW_AUTOSIZE);
    imshow(source_window, src);

    createTrackbar(" Canny thresh:", "Source", &thresh, max_thresh, thresh_callback);
    thresh_callback(0, 0);
    IplImage* fin = cvCloneImage(&(IplImage)drawing);

	cvWriteFrame(writer, fin);

    // ����������
    cvShowImage("capture", frame);
    cvShowImage("canny", fin);
  


    char c = cvWaitKey(1);
    if (c == 27) { // ���� ������ ESC - �������
      break;
    }
  }
  // ����������� �������
  cvReleaseCapture(&capture);
  cvReleaseVideoWriter(&writer);
  cvDestroyAllWindows();
  return 0;
 

  
}

using namespace std;
// @function thresh_callback 
void thresh_callback(int, void*)
{
  Mat canny_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;

  /// �������� �������
  Canny(src_gray, canny_output, thresh, thresh * 2,3);
  /// ������� �������
  findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

  /// ������ �������
  drawing = Mat::zeros(canny_output.size(), CV_8UC3);
  for (int i = 0; i < contours.size(); i++)
  {
    Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
    drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
  }

  /// ���������� ��������� � ����
  imwrite("out.jpg", canny_output);
  namedWindow("Contours", CV_WINDOW_AUTOSIZE);
  imshow("Contours", drawing);
}