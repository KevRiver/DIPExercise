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

bool ReadBMP(const char* filePath);
bool WriteBMP(const char* filePath, BYTE* data);

void ZoomOut(unsigned int n);

BYTE** MapInto2DArray(BYTE* pixelData);
BYTE* Serialize(BYTE** data2D);

void FreeMemory();

int main() {
	ReadBMP("lena.bmp");
	
	ZoomOut(2);
	
	WriteBMP("output_1.bmp", output);

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

BYTE** MapInto2DArray(BYTE* pixelData) {
	size_t width = (size_t)abs(bmp.infoHeader.biWidth);
	size_t height = (size_t)abs(bmp.infoHeader.biHeight);
	size_t imageSize = bmp.infoHeader.biSizeImage;

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

BYTE* Serialize(BYTE** image2D) {
	size_t width = (size_t)abs(bmp.infoHeader.biWidth);
	size_t height = (size_t)abs(bmp.infoHeader.biHeight);
	size_t imageSize = bmp.infoHeader.biSizeImage;

	BYTE* serialized = new BYTE[imageSize];
	memset(serialized, 0, imageSize);

	for (size_t i = 0; i < height; i++) {
		for (size_t j = 0; j < width; j++) {
			serialized[j + i * width] = image2D[i][j];
		}
	}
	return serialized;
}

void ZoomOut(unsigned int n) {
	size_t width = (size_t)abs(bmp.infoHeader.biWidth);
	size_t height = (size_t)abs(bmp.infoHeader.biHeight);

	input2D = MapInto2DArray(input);
	
	output2D = new BYTE * [height];
	for (size_t i = 0; i < height; i++) {
		output2D[i] = new BYTE[width];
		memset(output2D[i], 0, width);
	}

	for (size_t i = 0; i < height; i += n) {
		for (size_t j = 0; j < width; j += n) {
			size_t y = i / n;
			size_t x = j / n;
			output2D[y][x] = input2D[i][j];
		}
	}

	output = Serialize(output2D);
}

void ApplyFilter() {
}

void FreeMemory() {
	delete[] input;
	delete[] output;
	
	size_t height = (size_t)abs(bmp.infoHeader.biHeight);
	for (size_t i = 0; i < height; i++) {
		delete[] input2D[i];
	}
	delete[] input2D;

	for (size_t i = 0; i < height; i++) {
		delete[] output2D[i];
	}
	delete[] output2D;
}