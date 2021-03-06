#include <cassert>
#include <math.h>
#include <utility>
#include <vector>
#include <iostream>
#include <time.h>
#include <string>
#include "opencv2/opencv.hpp"

#define REP(i,n) for(int i=0;i<n;i++)
#define mp std::make_pair
#define pb push_back


using namespace cv;

const double distA=6.8327;
const double distB=0.0062;
const int distR=240;
const int distC=320;

double brx=6.3,bry=16,sq=4;

void maxFilter(Mat& frame, int ind, double multA=1.22, double multB=1.5)
{
	bool Y=false;
	if(ind!=0) multA=multB=1.3;
	if(ind==3) Y=true,ind=1,multA=0.8,1.12;
	REP(i,frame.rows)
	{
		REP(j,frame.cols)
		{
			Vec3b& cur =frame.at<Vec3b>(i,j);
			if(cur[ind]>60&&cur[ind]>multA*cur[(ind+1)%3]&&cur[ind]>multB*cur[(ind+2)%3])  
			{
				cur[ind]=255;
				cur[(ind+1)%3]=Y?255:0;
				cur[(ind+2)%3]=0;
			}
			else 
			{
				cur[ind]=cur[(ind+1)%3]=cur[(ind+2)%3]=0;
			}
		}
	}
}

int** comp; std::vector<std::pair<double,double> > cents; int currentComp;
int dimR, dimC, toti, totj;
int dx[]={1,-1,0,0};
int dy[]={0,0,1,-1};
bool check(int i, int j, Mat &inFrame)
{
	if(i<0||i>=dimR) return false;
	if(j<0||j>=dimC) return false;
	REP(k,3) if(inFrame.at<Vec3b>(i,j)[k]!=0) return false;
	if(comp[i][j]==-1) return true;	
	return false;
}
bool checkin(int i, int j)
{
	if(i<0||i>=dimR) return false;
	if(j<0||j>=dimC) return false;
	return true;
}
int dfs(int i, int j, Mat &inFrame)
{
	int ar=1;
	toti+=i, totj+=j;
	comp[i][j]=currentComp;
	inFrame.at<Vec3b>(i,j)[1]=255;
	REP(k,4)
	{
		int ni=i+dx[k],nj=j+dy[k];
		if(!check(ni,nj,inFrame)) continue;
		ar+=dfs(ni,nj,inFrame);
	}
	return ar;
}
void fill(Mat &inFrame)
{
	comp=new int*[inFrame.rows];
	cents.resize(0);
	currentComp=0;
	dimR=inFrame.rows, dimC=inFrame.cols;
	std::cout<<dimR<<" "<<dimC<<std::endl;
	REP(i,inFrame.rows)
	{
		comp[i]=new int[inFrame.cols];
		REP(j,inFrame.cols) comp[i][j]=-1;
	}
	REP(i,inFrame.rows) REP(j,inFrame.cols) 
	{
		if(!check(i,j,inFrame)) continue;
		toti=0,totj=0;
		int ar=dfs(i,j,inFrame);
		int ci=toti/((double)ar);
		int cj=totj/((double)ar);
		if(ar>=175&&ar<=3000&&ci>60)
		{
			std::cout<<ci<<" "<<cj<<std::endl;
			cents.pb(mp(ci,cj));
			REP(x,5) REP(y,5) 
			{
				int ni=ci+x,nj=cj+y;
				if(checkin(ni,nj))
				{
					//std::cout<<ni<<"" <<nj<<std::endl;
					inFrame.at<Vec3b>(ni,nj)[1]=0,inFrame.at<Vec3b>(ni,nj)[2]=255;
				}
			}
		}
		currentComp++;
	}
}

int main(int argc, char** argv)
{
	char* fileName=argv[1];
	Mat in=imread(fileName,1);
	maxFilter(in,0,0.85,0.85);
	fill(in);
	imwrite("calib.jpg", in );
	return 0;
}
