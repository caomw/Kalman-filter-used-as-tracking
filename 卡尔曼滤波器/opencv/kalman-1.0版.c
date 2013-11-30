/* 
   Tracking of rotating point.
   Rotation speed is constant.
   Both state and measurements vectors are 1D (a point angle),
   Measurement is the real point angle + gaussian noise.
   The real and the estimated points are connected with yellow line segment,
   the real and the measured points are connected with red line segment.
   (if Kalman filter works correctly,
    the yellow segment should be shorter than the red one).
   Pressing any key (except ESC) will reset the tracking with a different speed.
   Pressing ESC will stop the program.
*/

#ifdef _CH_
#pragma package <opencv>
#endif

#ifndef _EiC
#include "cv.h"
#include "highgui.h"
#include <math.h>
#endif

int main(int argc, char** argv)
{
    const float A[] = { 1, 1, 0, 1 };//״̬ת�ƾ���
    
    IplImage* img = cvCreateImage( cvSize(500,500), 8, 3 );//������ʾ���õ�ͼ��
    CvKalman* kalman = cvCreateKalman( 2, 1, 0 );//����cvKalman���ݽṹ��״̬����Ϊ2ά���۲�����Ϊ1ά���޼�������ά
    CvMat* state = cvCreateMat( 2, 1, CV_32FC1 ); //(phi, delta_phi) ������״̬����
    CvMat* process_noise = cvCreateMat( 2, 1, CV_32FC1 );// ��������һ��CV_32FC1�ĵ�ͨ�������;���
    CvMat* measurement = cvCreateMat( 1, 1, CV_32FC1 ); //����۲����
    CvRNG rng = cvRNG(-1);//��ʼ��һ��������к���
    char code = -1;

    cvZero( measurement );//�۲������������
    cvNamedWindow( "Kalman", 1 );

    for(;;)
    {		//�þ��ȷֲ�������̬�ֲ������������������state
        cvRandArr( &rng, state, CV_RAND_NORMAL, cvRealScalar(0), cvRealScalar(0.1) );//״̬state
        memcpy( kalman->transition_matrix->data.fl, A, sizeof(A));//��ʼ��״̬ת��F����
        
        //cvSetIdentity()�÷����������г�������������������������Ԫ�ص�ֵ������Ϊ0��������������ȵ�Ԫ�ص�ֵ������Ϊ1
        //���ǽ�(��һ��ǰ����׶ε�)����״̬��ʼ��Ϊһ�����ֵ
        cvSetIdentity( kalman->measurement_matrix, cvRealScalar(1) );//�۲����H
        cvSetIdentity( kalman->process_noise_cov, cvRealScalar(1e-5) );//��������Q
        cvSetIdentity( kalman->measurement_noise_cov, cvRealScalar(1e-1) );//�۲�����R 
        cvSetIdentity( kalman->error_cov_post, cvRealScalar(1));//�������Э����
        cvRandArr( &rng, kalman->state_post, CV_RAND_NORMAL, cvRealScalar(0), cvRealScalar(0.1) );//У��״̬
        
        //��ʱ����̬ϵͳ�Ͽ�ʼԤ��
        
        for(;;)
        {
            #define calc_point(angle)                                      \
                cvPoint( cvRound(img->width/2 + img->width/3*cos(angle)),  \
                         cvRound(img->height/2 - img->width/3*sin(angle))) 

            float state_angle = state->data.fl[0];
            CvPoint state_pt = calc_point(state_angle);
            
            const CvMat* prediction = cvKalmanPredict( kalman, 0 );//������һ��ʱ����Ԥ��ֵ������������Ϊ0
            float predict_angle = prediction->data.fl[0];
            CvPoint predict_pt = calc_point(predict_angle);
            
            float measurement_angle;
            CvPoint measurement_pt;

            cvRandArr( &rng, measurement, CV_RAND_NORMAL, cvRealScalar(0),
                       cvRealScalar(sqrt(kalman->measurement_noise_cov->data.fl[0])) );

            /* generate measurement */
            cvMatMulAdd( kalman->measurement_matrix, state, measurement, measurement );
            //cvMatMulAdd(src1,src2,src3,dst)����ʵ��dist=src1*src2+src3; 

            measurement_angle = measurement->data.fl[0];
            measurement_pt = calc_point(measurement_angle);
            
            //����Kalman�˲��������������µĲ���ֵ�����������ǲ�������������Ȼ���״̬���Դ��ݾ���F���һ�ε������������ǲ����Ĺ�������
            /* plot points */
            #define draw_cross( center, color, d )                                 \
                cvLine( img, cvPoint( center.x - d, center.y - d ),                \
                             cvPoint( center.x + d, center.y + d ), color, 1, CV_AA, 0); \
                cvLine( img, cvPoint( center.x + d, center.y - d ),                \
                             cvPoint( center.x - d, center.y + d ), color, 1, CV_AA, 0 )

            cvZero( img );
            //ʹ������궨��ĺ���
            draw_cross( state_pt, CV_RGB(255,255,255), 3 );//��ɫ��״̬��
            draw_cross( measurement_pt, CV_RGB(255,0,0), 3 );//��ɫ��������
            draw_cross( predict_pt, CV_RGB(0,255,0), 3 );//��ɫ�����Ƶ�
            cvLine( img, state_pt, measurement_pt, CV_RGB(255,0,0), 3, CV_AA, 0 );
            cvLine( img, state_pt, predict_pt, CV_RGB(255,255,0), 3, CV_AA, 0 );
            
            cvKalmanCorrect( kalman, measurement );//У���µĲ���ֵ

            cvRandArr( &rng, process_noise, CV_RAND_NORMAL, cvRealScalar(0),
                       cvRealScalar(sqrt(kalman->process_noise_cov->data.fl[0])));//������̬�ֲ���������
            cvMatMulAdd( kalman->transition_matrix, state, process_noise, state );

            cvShowImage( "Kalman", img );
			//����������ʱ����ʼ�µ�ѭ������ʼ������ܻ�ı䣬�����ƶ����ʻ�ı�
            code = (char) cvWaitKey( 100 );
            if( code > 0 )
                break;
        }
        if( code == 27 || code == 'q' || code == 'Q' )
            break;
    }
    
    cvDestroyWindow("Kalman");

    return 0;
}

#ifdef _EiC
main(1, "kalman.c");
#endif
