/*************************************************************************
	> File Name: DRCC.cpp
	> Author: Leosocy
	> Mail: 513887568@qq.com 
	> Created Time: 2017/04/06 22:12:08
 ************************************************************************/

#include "coding_algorithm.h"
#include "../image_transform/gabor.h"

DRCC::DRCC()
{
	this->numOfScales = 3;
	this->numOfDirections = 9;
}

DRCC::~DRCC()
{

}

int DRCC::getMaxGaborResponse( const Mat &src, Mat &result, Mat &C, Mat &Cs, int numOfScales, int numOfDirections, int kernelType )
{
	assert( numOfScales >= 1 && numOfDirections >= 1 );
	GaborFilter filter;
	Mat batchResult;
	Mat gaborFilter;
	filter.numOfScales = numOfScales;
	filter.numOfDirections = numOfDirections;
	filter.kernelSize = Size( 11, 11 );
	filter.doBatchGaborFilter( src, batchResult, kernelType, true );
	filter.showGaborFilter( gaborFilter, GaborFilter::GABOR_KERNEL_REAL );
	//imshow( "gabor filter result", batchResult );
	//imshow( "gabor filter", gaborFilter );
	vector< Mat > mv;
	vector< Mat >::iterator iter;
	split( batchResult, mv );
	for( iter = mv.begin(); iter != mv.end(); ++iter ) {
		GaussianBlur( *iter, *iter, Size( 11, 11 ), 1, 0 );
	}
	int height = src.rows;
	int width = src.cols;
	int Cleft = -1, Cright = -1;
	//int freq[6] = { 0 };
	result = Mat( src.size(), CV_64F );
	C = Mat( src.size(), CV_8S );
	Cs = Mat( src.size(), CV_8S );
	for( int i = 0; i < height; ++i ) {
		for( int j = 0; j < width; ++j ) {
			double maxResponse = DBL_MIN;
			int maxScale = -1;
			int maxDirection = -1;
			for( int s = 0; s < numOfScales; ++s ) {
				for( int d = 0; d < numOfDirections; ++d ) {
					if( mv[ s * numOfScales + d ].at<double>( i ,j ) > maxResponse ) {
						maxResponse = mv[ s * numOfScales + d ].at<double>( i, j );
						maxScale = s;
						maxDirection = d;
					}
				}
			}
			//printf( "index:%d maxScale:%d maxDirection:%d value:%lf\n", j, maxScale, maxDirection, maxResponse );
			result.at<double>( i, j ) = maxResponse;
	//		++freq[ maxDirection ];
			C.at<char>( i, j ) = maxDirection;
			if( maxDirection == numOfDirections - 1 ) {
				Cleft = 0;
			} else {
				Cleft = maxDirection + 1;
			}

			if( maxDirection == 0 ) {
				Cright = numOfDirections - 1;
			} else {
				Cright = maxDirection -1 ;
			}
			Cs.at<char>( i, j ) = mv[ maxScale * numOfScales + Cleft ].at<double>( i, j ) >= mv[ maxScale * numOfScales + Cright].at<double>( i, j )  ? 1 : 0; 
		}
	}
	/*for( int i = 0; i < 6; ++i ) {
		printf( "%d:%d\n", i, freq[i] );
	}*/
	return EXIT_SUCCESS;
}

int DRCC::doOnceDRCC( const Mat &src, const string &label )
{
	Mat maxResponseResult, C, Cs;
	this->numOfScales = 1;

	getMaxGaborResponse( src, maxResponseResult, C, Cs, this->numOfScales, this->numOfDirections, GaborFilter::GABOR_KERNEL_REAL );
	
	for( int h = 0; h < maxResponseResult.rows; ++h ) {
		for( int w = 0; w < maxResponseResult.cols; ++w ) {
			maxResponseResult.at<double>( h, w ) = 1 - maxResponseResult.at<double>( h, w );
		}
	}
	imshow( "max response", maxResponseResult );
	this->CVector.push_back( C );
	this->CsVector.push_back( Cs );
	this->Labels.push_back( label );
	return EXIT_SUCCESS; 
}

int DRCC::doBatchDRCC( const char *filename )
{
	FILE *roi_list = fopen( filename, "r" );
	if( roi_list == NULL ) {
		printf( "%s Not Exist.\n", filename );
		return EXIT_FAILURE;
	}
	string feature_string = "feature_string_info/CODING_FEATURES.xml";
	FileStorage feature_file( feature_string, FileStorage::READ );
	FileNode n;
	vector< Mat >v_feature;

	clock_t bt = clock();
	printf( "--------------------------------Begin Coding Train--------------------\n" );
	for( int i = 0; i < LIST_TRAIN_NUM_TOTAL && !feof( roi_list ); ++i ) {
		char num[10];
		sprintf( num, "%c%d", '_', i );
		const string s_num( num );

		n = feature_file[num];
		if( n.empty() ) {
			printf( "Begin Write Num:%d\n", i );
			feature_file.open( feature_string, FileStorage::APPEND );
			char id[10];
			char image_path[200];
			fscanf( roi_list, "%s %s", id, image_path );
			Mat image = imread( image_path, CV_LOAD_IMAGE_COLOR );
			Mat image_gray;
			cvtColor( image, image_gray, CV_BGR2GRAY );
			double proportion = (double)image.cols / image.rows;
			Size dsize = Size( IMAGE_HEIGHT * proportion, IMAGE_HEIGHT );
			resize(image_gray, image_gray, dsize);
			//Mat gaborResult;
			//GaborFilter filter;
			//filter.numOfDirections = 12;
			//filter.doBatchGaborFilter( image_gray, gaborResult, GaborFilter::GABOR_KERNEL_REAL );
			Mat t;	
			image_gray.convertTo( t, CV_64FC1 );
			normalize( t, t, 0, 1, CV_MINMAX );	
			for( int h = 0; h < image_gray.rows; ++h ) {
				for( int w = 0; w < image_gray.cols; ++w ) {
					t.at<double>( h, w ) = 1 - t.at<double>( h, w );
				}
			}
			imshow( "origin", t );
			doOnceDRCC( t, id );
			printf( "End Write Num:%d\n\n", i );
		} else {
			
		}
	}
	feature_file.release();
	clock_t et = clock();
	printf( "--------------------------End Of Train----------------------------\n\n" );
	printf( "Total Cost Time: %lf  Per Image Cost Time: %lf\n\n", ( (double)( et ) - bt ) / CLOCKS_PER_SEC, ( ( ( double )et - bt ) / CLOCKS_PER_SEC ) / LIST_TRAIN_NUM_TOTAL );
	
	fflush( stdout );
	double score = 0.0;
	int right = 0;
	for( int i = 0; i < CVector.size(); ++i ) {
		double maxScore = DBL_MIN;
		int maxIndex = -1;
		for( int j = 0; j < CVector.size(); ++j ) {
			if( i != j ) {
				score = matching( this->CVector[i], this->CsVector[i], this->CVector[j], this->CsVector[j] );
				if( score > maxScore && i != j ) {
					maxScore = score;
					maxIndex = j;			
				}	
			}
		}	
		if( Labels[i] == Labels[maxIndex] ) {
			++right;		
		}
		printf( "src1:%s  src2:%s  Score:%lf\n\n", Labels[i].c_str(), Labels[maxIndex].c_str(), score );	
	}
	printf( "GAR:%lf", (double)right / CVector.size() );

	return EXIT_SUCCESS;
}

double DRCC::matching( const Mat &Cx, const Mat &Csx, const Mat &Cy, const Mat &Csy )
{
	assert( Cx.rows == Csx.rows && Cy.rows == Csy.rows && Cx.rows == Cy.rows && Cx.cols == Csx.cols && Cy.cols == Csy.cols && Cx.cols == Cy.cols );
	int width = Cx.cols;
	int height = Cx.rows;
	
	double score = 0.0;
	
	for( int i = 0; i < height; ++i ) {
		for( int j = 0; j < width; ++j ) {
			score += ( Cx.at<char>( i, j ) == Cy.at<char>( i, j ) ) + ( ( Cx.at<char>( i, j ) == Cy.at<char>( i, j ) ) && ~( Csx.at<char>( i, j ) ^ Csy.at<char>( i, j ) ) ); 		
		}	
	}
	return score / ( 2 * width * height );
}

int DRCC::saveFeatures( const char *filename )
{
}

int DRCC::loadFeatures( const char *filename )
{
}


