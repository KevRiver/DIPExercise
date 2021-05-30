#pragma warning(disable:4996)

#include <windows.h>
#include <iostream>
#include <cmath>

typedef struct tagBITMAPHEADER {
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
	RGBQUAD colorTableRGB[256];
}BITMAPHEADER;

BITMAPHEADER bmp;

BYTE* input;
BYTE* output;

BYTE** input2D;
BYTE** output2D;

size_t orgWidth;
size_t orgHeight;
size_t width;
size_t height;

bool ReadBMP(const char* filePath);
bool WriteBMP(const char* filePath, BYTE* data);

void ZoomIn(unsigned int n);

BYTE** MapInto2DArray(BYTE* pixelData, size_t width, size_t height);
BYTE* Serialize(BYTE** data2D, size_t width, size_t height);

void FreeMemory();

int main() {
	ReadBMP("lena.bmp");
	
	ZoomIn(2);

	WriteBMP("output_2.bmp", output);

	FreeMemory();
}

bool ReadBMP(const char* filePath) {
	try {
		FILE* fp = fopen(filePath, "rb");
		if (fp == nullptr) throw std::exception("fail to open file");

		fread(&(bmp.fileHeader), sizeof(BITMAPFILEHEADER), 1, fp);
		fread(&(bmp.infoHeader), sizeof(BITMAPINFOHEADER), 1, fp);
		fread(bmp.colorTableRGB, sizeof(RGBQUAD), 256, fp);
		
		size_t imageSize = bmp.infoHeader.biSizeImage;
		orgWidth = bmp.infoHeader.biWidth;
		orgHeight = bmp.infoHeader.biHeight;
		input = new BYTE[imageSize];
		fread(input, sizeof(BYTE), imageSize, fp);
		
		fclose(fp);
	}
	catch (std::exception& e) {
		std::cout << e.what() << "\n";
		return false;
	}
	return true;
}

bool WriteBMP(const char* filePath, BYTE* data) {
	try {
		FILE* fp = fopen(filePath, "wb");
		if (fp == nullptr) throw std::exception("fail to open file");

		fwrite(&(bmp.fileHeader), sizeof(BITMAPFILEHEADER), 1, fp);
		fwrite(&(bmp.infoHeader), sizeof(BITMAPINFOHEADER), 1, fp);
		fwrite(bmp.colorTableRGB, sizeof(RGBQUAD), 256, fp);
		fwrite(data, sizeof(BYTE), bmp.infoHeader.biSizeImage, fp);
		fclose(fp);
	}
	catch (std::exception& e) {
		std::cout << e.what() << "\n";
		return false;
	}
	return true;
}

BYTE** MapInto2DArray(BYTE* pixelData, size_t width, size_t height) {
	size_t imageSize = width * height;

	BYTE** image2D = new BYTE * [height];
	for (size_t i = 0; i < height; i++) {
		image2D[i] = new BYTE[width];
	}

	for (size_t k = 0; k < imageSize; k++) {
		size_t i = k / width;
		size_t j = k % width;
		image2D[i][j] = pixelData[k];
	}
	
	return image2D;
}

BYTE* Serialize(BYTE** image2D, size_t width, size_t height) {
	size_t imageSize = width * height;

	BYTE* serialized = new BYTE[imageSize];
	memset(serialized, 0, imageSize);

	for (size_t i = 0; i < height; i++) {
		for (size_t j = 0; j < width; j++) {
			serialized[j + i * width] = image2D[i][j];
		}
	}
	return serialized;
}

void ZoomIn(unsigned int nx) {
	if (nx <= 0) return;

	input2D = MapInto2DArray(input, orgWidth, orgHeight);

	width = orgWidth * nx;
	height = orgHeight * nx;

	output2D = new BYTE* [height];
	for (size_t i = 0; i < height; i++) {
		output2D[i] = new BYTE[width];
		memset(output2D[i], 0, width);
	}

	for (size_t i = 0; i < orgHeight; i++) {
		for (size_t j = 0; j < orgWidth; j++) {
			for (size_t k = i * nx; k < (i + 1) * nx; k++) {
				for (size_t w = j * nx; w < (j + 1) * nx; w++) {
					output2D[k][w] = input2D[i][j];
				}
			}
		}
	}

	output = Serialize(output2D, width, height);
	
	bmp.infoHeader.biWidth = width;
	bmp.infoHeader.biHeight = height;
	bmp.infoHeader.biSizeImage = width * height;
}

void FreeMemory() {
	delete[] input;
	delete[] output;
	
	for (size_t i = 0; i < orgHeight; i++) {
		delete[] input2D[i];
	}
	delete[] input2D;

	for (size_t i = 0; i < height; i++) {
		delete[] output2D[i];
	}
	delete[] output2D;
}