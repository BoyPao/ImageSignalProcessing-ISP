//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: FileManager.cpp
// @brief: A file manager to manage image file, provoid file operations
//////////////////////////////////////////////////////////////////////////////////////

#include "FileManager.h"

FileManager::FileManager()
{
	memset(&mInputInfo, 0, sizeof(InputInfo));
	memset(&mOutputInfo, 0, sizeof(OutputInfo));
	mState = Uninited;
}

FileManager::~FileManager()
{
	memset(&mInputInfo, 0, sizeof(InputInfo));
	memset(&mOutputInfo, 0, sizeof(OutputInfo));
	mState = Uninited;
}

ISPResult FileManager::Init()
{
	ISPResult result = ISP_SUCCESS;

	mVTParam = { 0 };

	mState = Inited;

	return result;
}

ISPResult FileManager::DeInit()
{
	ISPResult result = ISP_SUCCESS;

	mVTParam = { 0 };

	mState = Uninited;

	return result;
}

ISPResult FileManager::SetInputInfo(InputInfo info)
{
	ISPResult result = ISP_SUCCESS;

	memcpy(&mInputInfo, &info, sizeof(InputInfo));

	return result;
}

ISPResult FileManager::SetOutputInfo(OutputInfo info)
{
	ISPResult result = ISP_SUCCESS;

	memcpy(&mOutputInfo, &info, sizeof(OutputInfo));

	return result;
}

ISPResult FileManager::SetOutputImgInfo(OutputImgInfo info)
{
	ISPResult result = ISP_SUCCESS;

	memcpy(&mOutputInfo.imgInfo, &info, sizeof(OutputImgInfo));

	return result;
}

ISPResult FileManager::SetOutputVideoInfo(OutputVideoInfo info)
{
	ISPResult result = ISP_SUCCESS;

	memcpy(&mOutputInfo.videoInfo, &info, sizeof(OutputVideoInfo));

	return result;
}

ISPResult FileManager::GetOutputVideoInfo(OutputVideoInfo* pInfo)
{
	ISPResult result = ISP_SUCCESS;

	if (!pInfo) {
		result = ISP_INVALID_PARAM;
		ISPLoge("Invalid input!");
	}

	if (SUCCESS(result)) {
		memcpy(pInfo, &mOutputInfo.videoInfo, sizeof(OutputVideoInfo));
	}

	return result;
}

ISPResult FileManager::ReadData(uint8_t* buffer, int32_t bufferSize)
{
	ISPResult result = ISP_SUCCESS;

	switch(mInputInfo.type) {
		case INPUT_FILE_TYPE_RAW:
			result = ReadRawData(buffer, bufferSize);
			break;
		case INPUT_FILE_TYPE_NUM:
		default:
			result = ISP_FAILED;
			ISPLoge("Not support input type:%d", mInputInfo.type);
			break;
	}
	return result;
}

ISPResult FileManager::ReadRawData(uint8_t* buffer, int32_t bufferSize)
{
	ISPResult result = ISP_SUCCESS;

	if (mState == Uninited) {
		result = ISP_STATE_ERROR;
	}

	if (SUCCESS(result)) {
		ISPLogi("Raw input path:%s", mInputInfo.path);
		ifstream inputFile(mInputInfo.path, ios::in | ios::binary);
		if (inputFile.fail()) {
			result = ISP_FAILED;
			ISPLoge("Open RAW failed! path:%s result:%d", mInputInfo.path, result);
		}

		if (SUCCESS(result)) {
			inputFile.seekg(0, ios::end);
			streampos fileSize = inputFile.tellg();
			mInputInfo.size = (int32_t)fileSize;
			ISPLogd("File size:%d", mInputInfo.size);
			inputFile.seekg(0, ios::beg);

			if (bufferSize >= mInputInfo.size) {
				inputFile.read((char*)buffer, mInputInfo.size);
			}
			else {
				ISPLogw("File size:%d > buffer size:%d! result:%d",
						mInputInfo.size, bufferSize, result);
				inputFile.read((char*)buffer, bufferSize);
			}
		}

		inputFile.close();
	}
	return result;
}

ISPResult FileManager::SaveImgData(uint8_t* srcData)
{
	ISPResult result = ISP_SUCCESS;

	switch(mOutputInfo.imgInfo.type) {
		case OUTPUT_FILE_TYPE_BMP:
			result = SaveBMPData(srcData, mOutputInfo.imgInfo.channels);
			break;
		case OUTPUT_FILE_TYPE_NUM:
		default:
			result = ISP_FAILED;
			ISPLoge("Not support output type:%d", mInputInfo.type);
			break;
	}
	return result;
}

ISPResult FileManager::SaveBMPData(uint8_t* srcData, int32_t channels)
{
	ISPResult result = ISP_SUCCESS;
	if (mState == Uninited) {
		result = ISP_FAILED;
		ISPLoge("ImageFile didnot init");
	}

	if(SUCCESS(result)) {
		mOutputInfo.imgInfo.size = mOutputInfo.imgInfo.width *
			mOutputInfo.imgInfo.hight *
			mOutputInfo.imgInfo.channels;
		BYTE* BMPdata = new BYTE[mOutputInfo.imgInfo.size];
		result = SetBMP(srcData, mOutputInfo.imgInfo.channels, BMPdata);
		if (SUCCESS(result)) {
			WriteBMP(BMPdata, channels);
		}
		else {
			ISPLoge("SetBMP failed. result:&d", result);
		}
		delete[] BMPdata;
	}
	return result;
}

ISPResult FileManager::SetBMP(uint8_t* srcData, int32_t channels, BYTE* dstData)
{
	ISPResult result = ISP_SUCCESS;
	int32_t j = 0;
	BYTE temp;

	if(channels != 3 && channels != 1) {
		result = ISP_INVALID_PARAM;
		ISPLoge("Invalid BMP output channnels:%d", channels);
		//TODO: does it need to be support?
	}

	if (SUCCESS(result)) {
		memcpy(dstData, srcData, mOutputInfo.imgInfo.size);

		/* Convertion for the head & tail of data array */
		while (j < mOutputInfo.imgInfo.size - j) {
			temp = dstData[mOutputInfo.imgInfo.size - j - 1];
			dstData[mOutputInfo.imgInfo.size - j - 1] = dstData[j];
			dstData[j] = temp;
			j++;
		}

		/* mirror flip */
		for (int32_t row = 0; row < mOutputInfo.imgInfo.hight; row++) {
			int32_t col = 0;
			while (col < channels * mOutputInfo.imgInfo.width - col) {
				temp = dstData[row * channels * mOutputInfo.imgInfo.width + channels * mOutputInfo.imgInfo.width - col - 1];
				dstData[channels * row * mOutputInfo.imgInfo.width + channels * mOutputInfo.imgInfo.width - col - 1] =
					dstData[channels * row * mOutputInfo.imgInfo.width + col];
				dstData[channels * row * mOutputInfo.imgInfo.width + col] = temp;
				col++;
			}
		}
	}

	return result;
}

void FileManager::WriteBMP(BYTE* data, int32_t channels)
{
	BITMAPFILEHEADER header;
	BITMAPINFOHEADER headerinfo;
	int32_t tempSize;

	ISPLogd("BYTE:%d WORD:%d DWORD:%d LONG:%d", sizeof(BYTE), sizeof(WORD), sizeof(DWORD), sizeof(LONG));
	memset(&header, 0, sizeof(BITMAPFILEHEADER));
	memset(&headerinfo, 0, sizeof(BITMAPINFOHEADER));
	headerinfo.biSize = sizeof(BITMAPINFOHEADER);
	headerinfo.biHeight = mOutputInfo.imgInfo.hight;
	headerinfo.biWidth = mOutputInfo.imgInfo.width;
	headerinfo.biPlanes = 1;
	headerinfo.biBitCount = (channels == 1) ? 8 : 24;
	headerinfo.biCompression = 0; //BI_RGB
	headerinfo.biSizeImage = mOutputInfo.imgInfo.size;
	headerinfo.biClrUsed = (channels == 1) ? 256 : 0;
	headerinfo.biClrImportant = 0;

	header.bfType = ('M' << 8) + 'B'; // equals to 0x4D42;
	header.bfReserved1 = 0;
	header.bfReserved2 = 0;

	tempSize = (channels == 1) ?
		(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)) :
		(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));

#ifdef LINUX_SYSTEM
	header.bfOffBits[0] = (tempSize % 0x10000) & 0xffff;
	header.bfOffBits[1] = (tempSize / 0x10000) & 0xffff;
	ISPLogd("ch:%d head:%d headinfo:%d bfOffBits:%d(%x %x)",
			channels, sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER),
			tempSize, header.bfOffBits[1], header.bfOffBits[0]);
#elif defined WIN32_SYSTEM
	header.bfOffBits = tempSize;
#endif

	tempSize += headerinfo.biSizeImage;
#ifdef LINUX_SYSTEM
	header.bfSize[0] = (tempSize % 0x10000) & 0xffff;
	header.bfSize[1] = (tempSize / 0x10000) & 0xffff;
	ISPLogd("biSizeImage:%d bfSize:%d(%x %x)", headerinfo.biSizeImage,
			tempSize, header.bfSize[1], header.bfSize[0]);
#elif defined WIN32_SYSTEM
	header.bfSize = tempSize;
#endif

	ISPLogi("BMP output path:%s", mOutputInfo.imgInfo.path);
	ofstream outputFile(mOutputInfo.imgInfo.path, ios::binary);
	if (outputFile.fail()) {
		ISPLoge("Cannot open ouput file:%s", mOutputInfo.imgInfo.path);
	}
	else {
		outputFile.write((char*)& header, sizeof(BITMAPFILEHEADER));
		outputFile.write((char*)& headerinfo, sizeof(BITMAPINFOHEADER));
		if (channels == 1) {
			static RGBQUAD palette[256];
			for (unsigned int i = 0; i < 256; i++)
			{
				palette[i].rgbBlue = i;
				palette[i].rgbGreen = i;
				palette[i].rgbRed = i;
			}
			outputFile.write((char*)palette, sizeof(RGBQUAD) * 256);
		}
		outputFile.write((char*)data, mOutputInfo.imgInfo.size);
	}
	outputFile.close();
}

ISPResult FileManager::CreateVideo(void* dst)
{
	ISPResult result = ISP_SUCCESS;

	if (!mVideo) {
		mVideo = std::make_unique<ISPVideo>();
	}

	result = mVideo->Init(dst);
	if (SUCCESS(result)) {
		mVTParam.pVideo = mVideo.get();
		mVTParam.pFileMgr = this;
		result = mVideo->CreateThread((void*)&mVTParam);
	}
	ISPLogi("Video fps:%d total frame:%d",
			mOutputInfo.videoInfo.fps, mOutputInfo.videoInfo.frameNum);

	return result;
}

ISPResult FileManager::SaveVideoData(int32_t frameCount)
{
	ISPResult result = ISP_SUCCESS;

	result = mVideo->Notify();
	ISPLogd("Notify F:%d", frameCount);

	return result;
}


ISPResult FileManager::DestroyVideo()
{
	ISPResult result = ISP_SUCCESS;

	result = mVideo->DestroyThread();

	return result;
}

ISPResult FileManager::Mipi10decode(void* src, void* dst, IMG_INFO* info)
{
	ISPResult result = ISP_SUCCESS;

	int32_t leftShift = 0;
	int32_t alignedW = ALIGNx(info->width, info->bitspp, CHECK_PACKAGED(info->rawFormat), info->stride);
	if (!info) {
		result = ISP_INVALID_PARAM;
	}

	if (SUCCESS(result)) {
		leftShift = info->bitspp - BITS_PER_WORD;
		if (leftShift < 0 || leftShift > 8)
		{
			result = ISP_INVALID_PARAM;
		}
	}

	if (SUCCESS(result)) {
		switch (info->rawFormat) {
			case ANDROID_RAW10:
				for (int32_t row = 0; row < info->height; row++) {
					for (int32_t col = 0; col < alignedW; col += 5) {
						if (col * BITS_PER_WORD / info->bitspp < info->width && row < info->height) {
							static_cast<uint16_t*>(dst)[row * info->width + col * BITS_PER_WORD / info->bitspp] =
								((static_cast<uint8_t*>(src)[row * alignedW + col] & 0xffff) << leftShift) |
								((static_cast<uint8_t*>(src)[row * alignedW + col + 4] & 0x3) & 0x3ff);
							static_cast<uint16_t*>(dst)[row * info->width + col * BITS_PER_WORD / info->bitspp + 1] =
								((static_cast<uint8_t*>(src)[row * alignedW + col + 1] & 0xffff) << leftShift) |
								(((static_cast<uint8_t*>(src)[row * alignedW + col + 4] >> 2) & 0x3) & 0x3ff);
							static_cast<uint16_t*>(dst)[row * info->width + col * BITS_PER_WORD / info->bitspp + 2] =
								((static_cast<uint8_t*>(src)[row * alignedW + col + 2] & 0xffff) << leftShift) |
								(((static_cast<uint8_t*>(src)[row * alignedW + col + 4] >> 4) & 0x3) & 0x3ff);
							static_cast<uint16_t*>(dst)[row * info->width + col * BITS_PER_WORD / info->bitspp + 3] =
								((static_cast<uint8_t*>(src)[row * alignedW + col + 3] & 0xffff) << leftShift) |
								(((static_cast<uint8_t*>(src)[row * alignedW + col + 4] >> 6) & 0x3) & 0x3ff);
						}
					}
				}
				break;
			case ORDINAL_RAW10:
				for (int32_t row = 0; row < info->height; row++) {
					for (int32_t col = 0; col < alignedW; col += 5) {
						if (col * BITS_PER_WORD / info->bitspp < info->width && row < info->height) {
							static_cast<uint16_t*>(dst)[row * info->width + col * BITS_PER_WORD / info->bitspp] =
								(((static_cast<uint8_t*>(src)[row * alignedW + col + 0] >> 0) & (0xff >> 0)) & 0xffff) |
								(((static_cast<uint8_t*>(src)[row * alignedW + col + 1] & 0x03) << 8) & 0xffff);
							static_cast<uint16_t*>(dst)[row * info->width + col * BITS_PER_WORD / info->bitspp + 1] =
								(((static_cast<uint8_t*>(src)[row * alignedW + col + 1] >> 2) & (0xff >> 2)) & 0xffff) |
								(((static_cast<uint8_t*>(src)[row * alignedW + col + 2] & 0x0f) << 6) & 0xffff);
							static_cast<uint16_t*>(dst)[row * info->width + col * BITS_PER_WORD / info->bitspp + 2] =
								(((static_cast<uint8_t*>(src)[row * alignedW + col + 2] >> 4) & (0xff >> 4)) & 0xffff) |
								(((static_cast<uint8_t*>(src)[row * alignedW + col + 3] & 0x3f) << 4) & 0xffff);
							static_cast<uint16_t*>(dst)[row * info->width + col * BITS_PER_WORD / info->bitspp + 3] =
								(((static_cast<uint8_t*>(src)[row * alignedW + col + 3] >> 6) & (0xff >> 6)) & 0xffff) |
								(((static_cast<uint8_t*>(src)[row * alignedW + col + 4] & 0xff) << 2) & 0xffff);
						}
					}
				}
				break;
			case UNPACKAGED_RAW10_LSB:
				for (int32_t row = 0; row < info->height; row++) {
					for (int32_t col = 0; col < alignedW; col += 2) {
						static_cast<uint16_t*>(dst)[row * info->width + col / 2] =
							(static_cast<uint8_t*>(src)[row * alignedW + col] & 0xffff) |
							(((static_cast<uint8_t*>(src)[row * alignedW + col + 1] & 0x3) & 0xffff) << BITS_PER_WORD);
					}
				}
			case UNPACKAGED_RAW10_MSB:
				for (int32_t row = 0; row < info->height; row++) {
					for (int32_t col = 0; col < alignedW; col += 2) {
						static_cast<uint16_t*>(dst)[row * info->width + col / 2] =
							((static_cast<uint8_t*>(src)[row * alignedW + col] >> (BITS_PER_WORD - leftShift)) & 0xffff) |
							((static_cast<uint8_t*>(src)[row * alignedW + col + 1] & 0xffff) << leftShift);
					}
				}
				break;
			case RAW_FORMAT_NUM:
			default:
				ISPLoge("Not support raw type:%d", info->rawFormat);
				break;
		}
		ISPLogi("finished");
	}

	return result;
}

void DumpImgDataAsText(void* data, int32_t height, int32_t width, size_t bitWidth, char* dumpPath)
{
	if (data != nullptr) {
		if (dumpPath) {
			ofstream OutFile(dumpPath);
			for (int32_t i = 0; i < height; i++) {
				OutFile << i << ": ";
				for (int32_t j = 0; j < width; j++) {
					switch (bitWidth) {
						case sizeof(uint16_t) :
							OutFile << (int)static_cast<uint16_t*>(data)[i * width + j] << ' ';
							break;
							case sizeof(uint8_t) :
								OutFile << (int)static_cast<uint8_t*>(data)[i * width + j] << ' ';
								break;
							default:
								ISPLoge("Dump failed. Unsopported data type");
					}
				}
				OutFile << endl;
			}
			OutFile.close();
			ISPLoge("Data saved as TXT finished");
		}
		else {
			ISPLoge("dumpPath is null");
		}
	}
	else {
		ISPLoge("Dump failed. data is nullptr");
	}
}
