#include <stdafx.h>
#include <stdio.h>
#include "BmpRot.h"
#include "stdlib.h"
#include "math.h"
#include <iostream>

#define PI 3.14159//圆周率宏定义

using namespace std;



//显示位图文件头信息
void showBmpHead(BITMAPFILEHEADER pBmpHead){
	cout<<"位图文件头:"<<endl;
	cout<<"文件大小:"<<pBmpHead.bfSize<<endl;
	cout<<"保留字_1:"<<pBmpHead.bfReserved1<<endl;
	cout<<"保留字_2:"<<pBmpHead.bfReserved2<<endl;
	cout<<"实际位图数据的偏移字节数:"<<pBmpHead.bfOffBits<<endl<<endl;
}

void showBmpInforHead(tagBITMAPINFOHEADER pBmpInforHead){
	cout<<"位图信息头:"<<endl;
	cout<<"结构体的长度:"<<pBmpInforHead.biSize<<endl;
	cout<<"位图宽:"<<pBmpInforHead.biWidth<<endl;
	cout<<"位图高:"<<pBmpInforHead.biHeight<<endl;
	cout<<"biPlanes平面数:"<<pBmpInforHead.biPlanes<<endl;
	cout<<"biBitCount采用颜色位数:"<<pBmpInforHead.biBitCount<<endl;
	cout<<"压缩方式:"<<pBmpInforHead.biCompression<<endl;
	cout<<"biSizeImage实际位图数据占用的字节数:"<<pBmpInforHead.biSizeImage<<endl;
	cout<<"X方向分辨率:"<<pBmpInforHead.biXPelsPerMeter<<endl;
	cout<<"Y方向分辨率:"<<pBmpInforHead.biYPelsPerMeter<<endl;
	cout<<"使用的颜色数:"<<pBmpInforHead.biClrUsed<<endl;
	cout<<"重要颜色数:"<<pBmpInforHead.biClrImportant<<endl;
}

bool RotateBmp(LPCWSTR strSrcFile, LPCWSTR strDestFile, int nAngle)
{

	//变量定义
	BITMAPFILEHEADER strHead;
	RGBQUAD strPla[256];//256色调色板
	BITMAPINFOHEADER strInfo;

	IMAGEDATA *imagedata = NULL;//动态分配存储原图片的像素信息的二维数组
	IMAGEDATA *imagedataRot = NULL;//动态分配存储旋转后的图片的像素信息的二维数组
	int width,height;//图片的宽度和高度

	FILE *fpi,*fpw;
	_wfopen_s(&fpi, strSrcFile, TEXT("rb"));
	if(fpi == NULL)
	{
		//cout<<"file open error!"<<endl;
		TRACE(TEXT("file open error!"));
		return false;
	}
	//读取bmp文件的文件头和信息头
	fread(&strHead, sizeof(BITMAPFILEHEADER), 1, fpi);
	//showBmpHead(strHead);//显示文件头
	if(0x4d42 != strHead.bfType)
	{
		//cout<<"the file is not a bmp file!"<<endl;
		TRACE(TEXT("the file is not a bmp file!"));
		return false;
	}
	fread(&strInfo,1,sizeof(tagBITMAPINFOHEADER),fpi);
	//showBmpInforHead(strInfo);//显示文件信息头

	//读取调色板
	for(unsigned int nCounti=0;nCounti<strInfo.biClrUsed;nCounti++)
	{
		fread((char *)&(strPla[nCounti].rgbBlue),1,sizeof(BYTE),fpi);
		fread((char *)&(strPla[nCounti].rgbGreen),1,sizeof(BYTE),fpi);
		fread((char *)&(strPla[nCounti].rgbRed),1,sizeof(BYTE),fpi);
		fread((char *)&(strPla[nCounti].rgbReserved),1,sizeof(BYTE),fpi);
	}

	width = strInfo.biWidth;
	height = strInfo.biHeight;
	//图像每一行的字节数必须是4的整数倍
	width = (width/* * sizeof(IMAGEDATA)*/ + 3) / 4 * 4;
	imagedata = (IMAGEDATA*)malloc(width * height * sizeof(IMAGEDATA));
	//imagedata = (IMAGEDATA*)malloc(width * height);
	imagedataRot = (IMAGEDATA*)malloc(2 * width * 2 * height * sizeof(IMAGEDATA));
	//初始化原始图片的像素数组
	int pixel = 0;
	unsigned char pixBuf[3] = {0};
	for(int i = 0;i < height;++i)
	{
		for(int j = 0;j < width;++j)
		{
			fread(pixBuf, 1, 3, fpi);

			(*(imagedata + i * width + j)).blue = pixBuf[0];
			(*(imagedata + i * width + j)).green = pixBuf[1];
			(*(imagedata + i *  width + j)).red = pixBuf[2];
			if (j >= (width-2))
			{

 				(*(imagedata + i * width + j)).blue = 0xFF;
				(*(imagedata + i * width + j)).green = 0xFF;
				(*(imagedata + i *  width + j)).red = 0xFF;
			}
		}
	}
	//初始化旋转后图片的像素数组
	for(int i = 0;i < 2 * height;++i)
	{
		for(int j = 0;j < 2 * width;++j)
		{
			(*(imagedataRot + i * 2 * width + j)).blue = 0xFF;
			(*(imagedataRot + i * 2 * width + j)).green = 0xFF;
			(*(imagedataRot + i * 2 * width + j)).red = 0xFF;
		}
	}
	//fseek(fpi,54,SEEK_SET);
	//读出图片的像素数据
	//fread(imagedata,sizeof(struct tagIMAGEDATA) * width,height,fpi);
	fclose(fpi);
	
	//图片旋转处理
	int RotateAngle = nAngle;//要旋转的角度数
	double angle;//要旋转的弧度数
	int midX_pre,midY_pre,midX_aft,midY_aft;//旋转所围绕的中心点的坐标
	midX_pre = width / 2;
	midY_pre = height / 2;
	midX_aft = width;
	midY_aft = height;
	int pre_i,pre_j,after_i,after_j;//旋转前后对应的像素点坐标
	
	angle = 1.0 * RotateAngle * PI / 180;
	int rgb = 0;
	for(int i = 0;i < 2 * height;++i)
	{
		for(int j = 0;j < 2 * width;++j)
		{
			after_i = i - midX_aft;//坐标变换
			after_j = j - midY_aft;
			pre_i = (int)(cos((double)angle) * after_i - sin((double)angle) * after_j) + midX_pre;
			pre_j = (int)(sin((double)angle) * after_i + cos((double)angle) * after_j) + midY_pre;
			if(pre_i >= 0 && pre_i < height && pre_j >= 0 && pre_j < width)//在原图范围内
			{
				IMAGEDATA *pRot = (imagedataRot + i * 2 * width + j);
				IMAGEDATA *pImage = (imagedata + pre_i * width + pre_j);
				rgb = pImage->blue + pImage->green + pImage->red;
				if (rgb > 500)
				{
					pRot->blue = 0xFF;
					pRot->green = 0xFF;
					pRot->red = 0xFF;
				}
				else
				{
					pRot->blue = 0;
					pRot->green = 0;
					pRot->red = 0;
				}
			}
		}
	}

	//保存bmp图片
	_wfopen_s(&fpw, strDestFile,TEXT("wb"));
	if(fpw==NULL)
	{
		//cout<<"create the bmp file error!"<<endl;
		TRACE(TEXT("create the bmp file error!"));

		delete[] imagedata;
		delete[] imagedataRot;
		return false;
	}
	
	fwrite(&strHead, sizeof(tagBITMAPFILEHEADER), 1, fpw);
	strInfo.biWidth = 2 * width;
	strInfo.biHeight = 2 * height;
	fwrite(&strInfo, sizeof(tagBITMAPINFOHEADER) ,1 ,fpw);
	//保存调色板数据
	for(int nCounti=0;nCounti<strInfo.biClrUsed;nCounti++)
	{
	/*	fwrite(&strPla[nCounti].rgbBlue,1,sizeof(BYTE),fpw);
		fwrite(&strPla[nCounti].rgbGreen,1,sizeof(BYTE),fpw);
		fwrite(&strPla[nCounti].rgbRed,1,sizeof(BYTE),fpw);
		fwrite(&strPla[nCounti].rgbReserved,1,sizeof(BYTE),fpw);*/
		fwrite(&strPla[nCounti], sizeof(RGBQUAD), 1, fpw);
	}
	//保存像素数据
	//for(int i =0;i < 2 * height;++i)
	//{
	//	for(int j = 0;j < 2 * width;++j)
	//	{
	//		/*fwrite( &((*(imagedataRot + i * 2 * width + j)).blue),1,sizeof(BYTE),fpw);
	//		fwrite( &((*(imagedataRot + i * 2 * width + j)).green),1,sizeof(BYTE),fpw);
	//		fwrite( &((*(imagedataRot + i * 2 * width + j)).red),1,sizeof(BYTE),fpw);*/
	//		//fwrite((imagedataRot+i*2*width+j), sizeof(IMAGEDATA),1, fpw);
	//	}
	//}
	fwrite(imagedataRot, sizeof(IMAGEDATA), 2 * width * 2 * height, fpw);
	fclose(fpw);

	//释放内存
	delete[] imagedata;
	delete[] imagedataRot;

	return true;
}