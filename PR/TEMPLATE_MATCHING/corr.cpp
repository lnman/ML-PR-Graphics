#include <bits/stdc++.h>
#include "bitmap_image.hpp"
using namespace std;

#define MX 1024
#define LEVEL 5

const int inf = 0x3f3f3f3f;

double templ[MX][MX],original[MX][MX];
int templ_h,templ_w,original_h,original_w;

double templ_sub_sample[LEVEL][MX][MX],original_sub_sample[LEVEL][MX][MX];
int templ_sub_h[LEVEL],templ_sub_w[LEVEL],original_sub_h[LEVEL],original_sub_w[LEVEL];

void image_read(string name,double arr[MX][MX],int& height,int& width){
	bitmap_image image(name);
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	height = image.height();
	width  = image.width();
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			image.get_pixel(x,y,red,green,blue);
			arr[x][y]=double(red)*0.299+double(green)*0.587+double(blue)*0.144;
			arr[x][y]/=255;
		}
	}
}


void create_image(string name,int pos_x,int pos_y,string name2){
	bitmap_image image(name);
	for (std::size_t y = pos_y; y < pos_y+templ_h; ++y)
	{
		image.set_pixel(pos_x,y,255,0,0);
		image.set_pixel(pos_x+templ_w-1,y,255,0,0);
	}
	for (std::size_t x = pos_x; x < pos_x+templ_w; ++x)
	{
		image.set_pixel(x,pos_y,255,0,0);
		image.set_pixel(x,pos_y+templ_h-1,255,0,0);
	}
	image.save_image(name2);
}


double cross_correlation(int x,int y){
	double val=0;
	double val_n=0;
	for (int i = 0; i < templ_h; ++i)
	{
		for (int j = 0; j < templ_w; ++j)
		{
			val+=templ[j][i]*original[j+x][i+y];
			val_n+=original[j+x][i+y]*original[j+x][i+y];
		}
	}
	val_n=sqrt(val_n);
	return val/val_n;
}


void correlation(){
	image_read("base.bmp",original,original_h,original_w);
	image_read("template.bmp",templ,templ_h,templ_w);
	int d_h=original_h-templ_h;
	int d_w=original_w-templ_w;
	int pos_x=0,pos_y=0;
	double mmax=0;
	for (int y = 0; y < d_h; ++y)
	{
		for (int x = 0; x < d_w; ++x)
		{
			double v1=cross_correlation(x,y);
			if(v1>mmax){
				mmax=v1;
				pos_x=x;
				pos_y=y;
			}
		}
	}
	printf("%d %d\n",pos_x,pos_y);
	create_image("base.bmp",pos_x,pos_y,"cor.bmp");
}


void logarithmic(){
	image_read("base.bmp",original,original_h,original_w);
	image_read("template.bmp",templ,templ_h,templ_w);
	int init_x,init_y,p;
	//for testing
	init_x=140,init_y=140;
	p=63;
	cin>>init_x>>init_y>>p;
	if(init_x>=original_w-templ_w)init_x=original_w-templ_w-1;
	if(init_y>=original_h-templ_h)init_y=original_h-templ_h-1;

	int pos_x=0,pos_y=0;
	double mmax=0;
	int ppp[8][2]={{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}};
	mmax=cross_correlation(init_x,init_y);
	pos_x=init_x,pos_y=init_y;
	int k=ceil(log2(p));
	while(k>=1){
		int d1=pow(2,k-1);
		for (int i = 0; i < 8; ++i)
		{
			int temp_x=init_x+ppp[i][0]*d1;
			int temp_y=init_y+ppp[i][1]*d1;
			double v1=cross_correlation(temp_x,temp_y);
			if(v1>mmax){
				mmax=v1;
				pos_x=temp_x;
				pos_y=temp_y;
			}
		}
		init_x=pos_x;
		init_y=pos_y;
		k--;

	}
	printf("%d %d\n",pos_x,pos_y);
	create_image("base.bmp",pos_x,pos_y,"log.bmp");

}





void subsample(double arr[MX][MX],int w1,int h1,double arr2[MX][MX]){
	double tot;
	for (int i = 0; i < h1-1; i+=2)
	{
		for (int j = 0; j < w1-1; j+=2)
		{
			tot=arr[i][j]+arr[i][j+1]+arr[i+1][j]+arr[i+1][j+1];
			arr2[i/2][j/2]=tot/4;
			//arr[i/2][j/2]=arr[i][j];
		}
	}
}


double cross_correlation(double ori[MX][MX],double tmp[MX][MX],int tmp_h,int tmp_w,int x,int y){
	double val=0;
	double val_n=0;
	for (int i = 0; i < tmp_h; ++i)
	{
		for (int j = 0; j < tmp_w; ++j)
		{
			val+=tmp[j][i]*ori[j+x][i+y];
			val_n+=ori[j+x][i+y]*ori[j+x][i+y];
		}
	}
	val_n=sqrt(val_n);
	return val/val_n;
}


void heirarchical_logarithmic(double ori[MX][MX],double tmp[MX][MX],int tmp_h,int tmp_w,int init_x,int init_y,int p,int &res_x,int& res_y){
	int pos_x=0,pos_y=0;
	double mmax=0;
	int ppp[8][2]={{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}};
	mmax=cross_correlation(ori,tmp,tmp_h,tmp_w,init_x,init_y);
	pos_x=init_x,pos_y=init_y;
	int k=ceil(log2(p));
	while(k>=1){
		int d1=floor(pow(2,k-1));
		for (int i = 0; i < 8; ++i)
		{
			int temp_x=init_x+ppp[i][0]*d1;
			int temp_y=init_y+ppp[i][1]*d1;
			//cout<<p<<" "<<temp_x<<" "<<temp_y<<endl;
			double v1=cross_correlation(ori,tmp,tmp_h,tmp_w,temp_x,temp_y);
			if(v1>=mmax){
				mmax=v1;
				pos_x=temp_x;
				pos_y=temp_y;
			}
		}
		init_x=pos_x;
		init_y=pos_y;
		k--;
	}
	res_x=pos_x;
	res_y=pos_y;
	printf("%d %d\n",pos_x,pos_y);

}


void heirarchical(){
	image_read("base.bmp",original_sub_sample[0],original_sub_h[0],original_sub_w[0]);
	image_read("template.bmp",templ_sub_sample[0],templ_sub_h[0],templ_sub_w[0]);
	int no_of_level=2;
	int divI=1;
	for (int i = 0; i < no_of_level; ++i)
	{
		subsample(original_sub_sample[i],original_sub_h[i],original_sub_w[i],original_sub_sample[i+1]);
		subsample(templ_sub_sample[i],templ_sub_h[i],templ_sub_w[i],templ_sub_sample[i+1]);
		original_sub_w[i+1]=original_sub_w[i]/2;
		original_sub_h[i+1]=original_sub_h[i]/2;
		templ_sub_h[i+1]=templ_sub_h[i]/2;
		templ_sub_w[i+1]=templ_sub_w[i]/2;
		divI*=2;
	}

	int init_x,init_y,p,res_x,res_y,tt_x,tt_y;
	//for testing
	init_x=160,init_y=160;
	p=63;
	cin>>init_x>>init_y>>p;
	if(init_x>=original_sub_w[0]-templ_sub_w[0])init_x=original_sub_w[0]-templ_sub_w[0]-1;
	if(init_y>=original_sub_h[0]-templ_sub_h[0])init_y=original_sub_h[0]-templ_sub_h[0]-1;

	for (int i = no_of_level; i >=0; --i)
	{
		if(i==no_of_level){
			heirarchical_logarithmic(original_sub_sample[i],templ_sub_sample[i],templ_sub_h[i],templ_sub_w[i],init_x/divI,init_y/divI,p/divI,res_x,res_y);
			tt_x=(init_x/divI-res_x)*2+init_x*2/divI;
			tt_y=(init_y/divI-res_y)*2+init_y*2/divI;
		}else{
			heirarchical_logarithmic(original_sub_sample[i],templ_sub_sample[i],templ_sub_h[i],templ_sub_w[i],tt_x,tt_y,2,res_x,res_y);
			tt_x=(init_x/divI-res_x)*2+init_x*2/divI;
			tt_y=(init_y/divI-res_y)*2+init_y*2/divI;
		}
		divI/=2;
	}
	bitmap_image image("base.bmp");
	for (std::size_t y = res_y; y < res_y+templ_sub_h[0]; ++y)
	{
		image.set_pixel(res_x,y,255,0,0);
		image.set_pixel(res_x+templ_sub_w[0]-1,y,255,0,0);
	}
	for (std::size_t x = res_x; x < res_x+templ_sub_w[0]; ++x)
	{
		image.set_pixel(x,res_y,255,0,0);
		image.set_pixel(x,res_y+templ_sub_h[0]-1,255,0,0);
	}
	image.save_image("hier.bmp");
}


int main(int argc, char const *argv[])
{
	int t;
	cin>>t;
	if(t==1){
		correlation();
	}else if(t==2){
		logarithmic();
	}else if(t==3){
		heirarchical();
	}
	return 0;
}