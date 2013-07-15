// vc80: disable deprecated functions warning
#pragma warning( disable: 4996 )

#include <stdio.h>
#include <stdlib.h>

// ��� �������� ������ ���� ����� ����� (����� �� ������ ������� ����)
typedef unsigned long       dword;
typedef unsigned short      word;

//-------------------------------------------------------------------------------------------------
// ��������� BMP ����� ������:
//  - ��������� BITMAPFILEHEADER
//  - ��������� BITMAPINFOHEADER
//  - ������
// ������� ������� ������ ���������
//-------------------------------------------------------------------------------------------------

#include <pshpack2.h>			// ��������� �������������� ������������ ����� ���������
// ����� ��������� BITMAPFILEHEADER
typedef struct {
	word	bfType;				// ��� �����
	dword	bfSize;				// ������ ����� � ������
	word	bfReserved1;		// ��������������, ������ ���� 0
	word	bfReserved2;		// ��������������, ������ ���� 0
	dword	bfOffBits;			// ��������� �������� � ������, �� ������ ���� ���������, �� ������� ��������
} bmpfh_t;
#include <poppack.h>			// ��������������� �������������� ������������ ����� ���������

// ����� ��������� BITMAPINFOHEADER
typedef struct {
	dword	biSize;				// ������ ��������� � ������
	long	biWidth;			// ������ �������� � ��������
	long	biHeight;			// ������ �������� � ��������, ���� ������������, �� ������ ���� �����-�����
								//	���� ������������, �� ����������� ����� ������-����
	word	biPlanes;			// ����� �����, ������ ���� ����� 1
	word	biBitCount;			// ����� ��� �� ������� (�������������� ������ 24 � 32 ����)
	dword	biCompression;		// ��������� ��� ����������, ������ ���� 0
	dword	biSizeImage;		// ��������� ������ ����������� � ������, ����� ���� 0 ��� e_RGB ��������
	long	biXPelsPerMeter;	// ���������� �� ����������� ��� �������� ����������
	long	biYPelsPerMeter;	// ���������� �� ��������� ��� �������� ����������
	dword	biClrUsed;			// ����� ������ � color table ������� ������������� ��� ����������� ��������
	dword	biClrImportant;		// ����� ������ ����������� ��� ����������� ��������
} bmpih_t;

//-------------------------------------------------------------------------------------------------
// ������ �����
//-------------------------------------------------------------------------------------------------

char* read_bmp(const char* filename, int *width, int *height, int *components)
{
	FILE*	file = 0;	// ����
	bmpfh_t bfh = {0};	// BITMAPFILEHEADER
	bmpih_t bih = {0};	// BITMAPINFOHEADER
	int		lPixelSize, // ������ ������� � ������
			lRowSize,	// ������ ������ � ������
			lSize;		// ������ ������� �������� � ������
	char*	res = 0;	// �������������� ������ ����
	int		i;			// ��� �����
	
	// ��������� ���� �� ������
	file = fopen(filename, "rb");
	if ( file == 0 ) return(0);

	// ������ ��������� � ��������� ���������� �����
	if ( fread(&bfh, sizeof(bmpfh_t), 1, file) != 1 ) return(0);
	// ��������� ����� - 'BM', �� � x86 ����� ��������� � ������������ ���������, ������� ���������� � 'MB'
	if ( bfh.bfType != 'MB' ) return(0);
	
	// ������ ���� � �������� � ��������� �� ������������
	if ( fread(&bih, sizeof(bmpih_t), 1, file) != 1 ) return(0);
	if ( bih.biPlanes != 1 ) return(0);			// ����� ����� ������ ���� ����� 1
	if ( bih.biBitCount < 24 ) return(0);		// ������ ������ RGB � RGBA ������
	if ( bih.biCompression != 0 ) return(0);	// RLE ���������� ���� �� ������

	//-------------------------------------------------------------------
	// ��������� ��������� ��������

	// ������� ��������� ��� �������� (������� �����)
	if (components) *components = bih.biBitCount / 8;
	// ������ ������ ��������
	if (width) *width = bih.biWidth;
	if (height) *height = bih.biHeight < 0 ? -bih.biHeight : bih.biHeight;

	// � ������ ���������� �������� ������ ��� ������ ��������
	lPixelSize = (*components);
	lRowSize = lPixelSize * (*width);
	lSize = lRowSize * (*height);
	res = (char*)malloc(lSize);

	// ������ ������� �������� �� �����
	// � ������, ���� biHeight ������������, �� ������ ���� ������ ����, ����� ���
	if ( bih.biHeight < 0 )
		for (i = 0; i < (*height); i++) fread(&res[i * lRowSize], lPixelSize, *width, file);
	else
		for (i = (*height) - 1; i >= 0; i--) fread(&res[i * lRowSize], lPixelSize, *width, file);

	fclose(file);		// ��������� ����
	return(res);		// � ���������� ���������
}