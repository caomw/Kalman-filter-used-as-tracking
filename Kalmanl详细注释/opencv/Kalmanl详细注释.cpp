/*
cvRandInit()
��ʼ��CvRandState���ݽṹ,����ѡ������ֲ�������,������������,����������
cvRandInit(CvRandState���ݽṹ,����Ͻ�,����½�,���ȷֲ�����,64bits���ӵ�����)---- CV_RAND_UNI ָ��Ϊ���ȷֲ�����
cvRandInit(CvRandState���ݽṹ,ƽ����,��׼ƫ��,��̬�ֲ�����,64bits���ӵ�����)---- CV_RAND_NORMAL


cvRandSetRange()
�޸�CvRandState���ݽṹ�Ĳ�������,���ȷֲ��Ļ�����ÿ���ŵ������½糣̬�ֲ������޸�ÿ��ͨ����ƽ����,��׼ƫ��.
cvRandSetRange(CvRandState���ݽṹ,���ȷֲ��Ͻ�,���ȷֲ��½�,Ŀ���ŵ�����)
cvRandSetRange(CvRandState���ݽṹ,��̬�ֲ�ƽ����,��̬�ֲ���׼ƫ��,Ŀ���ŵ�����)

cvRand()
��CvMat��IplImage���ݽṹ�����,�ñ��趨����CvRandState���ݽṹ�����.
cvRand(CvRandState���ݽṹ,CvMat��IplImage���ݽṹ)

cvbRand()
��һά���������,�����趨����ĳ���
cvbRand(RandState���ݽṹ,Float�ͱ�������,����ĳ���);
*/


//cvKalman�ṹ��ϸ˵��
/*
typedef struct CvKalman
{
    int MP;                     /* ��������ά�� */
    int DP;                     /* ״̬����ά�� */
    int CP;                     /* ��������ά�� */

    /* �������ֶ� */
		#if 1
			float* PosterState;         /* =state_pre->data.fl */
			float* PriorState;          /* =state_post->data.fl */
			float* DynamMatr;           /* =transition_matrix->data.fl */
			float* MeasurementMatr;     /* =measurement_matrix->data.fl */
			float* MNCovariance;        /* =measurement_noise_cov->data.fl */
			float* PNCovariance;        /* =process_noise_cov->data.fl */
			float* KalmGainMatr;        /* =gain->data.fl */
			float* PriorErrorCovariance;/* =error_cov_pre->data.fl */
			float* PosterErrorCovariance;/* =error_cov_post->data.fl */
			float* Temp1;               /* temp1->data.fl */
			float* Temp2;               /* temp2->data.fl */
		#endif

    CvMat* state_pre;           /* Ԥ��״̬ (x'(k)): 
                                    x(k)=A*x(k-1)+B*u(k) */
    CvMat* state_post;          /* ����״̬ (x(k)):
                                    x(k)=x'(k)+K(k)*(z(k)-H*x'(k)) */
    CvMat* transition_matrix;   /* ״̬���ݾ��� state transition matrix (A) */
    CvMat* control_matrix;      /* ���ƾ��� control matrix (B)
                                   (���û�п��ƣ���ʹ����)*/
    CvMat* measurement_matrix;  /* �������� measurement matrix (H) */
    CvMat* process_noise_cov;   /* ��������Э�������
                                        process noise covariance matrix (Q) */
    CvMat* measurement_noise_cov; /* ��������Э�������
                                          measurement noise covariance matrix (R) */
    CvMat* error_cov_pre;       /* ��������Э�������
                                        priori error estimate covariance matrix (P'(k)):
                                     P'(k)=A*P(k-1)*At + Q)*/
    CvMat* gain;                /* Kalman ������� gain matrix (K(k)):
                                    K(k)=P'(k)*Ht*inv(H*P'(k)*Ht+R)*/
    CvMat* error_cov_post;      /* ����������Э�������
                                        posteriori error estimate covariance matrix (P(k)):
                                     P(k)=(I-K(k)*H)*P'(k) */
    CvMat* temp1;               /* ��ʱ���� temporary matrices */
    CvMat* temp2;
    CvMat* temp3;
    CvMat* temp4;
    CvMat* temp5;
}CvKalman;

*/



#include "StdAfx.h"
#include "cv.h"
#include "highgui.h"
#include <math.h>



int main(int argc, char** argv)
{
 
	cvNamedWindow( "Kalman", 1 );//�������ڣ���Ϊ��ʱ�򣬱�ʾ���ڴ�С�Զ��趨
	CvRandState rng;
	cvRandInit( &rng, 0, 1, -1, CV_RAND_UNI );/* CV_RAND_UNI ָ��Ϊ���ȷֲ����͡����������Ϊ-1----------�����ָ�Ϊ��˹�ֲ� */

	IplImage* img = cvCreateImage( cvSize(500,500), 8, 3 );
	CvKalman* kalman = cvCreateKalman( 2, 1, 0 );/*״̬����Ϊά���۲�����Ϊά���޼�������ά*/

	// State is phi, delta_phi - angle and angular velocity
	// Initialize with random guess
	CvMat* x_k = cvCreateMat( 2, 1, CV_32FC1 );/*�������С�Ԫ������ΪCV_32FC1��Ԫ��Ϊλ��ͨ���������;���*/
	cvRandSetRange( &rng, 0, 0.1, 0 );/*�����������Χ�������������̬�ֲ�����ֵΪ��������Ϊ.1��ͨ������Ϊ*/
	rng.disttype = CV_RAND_NORMAL;
	cvRand( &rng, x_k ); /*����������*/

	// Process noise
	CvMat* w_k = cvCreateMat( 2, 1, CV_32FC1 );

	// Measurements, only one parameter for angle
	CvMat* z_k = cvCreateMat( 1, 1, CV_32FC1 );/*����۲����*/
	cvZero( z_k ); /*��������*/

	// Transition matrix F describes model parameters at and k and k+1
	const float F[] = { 1, 1, 0, 1 }; /*״̬ת�ƾ���*/
	memcpy( kalman->transition_matrix->data.fl, F, sizeof(F));
	/*��ʼ��ת�ƾ������У������CvKalman* kalman = cvCreateKalman( 2, 1, 0 );*/

	// Initialize other Kalman parameters
	cvSetIdentity( kalman->measurement_matrix, cvRealScalar(1) );/*�۲����*/
	cvSetIdentity( kalman->process_noise_cov, cvRealScalar(1e-5) );/*��������*/
	cvSetIdentity( kalman->measurement_noise_cov, cvRealScalar(1e-1) );/*�۲�����*/
	cvSetIdentity( kalman->error_cov_post, cvRealScalar(1) );/*�������Э����*/

	// Choose random initial state
	cvRand( &rng, kalman->state_post );/*��ʼ��״̬����*/

	// Make colors
	CvScalar yellow = CV_RGB(255,255,0);/*����Ϊ��������ɫ*/
	CvScalar white = CV_RGB(255,255,255);
	CvScalar red = CV_RGB(255,0,0);

	while( 1 )
	{
		// Predict point position
		const CvMat* y_k = cvKalmanPredict( kalman, 0 );/*����������Ϊ*/

		// Generate Measurement (z_k)
		cvRandSetRange( &rng, 0, sqrt( kalman->measurement_noise_cov->data.fl[0] ), 0 );/*���ù۲�����*/	
		cvRand( &rng, z_k );
		cvMatMulAdd( kalman->measurement_matrix, x_k, z_k, z_k );

		// Update Kalman filter state
		cvKalmanCorrect( kalman, z_k );

		// Apply the transition matrix F and apply "process noise" w_k
		cvRandSetRange( &rng, 0, sqrt( kalman->process_noise_cov->data.fl[0] ), 0 );/*������̬�ֲ���������*/
		cvRand( &rng, w_k );
		cvMatMulAdd( kalman->transition_matrix, x_k, w_k, x_k );

		// Plot Points
		cvZero( img );/*����ͼ��*/
		// Yellow is observed state ��ɫ�ǹ۲�ֵ
		//cvCircle(IntPtr, Point, Int32, MCvScalar, Int32, LINE_TYPE, Int32)
		//��Ӧ���������У�shiftΪ���ݾ���
		//cvCircle(img, center, radius, color, thickness, lineType, shift)
		//���ƻ����һ������Բ�ĺͰ뾶��Բ
		cvCircle( img, 
			cvPoint( cvRound(img->width/2 + img->width/3*cos(z_k->data.fl[0])),
			cvRound( img->height/2 - img->width/3*sin(z_k->data.fl[0])) ), 
			4, yellow );
		// White is the predicted state via the filter
		cvCircle( img, 
			cvPoint( cvRound(img->width/2 + img->width/3*cos(y_k->data.fl[0])),
			cvRound( img->height/2 - img->width/3*sin(y_k->data.fl[0])) ), 
			4, white, 2 );
		// Red is the real state
		cvCircle( img, 
			cvPoint( cvRound(img->width/2 + img->width/3*cos(x_k->data.fl[0])),
			cvRound( img->height/2 - img->width/3*sin(x_k->data.fl[0])) ),
			4, red );
		CvFont font;
		cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX,0.5f,0.5f,0,1,8);
		cvPutText(img,"Yellow:observe",cvPoint(0,20),&font,cvScalar(0,0,255));
		cvPutText(img,"While:predict",cvPoint(0,40),&font,cvScalar(0,0,255));
		cvPutText(img,"Red:real",cvPoint(0,60),&font,cvScalar(0,0,255));
		cvPutText(img,"Press Esc to Exit...",cvPoint(0,80),&font,cvScalar(255,255,255));
		cvShowImage( "Kalman", img );		

		// Exit on esc key
		if(cvWaitKey(100) == 27) 
			break;
	}
	cvReleaseImage(&img);/*�ͷ�ͼ��*/
	cvReleaseKalman(&kalman);/*�ͷ�kalman�˲�����*/
	cvDestroyAllWindows();/*�ͷ����д���*/
}
