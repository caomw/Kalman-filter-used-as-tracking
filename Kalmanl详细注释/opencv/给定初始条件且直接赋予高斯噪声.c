int main (int argc,int ** argv)
����{
����	// Initialize Kalman filter object, window, number generator, etc
����	cvNamedWindow( "Kalman", 1 );//�������ڣ���Ϊ��ʱ�򣬱�ʾ���ڴ�С�Զ��趨
����	CvRandState rng;
����	//cvRandInit( &rng, 0, 1, -1, CV_RAND_UNI );/* CV_RAND_UNI ָ��Ϊ���ȷֲ����͡����������Ϊ-1 *///��ô�������ָ�Ϊ��˹�ֲ�
����
����	cvRandInit(&rng,0,0.1,-1,CV_RAND_NORMAL);//�Լ���ӣ�ԭ����������Ϊ���ȷֲ��ٸĻأ�����û�б�Ҫ
����	IplImage* img = cvCreateImage( cvSize(500,500), 8, 3 );
����	CvKalman* kalman = cvCreateKalman( 2, 1, 0 );/*״̬����Ϊ2ά���۲�����Ϊ1ά���޼�������ά*/
����
����	// State is phi, delta_phi - angle and angular velocity
����	// Initialize with random guess
����	CvMat* x_k = cvCreateMat( 2, 1, CV_32FC1 );/*�������С�Ԫ������ΪCV_32FC1��Ԫ��Ϊλ��ͨ���������;���*/
����	//cvRandSetRange( &rng, 0, 0.1, 0 );/*�����������Χ�������������̬�ֲ�����ֵΪ��������Ϊ.1��ͨ������Ϊ*/
����	//rng.disttype = CV_RAND_NORMAL;
����	//cvRand( &rng, x_k ); /*����������*/
����	x_k->data.fl[0]=0.;//����ʼ�Ƕ�Ϊ0��//�Լ��޸Ĺ���������x_kΪ������Բ���˶��Ĳ�����ȥ�����������ں�����kalman
����
����	//�˲���У������˶������Ա�
����	x_k->data.fl[1]=0.05f;//����ٶ�Ϊ1,����ֵ
����
����	// Process noise
����	CvMat* w_k = cvCreateMat( 2, 1, CV_32FC1 );
����
����	// Measurements, only one parameter for angle
����	CvMat* z_k = cvCreateMat( 1, 1, CV_32FC1 );/*����۲����*/
����	cvZero( z_k ); /*��������*/
����
����	// Transition matrix F describes model parameters at and k and k+1
����	const float F[] = { 1, 1, 0, 1 }; /*״̬ת�ƾ���*///F ����ʵ����Ӧ����2*2�ľ���[1,1;0,1]
����	memcpy( kalman->transition_matrix->data.fl, F, sizeof(F));
����	/*��ʼ��ת�ƾ������У������CvKalman* kalman = cvCreateKalman( 2, 1, 0 );*/
����
����	// Initialize other Kalman parameters
����	cvSetIdentity( kalman->measurement_matrix, cvRealScalar(1) );/*�۲����*///�۲����Ҳ��1*2 [1,0],��Ϊֻ�ܲ��������λ��
����	cvSetIdentity( kalman->process_noise_cov, cvRealScalar(1e-5) );/*��������*/
����	cvSetIdentity( kalman->measurement_noise_cov, cvRealScalar(1e-1) );/*�۲�����*/
����	cvSetIdentity( kalman->error_cov_post, cvRealScalar(1) );/*�������Э����*/
����
����	// Choose random initial state
����	cvRand( &rng, kalman->state_post );/*��ʼ��״̬����*/
����
����	// Make colors
����	CvScalar yellow = CV_RGB(255,255,0);/*����Ϊ��������ɫ*/
����	CvScalar white = CV_RGB(255,255,255);
����	CvScalar red = CV_RGB(255,0,0);
����
����	while( 1 ){
����	 // Predict point position
����	 const CvMat* y_k = cvKalmanPredict( kalman, 0 );/*����������Ϊ*/
����
����	 // Generate Measurement (z_k)
����	 cvRandSetRange( &rng, 0, sqrt( kalman->measurement_noise_cov->data.fl[0] ), 0 );/*���ù۲�����*/ 
����	 cvRand( &rng, z_k );//��ʱ��z_k���ǲ���ֵ��ֻ��Ϊ�۲���������ʵΪ��ⷽ��Ӧ����һ�Ķ����ı���
����	 cvMatMulAdd( kalman->measurement_matrix, x_k, z_k, z_k );//z_kΪ����ֵ
����
����	 // Update Kalman filter state
����	 cvKalmanCorrect( kalman, z_k );
����
����	 //// Apply the transition matrix F and apply "process noise" w_k
����	 //cvRandSetRange( &rng, 0, sqrt( kalman->process_noise_cov->data.fl[0] ), 0 );/*������̬�ֲ���������*/
����	 //cvRand( &rng, w_k );
����	 //cvMatMulAdd( kalman->transition_matrix, x_k, w_k, x_k );//
����
����	 // Plot Points
����	 cvZero( img );/*����ͼ��*/
����	 // Yellow is observed state ��ɫ�ǹ۲�ֵ
����	 //cvCircle(IntPtr, Point, Int32, MCvScalar, Int32, LINE_TYPE, Int32)
����	 //��Ӧ���������У�shiftΪ���ݾ���
����	 //cvCircle(img, center, radius, color, thickness, lineType, shift)
����	 //���ƻ����һ������Բ�ĺͰ뾶��Բ
����	 cvCircle( img, 
����	 cvPoint( cvRound(img->width/2 + img->width/3*cos(z_k->data.fl[0])),
����	 cvRound( img->height/2 - img->width/3*sin(z_k->data.fl[0])) ), 
����	 4, yellow );
����	 // White is the predicted state via the filter
����	 cvCircle( img, 
����	 cvPoint( cvRound(img->width/2 + img->width/3*cos(y_k->data.fl[0])),
����	 cvRound( img->height/2 - img->width/3*sin(y_k->data.fl[0])) ), 
����	 4, white, 2 );
����	 // Red is the real state
����	 cvCircle( img, 
����	 cvPoint( cvRound(img->width/2 + img->width/3*cos(x_k->data.fl[0])),
����	 cvRound( img->height/2 - img->width/3*sin(x_k->data.fl[0])) ),
����	 4, red );
����	 CvScalar bule=cvScalar(255,0,0,0);
����	 cvCircle( img, 
����	 cvPoint( cvRound(img->width/2 + img->width/3*cos(kalman->state_post->data.fl[0])),
����	 cvRound( img->height/2 - img->width/3*sin(kalman->state_post->data.fl[0])) ),
����	 4, bule ,2);
����
����	 // Apply the transition matrix F and apply "process noise" w_k
����	 //cvRandSetRange( &rng, 0, sqrt( kalman->process_noise_cov->data.fl[0] ), 0 );/*������̬�ֲ���������*/
����	 //cvRand( &rng, w_k );
����	 cvMatMulAdd( kalman->transition_matrix, x_k, NULL, x_k );//
����
����	 CvFont font;
����	 cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX,0.5f,0.5f,0,1,8);
����	 cvPutText(img,"Yellow:observe",cvPoint(0,20),&font,cvScalar(0,0,255));
����	 cvPutText(img,"While:predict",cvPoint(0,40),&font,cvScalar(0,0,255));
����	 cvPutText(img,"Red:real",cvPoint(0,60),&font,cvScalar(0,0,255));
����	 cvPutText(img,"Press Esc to Exit...",cvPoint(0,80),&font,cvScalar(255,255,255));
����	 cvShowImage( "Kalman", img ); 
����
����	 // Exit on esc key
����	 if(cvWaitKey(100) == 27) 
����	 break;
����	}
����	cvReleaseImage(&img);/*�ͷ�ͼ��*/
����	cvReleaseKalman(&kalman);/*�ͷ�kalman�˲�����*/
����	cvDestroyAllWindows();/*�ͷ����д���*/
����}