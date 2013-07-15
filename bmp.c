// vc80: disable deprecated functions warning
#pragma warning( disable: 4996 )

#include <stdio.h>
#include <stdlib.h>

// для удобства введем пару новых типов (чтобы не писать длинных имен)
typedef unsigned long       dword;
typedef unsigned short      word;

//-------------------------------------------------------------------------------------------------
// структура BMP файла такова:
//  - структура BITMAPFILEHEADER
//  - структура BITMAPINFOHEADER
//  - данные
// поэтому заведем нужные структуры
//-------------------------------------------------------------------------------------------------

#include <pshpack2.h>			// отключаем автоматическое выравнивание полей структуры
// копия структуры BITMAPFILEHEADER
typedef struct {
	word	bfType;				// тип файла
	dword	bfSize;				// размер файла в байтах
	word	bfReserved1;		// зарезервирован, должен быть 0
	word	bfReserved2;		// зарезервирован, должен быть 0
	dword	bfOffBits;			// указывает смещение в байтах, от начала этой структуры, до массива пикселей
} bmpfh_t;
#include <poppack.h>			// восстанавливаем автоматическое выравнивание полей структуры

// копия структуры BITMAPINFOHEADER
typedef struct {
	dword	biSize;				// размер структуры в байтах
	long	biWidth;			// ширина битмапки в пикселях
	long	biHeight;			// высота битмапки в пикселях, если положительна, то строки идут снизу-вверх
								//	если отрицательна, то направление строк сверху-вниз
	word	biPlanes;			// число слоев, должен быть равен 1
	word	biBitCount;			// число бит на пиксель (поддерживается только 24 и 32 бита)
	dword	biCompression;		// указывает тип компрессии, должен быть 0
	dword	biSizeImage;		// указывает размер изображения в байтах, может быть 0 для e_RGB картинок
	long	biXPelsPerMeter;	// разрешение по горизонтали для целевого устройства
	long	biYPelsPerMeter;	// разрешение по вертикали для целевого устройства
	dword	biClrUsed;			// число цветов в color table которые задействованы для отображения картинки
	dword	biClrImportant;		// число цветов необходимых для отображения картинки
} bmpih_t;

//-------------------------------------------------------------------------------------------------
// чтение файла
//-------------------------------------------------------------------------------------------------

char* read_bmp(const char* filename, int *width, int *height, int *components)
{
	FILE*	file = 0;	// файл
	bmpfh_t bfh = {0};	// BITMAPFILEHEADER
	bmpih_t bih = {0};	// BITMAPINFOHEADER
	int		lPixelSize, // размер пикселя в байтах
			lRowSize,	// размер строки в байтах
			lSize;		// размер массива пикселей в байтах
	char*	res = 0;	// результирующий массив байт
	int		i;			// для цикла
	
	// открываем файл на чтение
	file = fopen(filename, "rb");
	if ( file == 0 ) return(0);

	// грузим заголовок и проверяем валидность файла
	if ( fread(&bfh, sizeof(bmpfh_t), 1, file) != 1 ) return(0);
	// сигнатура файла - 'BM', но в x86 слово находится в перевернутом состоянии, поэтому сравниваем с 'MB'
	if ( bfh.bfType != 'MB' ) return(0);
	
	// грузим инфу о картинке и проверяем ее правильность
	if ( fread(&bih, sizeof(bmpih_t), 1, file) != 1 ) return(0);
	if ( bih.biPlanes != 1 ) return(0);			// число слоев должно быть равно 1
	if ( bih.biBitCount < 24 ) return(0);		// грузим только RGB и RGBA формат
	if ( bih.biCompression != 0 ) return(0);	// RLE компрессии быть не должно

	//-------------------------------------------------------------------
	// заполняем структуру картинки

	// сначала указываем тип картинки (глубину цвета)
	if (components) *components = bih.biBitCount / 8;
	// теперь размер картинки
	if (width) *width = bih.biWidth;
	if (height) *height = bih.biHeight < 0 ? -bih.biHeight : bih.biHeight;

	// а теперь собственно выделяем память под массив пикселей
	lPixelSize = (*components);
	lRowSize = lPixelSize * (*width);
	lSize = lRowSize * (*height);
	res = (char*)malloc(lSize);

	// читаем пиксели картинки из файла
	// в случае, если biHeight отрицательно, то строки идут сверху вниз, учтем это
	if ( bih.biHeight < 0 )
		for (i = 0; i < (*height); i++) fread(&res[i * lRowSize], lPixelSize, *width, file);
	else
		for (i = (*height) - 1; i >= 0; i--) fread(&res[i * lRowSize], lPixelSize, *width, file);

	fclose(file);		// закрываем файл
	return(res);		// и возвращаем результат
}