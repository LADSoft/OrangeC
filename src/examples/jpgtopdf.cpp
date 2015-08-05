#include <iostream>
#include <cstdio>
#include <string>
#include <vector>

using namespace std;

typedef struct _SIZE_
{
	unsigned int Width;
	unsigned int Height;
	unsigned long Length;
} size;

typedef struct _JPEG_IMAGE_
{
	size *imageSize;
	int dpi;
	const char *imageName;
} jpeg_image;

bool getImageSize(jpeg_image *jpgImage)
{
	FILE *fp;

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
	fopen_s(&fp, jpgImage->imageName, "rb");
#else
	fp = fopen(jpgImage->imageName, "rb");
#endif
	if (fp == NULL)
	{
		cout << "Fail to open image file.." << endl;
		return false;
	}
	fseek(fp, 0, SEEK_END);
	long len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (len < 24)
	{
		fclose(fp);
		return false;
	}
	jpgImage->imageSize->Length = len;

	unsigned char buf[24];
	fread(buf, 1, 24, fp);

	if (buf[0] == 0xFF && buf[1] == 0xD8 && buf[2] == 0xFF && buf[3] == 0xE0 &&
		buf[6] == 'J' && buf[7] == 'F' && buf[8] == 'I' && buf[9] == 'F')
	{
		long pos = 2;
		while (buf[2] == 0xFF)
		{
			if (buf[3] == 0xC0 || buf[3] == 0xC1 || buf[3] == 0xC2 || buf[3] == 0xC3 ||
				buf[3] == 0xC9 || buf[3] == 0xCA || buf[3] == 0xCB)
				break;
			pos += 2 + (buf[4] << 8) + buf[5];
			if (pos + 12 > len)
				break;
			fseek(fp, pos, SEEK_SET);
			fread(buf + 2, 1, 12, fp);
		}
	}

	if (buf[0] == 0xFF && buf[1] == 0xD8 && buf[2] == 0xFF)
	{
		jpgImage->imageSize->Height = (buf[7] << 8) + buf[8];
		jpgImage->imageSize->Width = (buf[9] << 8) + buf[10];
		return true;
	}
	return false;
}

size a4PageSize(int dpi)
{
	size sz;
	sz.Height = (int)(dpi * 8.2677);
	sz.Width = (int)(dpi * 11.6929);
	return sz;
}

void addPosInVect(FILE *fp, vector<fpos_t> *positionVector)
{
	fpos_t postion;
	fgetpos(fp, &postion);
	positionVector->push_back(postion);
}

string intToString(int val)
{
	char *buf = (char*)malloc(sizeof(char) * 48);
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
	_itoa_s(val, buf, 48, 10);
#else
	itoa(val, buf, 10);
#endif
	string ret(buf);
	free(buf);
	return ret;
}

void makePdf(string pdfName, jpeg_image *jpgImage)
{
	int numberOfImages = 1;
	int no_j = 3 + numberOfImages * 2;
	string devs[] = { "", "DeviceGray", "", "DeviceRGB", "DeviceCMYK" };
	size a4Paper = a4PageSize(jpgImage->dpi);

	vector<fpos_t> objp;
	FILE *fp;
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
	fopen_s(&fp, pdfName.c_str(), "wb");
#else
	fp = fopen(pdfName.c_str(), "wb");
#endif
	fprintf(fp, "%%PDF-1.2\r\n");
	fprintf(fp, "\r\n");
	addPosInVect(fp, &objp);

	fprintf(fp, "1 0 obj\r\n");
	fprintf(fp, "<<\r\n");
	fprintf(fp, "  /Type /Catalog /Pages 2 0 R\r\n");
	fprintf(fp, ">>\r\n");
	fprintf(fp, "endobj\r\n");
	fprintf(fp, "\r\n");
	addPosInVect(fp, &objp);

	fprintf(fp, "2 0 obj\r\n");
	fprintf(fp, "<<\r\n");
	fprintf(fp, "  /Type /Pages /Count %d\r\n", numberOfImages);
	fprintf(fp, "  /Kids\r\n");
	fprintf(fp, "  [");
	for (int i = 0; i < numberOfImages; i++)
	{
		if ((i & 7) == 0)
		{
			fprintf(fp, "\r\n");
			fprintf(fp, "   ");
		}
		fprintf(fp, " %d 0 R", 3 + i * 2);
	}
	fprintf(fp, "\r\n");
	fprintf(fp, "  ]\r\n");
	fprintf(fp, ">>\r\n");
	fprintf(fp, "endobj\r\n");

	for (int i = 0; i < numberOfImages; i++)
	{
		int pp = 20 + i * 10 / numberOfImages;
		int no_p = 3 + i * 2;
		int no_c = no_p + 1;
		string name = "/Jpeg" + intToString(i + 1);
		size *sz = jpgImage->imageSize;

		fprintf(fp, "\r\n");
		addPosInVect(fp, &objp);
		fprintf(fp, "%d 0 obj\r\n", no_p);
		fprintf(fp, "<<\r\n");
		fprintf(fp, "  /Type /Page /Parent 2 0 R /Contents %d 0 R\r\n", no_c);
		fprintf(fp, "  /MediaBox [ 0 0 %d %d ]\r\n", 2480, 3508 /*sz->Width, sz->Height*/);

		//if (sz->Width > sz->Height)
		//	fprintf(fp, "  /Rotate 90\r\n");

		fprintf(fp, "  /Resources\r\n");
		fprintf(fp, "  <<\r\n");
		fprintf(fp, "    /ProcSet [ /PDF /ImageB /ImageC /ImageI ]\r\n");
		fprintf(fp, "    /XObject << %s %d 0 R >>\r\n", name.c_str(), no_j + i);
		fprintf(fp, "  >>\r\n");
		fprintf(fp, ">>\r\n");
		fprintf(fp, "endobj\r\n");

		fprintf(fp, "\r\n");
		addPosInVect(fp, &objp);
		fprintf(fp, "%d 0 obj\r\n", no_c);
		char st4[100];
		int size;
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
		size = sprintf_s(st4, sizeof(st4), "q %d 0 0 %d 0 0 cm %s Do Q\r\n", sz->Width, sz->Height, name.c_str());
#else
		size = sprintf(st4, "q %d 0 0 %d 0 0 cm %s Do Q\r\n", sz->Width, sz->Height, name.c_str());
#endif
		fprintf(fp, "<< /Length %d >>\r\n", size - 2);
		fprintf(fp, "stream\r\n");
		fprintf(fp, st4);
		fprintf(fp, "endstream\r\n");
		fprintf(fp, "endobj\r\n");
	}

	for (int i = 0; i < numberOfImages; i++)
	{
		string name = "/Jpeg" + intToString(i + 1);
		size *sz = jpgImage->imageSize;

		fprintf(fp, "\r\n");
		addPosInVect(fp, &objp);
		fprintf(fp, "%d 0 obj\r\n", no_j + i);
		fprintf(fp, "<<\r\n");
		fprintf(fp, "  /Type /XObject /Subtype /Image /Name %s\r\n", name.c_str());
		fprintf(fp, "  /Filter /DCTDecode /BitsPerComponent 8 /ColorSpace /%s\r\n", devs[3].c_str());
		fprintf(fp, "  /Width %d /Height %d /Length %d\r\n", sz->Width, sz->Height, sz->Length);
		fprintf(fp, ">>\r\n");
		fprintf(fp, "stream\r\n");

		FILE *jpgImgFile;
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
		fopen_s(&jpgImgFile, jpgImage->imageName, "rb");
#else
		jpgImgFile = fopen(jpgImage->imageName, "rb");
#endif
		if (jpgImgFile == NULL)
		{
			printf("Fail to load the image %s\r\n", jpgImage->imageName);
			return;
		}

		int len = 0;
		char *buffer = (char*)malloc(4096);
		while ((len = fread(buffer, sizeof(char), 4096, jpgImgFile)) > 0)
		{
			fwrite(buffer, sizeof(char), len, fp);
		}

		free(buffer);
		fclose(jpgImgFile);

		fprintf(fp, "\r\n");
		fprintf(fp, "endstream\r\n");
		fprintf(fp, "endobj\r\n");
	}

	fprintf(fp, "\r\n");
	fpos_t xref;
	fgetpos(fp, &xref);

	fprintf(fp, "xref\r\n");
	int size = objp.size() + 1;
	fprintf(fp, "0 %d\r\n", size);
	fprintf(fp, "%010d %05d f\r\n", 0, 65535);
	for(int k=0;k<objp.size();k++)
	{
		fpos_t p = objp[k];
		fprintf(fp, "%010d %05d n\r\n", p, 0);
	}
	fprintf(fp, "trailer\r\n");
	fprintf(fp, "<< /Root 1 0 R /Size %d >>\r\n", size);
	fprintf(fp, "startxref\r\n");
	fprintf(fp, "%d\r\n", xref);
	fprintf(fp, "%%%%EOF\r\n");

	fclose(fp);
}

int main(int argc, char **argv)
{
	if (argc != 4)
	{
		cout << "Modo de usar:" << endl;
		cout << "pdf2jpg <input file>.jpg <output file>.pdf <dpi>" << endl;
		return -1;
	}

	size *imgSize = (size*)malloc(sizeof(size));
	jpeg_image *jpgImage = (jpeg_image*)malloc(sizeof(jpeg_image));
	jpgImage->imageSize = imgSize;
	jpgImage->imageName = argv[1];
	jpgImage->dpi = atoi(argv[3]);
	getImageSize(jpgImage);

	makePdf(argv[2], jpgImage);

	free(jpgImage->imageSize);
	free(jpgImage);

	return 0;
}