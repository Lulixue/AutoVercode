#include <stdafx.h>
#include <stdio.h>
#include "BmpRot.h"
#include "stdlib.h"
#include "math.h"
#include <iostream>

#define PI 3.14159//Բ���ʺ궨��

using namespace std;



//��ʾλͼ�ļ�ͷ��Ϣ
void showBmpHead(BITMAPFILEHEADER pBmpHead){
	cout<<"λͼ�ļ�ͷ:"<<endl;
	cout<<"�ļ���С:"<<pBmpHead.bfSize<<endl;
	cout<<"������_1:"<<pBmpHead.bfReserved1<<endl;
	cout<<"������_2:"<<pBmpHead.bfReserved2<<endl;
	cout<<"ʵ��λͼ���ݵ�ƫ���ֽ���:"<<pBmpHead.bfOffBits<<endl<<endl;
}

void showBmpInforHead(tagBITMAPINFOHEADER pBmpInforHead){
	cout<<"λͼ��Ϣͷ:"<<endl;
	cout<<"�ṹ��ĳ���:"<<pBmpInforHead.biSize<<endl;
	cout<<"λͼ��:"<<pBmpInforHead.biWidth<<endl;
	cout<<"λͼ��:"<<pBmpInforHead.biHeight<<endl;
	cout<<"biPlanesƽ����:"<<pBmpInforHead.biPlanes<<endl;
	cout<<"biBitCount������ɫλ��:"<<pBmpInforHead.biBitCount<<endl;
	cout<<"ѹ����ʽ:"<<pBmpInforHead.biCompression<<endl;
	cout<<"biSizeImageʵ��λͼ����ռ�õ��ֽ���:"<<pBmpInforHead.biSizeImage<<endl;
	cout<<"X����ֱ���:"<<pBmpInforHead.biXPelsPerMeter<<endl;
	cout<<"Y����ֱ���:"<<pBmpInforHead.biYPelsPerMeter<<endl;
	cout<<"ʹ�õ���ɫ��:"<<pBmpInforHead.biClrUsed<<endl;
	cout<<"��Ҫ��ɫ��:"<<pBmpInforHead.biClrImportant<<endl;
}

bool RotateBmp(LPCWSTR strSrcFile, LPCWSTR strDestFile, int nAngle)
{

	//��������
	BITMAPFILEHEADER strHead;
	RGBQUAD strPla[256];//256ɫ��ɫ��
	BITMAPINFOHEADER strInfo;

	IMAGEDATA *imagedata = NULL;//��̬����洢ԭͼƬ��������Ϣ�Ķ�ά����
	IMAGEDATA *imagedataRot = NULL;//��̬����洢��ת���ͼƬ��������Ϣ�Ķ�ά����
	int width,height;//ͼƬ�Ŀ�Ⱥ͸߶�

	FILE *fpi,*fpw;
	_wfopen_s(&fpi, strSrcFile, TEXT("rb"));
	if(fpi == NULL)
	{
		//cout<<"file open error!"<<endl;
		TRACE(TEXT("file open error!"));
		return false;
	}
	//��ȡbmp�ļ����ļ�ͷ����Ϣͷ
	fread(&strHead, sizeof(BITMAPFILEHEADER), 1, fpi);
	//showBmpHead(strHead);//��ʾ�ļ�ͷ
	if(0x4d42 != strHead.bfType)
	{
		//cout<<"the file is not a bmp file!"<<endl;
		TRACE(TEXT("the file is not a bmp file!"));
		return false;
	}
	fread(&strInfo,1,sizeof(tagBITMAPINFOHEADER),fpi);
	//showBmpInforHead(strInfo);//��ʾ�ļ���Ϣͷ

	//��ȡ��ɫ��
	for(unsigned int nCounti=0;nCounti<strInfo.biClrUsed;nCounti++)
	{
		fread((char *)&(strPla[nCounti].rgbBlue),1,sizeof(BYTE),fpi);
		fread((char *)&(strPla[nCounti].rgbGreen),1,sizeof(BYTE),fpi);
		fread((char *)&(strPla[nCounti].rgbRed),1,sizeof(BYTE),fpi);
		fread((char *)&(strPla[nCounti].rgbReserved),1,sizeof(BYTE),fpi);
	}

	width = strInfo.biWidth;
	height = strInfo.biHeight;
	//ͼ��ÿһ�е��ֽ���������4��������
	width = (width/* * sizeof(IMAGEDATA)*/ + 3) / 4 * 4;
	imagedata = (IMAGEDATA*)malloc(width * height * sizeof(IMAGEDATA));
	//imagedata = (IMAGEDATA*)malloc(width * height);
	imagedataRot = (IMAGEDATA*)malloc(2 * width * 2 * height * sizeof(IMAGEDATA));
	//��ʼ��ԭʼͼƬ����������
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
	//��ʼ����ת��ͼƬ����������
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
	//����ͼƬ����������
	//fread(imagedata,sizeof(struct tagIMAGEDATA) * width,height,fpi);
	fclose(fpi);
	
	//ͼƬ��ת����
	int RotateAngle = nAngle;//Ҫ��ת�ĽǶ���
	double angle;//Ҫ��ת�Ļ�����
	int midX_pre,midY_pre,midX_aft,midY_aft;//��ת��Χ�Ƶ����ĵ������
	midX_pre = width / 2;
	midY_pre = height / 2;
	midX_aft = width;
	midY_aft = height;
	int pre_i,pre_j,after_i,after_j;//��תǰ���Ӧ�����ص�����
	
	angle = 1.0 * RotateAngle * PI / 180;
	int rgb = 0;
	for(int i = 0;i < 2 * height;++i)
	{
		for(int j = 0;j < 2 * width;++j)
		{
			after_i = i - midX_aft;//����任
			after_j = j - midY_aft;
			pre_i = (int)(cos((double)angle) * after_i - sin((double)angle) * after_j) + midX_pre;
			pre_j = (int)(sin((double)angle) * after_i + cos((double)angle) * after_j) + midY_pre;
			if(pre_i >= 0 && pre_i < height && pre_j >= 0 && pre_j < width)//��ԭͼ��Χ��
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

	//����bmpͼƬ
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
	//�����ɫ������
	for(int nCounti=0;nCounti<strInfo.biClrUsed;nCounti++)
	{
	/*	fwrite(&strPla[nCounti].rgbBlue,1,sizeof(BYTE),fpw);
		fwrite(&strPla[nCounti].rgbGreen,1,sizeof(BYTE),fpw);
		fwrite(&strPla[nCounti].rgbRed,1,sizeof(BYTE),fpw);
		fwrite(&strPla[nCounti].rgbReserved,1,sizeof(BYTE),fpw);*/
		fwrite(&strPla[nCounti], sizeof(RGBQUAD), 1, fpw);
	}
	//������������
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

	//�ͷ��ڴ�
	delete[] imagedata;
	delete[] imagedataRot;

	return true;
}