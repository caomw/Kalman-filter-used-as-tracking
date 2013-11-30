#include "stdafx.h"
#include <cv.h>
#include <highgui.h>
#include <stdio.h>

int main ()
{
	cvNamedWindow("Kalman",1);
	CvRandState random;//�������
	cvRandInit(&random,0,1,-1,CV_RAND_NORMAL);
	IplImage * image=cvCreateImage(cvSize(600,450),8,3);
	CvKalman * kalman=cvCreateKalman(4,2,0);//״̬����4ά��x��y�������x��y�����ϵ��ٶȣ���������2ά��x��y����

	CvMat * xK=cvCreateMat(4,1,CV_32FC1);//��ʼ��״̬����������Ϊ��40,40����x��y������ٶȷֱ�Ϊ10��10
	xK->data.fl[0]=40.;
	xK->data.fl[1]=40;
	xK->data.fl[2]=10;
	xK->data.fl[3]=10;

	const float F[]={1,0,1,0,0,1,0,1,0,0,1,0,0,0,0,1};//��ʼ�����ݾ��� [1  0  1  0]
													  //               [0  1  0  1]
													  //               [0  0  1  0]
													  //               [0  0  0  1]
	memcpy(kalman->transition_matrix->data.fl,F,sizeof(F));



	CvMat * wK=cvCreateMat(4,1,CV_32FC1);//��������
	cvZero(wK);

	CvMat * zK=cvCreateMat(2,1,CV_32FC1);//��������2ά��x��y����
	cvZero(zK);

	CvMat * vK=cvCreateMat(2,1,CV_32FC1);//��������
	cvZero(vK);

	cvSetIdentity( kalman->measurement_matrix, cvScalarAll(1) );//��ʼ����������H=[1  0  0  0]
																//                [0  1  0  0]
	cvSetIdentity( kalman->process_noise_cov, cvScalarAll(1e-1) );/*��������____�����ʵ���ֵ��
																    ����Ŀ���˶�������ԣ�
																	�������õĺܴ���ϵͳ����������
																	���ٶ�Խ��Խ��*/
	cvSetIdentity( kalman->measurement_noise_cov, cvScalarAll(10) );/*�۲�����____���⽫�۲��������õúܴ�
																	ʹ֮���������Ԥ����ͬ���������*/
	cvSetIdentity( kalman->error_cov_post, cvRealScalar(1) );/*�������Э����*/
	cvRand( &random, kalman->state_post );

	CvMat * mK=cvCreateMat(1,1,CV_32FC1);  //����ʱ��ӵ����������


	while(1){
		cvZero( image );
		cvRectangle(image,cvPoint(30,30),cvPoint(570,420),CV_RGB(255,255,255),2);//����Ŀ�굯��ġ�ײ���ڡ�
		const CvMat * yK=cvKalmanPredict(kalman,0);//����Ԥ��λ��
		cvRandSetRange( &random, 0, sqrt( kalman->measurement_noise_cov->data.fl[0] ), 0 );
		cvRand( &random, vK );//��������Ĳ������
		cvMatMulAdd( kalman->measurement_matrix, xK, vK, zK );//zK=H*xK+vK
		cvCircle(image,cvPoint(cvRound(CV_MAT_ELEM(*xK,float,0,0)),cvRound(CV_MAT_ELEM(*xK,float,1,0))),
			4,CV_RGB(255,255,255),2);//��Ȧ����ʵλ��
		cvCircle(image,cvPoint(cvRound(CV_MAT_ELEM(*yK,float,0,0)),cvRound(CV_MAT_ELEM(*yK,float,1,0))),
			4,CV_RGB(0,255,0),2);//��Ȧ��Ԥ��λ��
		cvCircle(image,cvPoint(cvRound(CV_MAT_ELEM(*zK,float,0,0)),cvRound(CV_MAT_ELEM(*zK,float,1,0))),
			4,CV_RGB(0,0,255),2);//��Ȧ���۲�λ��

		cvRandSetRange(&random,0,sqrt(kalman->process_noise_cov->data.fl[0]),0);
		cvRand(&random,wK);//��������Ĺ������
		cvMatMulAdd(kalman->transition_matrix,xK,wK,xK);//xK=F*xK+wK
	
		if(cvRound(CV_MAT_ELEM(*xK,float,0,0))<30){  //��ײ����������ʱ����Ӧ�᷽��ȡ����������
			cvRandSetRange( &random, 0, sqrt(1e-1), 0 );
			cvRand( &random, mK );
			xK->data.fl[2]=10+CV_MAT_ELEM(*mK,float,0,0);
		}
		if(cvRound(CV_MAT_ELEM(*xK,float,0,0))>570){
			cvRandSetRange( &random, 0, sqrt(1e-2), 0 );
			cvRand( &random, mK );
			xK->data.fl[2]=-(10+CV_MAT_ELEM(*mK,float,0,0));
		}
		if(cvRound(CV_MAT_ELEM(*xK,float,1,0))<30){
			cvRandSetRange( &random, 0, sqrt(1e-1), 0 );
			cvRand( &random, mK );
			xK->data.fl[3]=10+CV_MAT_ELEM(*mK,float,0,0);
		}
		if(cvRound(CV_MAT_ELEM(*xK,float,1,0))>420){
			cvRandSetRange( &random, 0, sqrt(1e-3), 0 );
			cvRand( &random, mK );
			xK->data.fl[3]=-(10+CV_MAT_ELEM(*mK,float,0,0));
		}

		printf("%f_____%f\n",xK->data.fl[2],xK->data.fl[3]);


		cvShowImage("Kalman",image);

		cvKalmanCorrect( kalman, zK );


		if(cvWaitKey(100)=='e'){
			break;
		}
	}

	
    cvReleaseImage(&image);/*�ͷ�ͼ��*/
    cvDestroyAllWindows();
}