// License: GPL-3.0-or-later
/*
 * Defined a file manager to manage files, provoid file operations.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "FileManager.h"
#include <linux/videodev2.h>

#define MIN_IO_PARAM_CNT 4

struct ImgFmtInfo {
	uint32_t fmt;
	uint32_t bitWidth;
	uint32_t cspace;
	uint32_t order;
	uint32_t dpt;
	char info[20];
};

const ImgFmtInfo gSupportedFmt[] = {
	{V4L2_PIX_FMT_SBGGR8,	8,	CS_Bayer,	BO_BGGR,	DPT_NUM,		"Raw8-bggr"				},
	{V4L2_PIX_FMT_SGBRG8,	8,	CS_Bayer,	BO_GBRG,	DPT_NUM,		"Raw8-gbrg"				},
	{V4L2_PIX_FMT_SGRBG8,	8,	CS_Bayer,	BO_GRBG,	DPT_NUM,		"Raw8-grbg"				},
	{V4L2_PIX_FMT_SRGGB8,	8,	CS_Bayer,	BO_RGGB,	DPT_NUM,		"Raw8-rggb"				},
	{V4L2_PIX_FMT_SBGGR10,	10, CS_Bayer,	BO_BGGR,	DPT_Unpackaged, "Raw10-bggr"		   	},
	{V4L2_PIX_FMT_SGBRG10,	10, CS_Bayer,	BO_GBRG,	DPT_Unpackaged, "Raw10-gbrg"		   	},
	{V4L2_PIX_FMT_SGRBG10,	10, CS_Bayer,	BO_GRBG,	DPT_Unpackaged, "Raw10-grbg"		   	},
	{V4L2_PIX_FMT_SRGGB10,	10, CS_Bayer,	BO_RGGB,	DPT_Unpackaged, "Raw10-rggb"		   	},
	{V4L2_PIX_FMT_SBGGR10P, 10, CS_Bayer,	BO_BGGR,	DPT_Packaged,	"Raw10-bggr-packed"  	},
	{V4L2_PIX_FMT_SGBRG10P, 10, CS_Bayer,	BO_GBRG,	DPT_Packaged,	"Raw10-gbrg-packed"  	},
	{V4L2_PIX_FMT_SGRBG10P, 10, CS_Bayer,	BO_GRBG,	DPT_Packaged,	"Raw10-grbg-packed"  	},
	{V4L2_PIX_FMT_SRGGB10P, 10, CS_Bayer,	BO_RGGB,	DPT_Packaged,	"Raw10-rggb-packed"  	},
	{V4L2_PIX_FMT_SBGGR12P, 12, CS_Bayer,	BO_BGGR,	DPT_Packaged,	"Raw12-bggr-packed"  	},
	{V4L2_PIX_FMT_SGBRG12P, 12, CS_Bayer,	BO_GBRG,	DPT_Packaged,	"Raw12-gbrg-packed"  	},
	{V4L2_PIX_FMT_SGRBG12P, 12, CS_Bayer,	BO_GRBG,	DPT_Packaged,	"Raw12-grbg-packed"  	},
	{V4L2_PIX_FMT_SRGGB12P, 12, CS_Bayer,	BO_RGGB,	DPT_Packaged,	"Raw12-rggb-packed"  	},
};

FileManager::FileManager()
{
	memset(&mInputInfo, 0, sizeof(InputInfo));
	memset(&mOutputInfo, 0, sizeof(OutputInfo));
	IOInfoFlag = 0;

	mState = Uninited;
}

FileManager::~FileManager()
{
	memset(&mInputInfo, 0, sizeof(InputInfo));
	memset(&mOutputInfo, 0, sizeof(OutputInfo));

	mState = Uninited;
}

int32_t FileManager::Init()
{
	int32_t rt = ISP_SUCCESS;

	mVTParam = { 0 };
	if(!pDynamicInfo) {
		rt = ISP_STATE_ERROR;
		ILOGE("IO info buffer is null %d", rt);
	} else {
		memset(pDynamicInfo, 0, sizeof(MediaInfo));
	}

	mState = Inited;

	return rt;
}

int32_t FileManager::DeInit()
{
	int32_t rt = ISP_SUCCESS;

	mVTParam = { 0 };
	if(!pDynamicInfo) {
		rt = ISP_STATE_ERROR;
		ILOGE("IO info buffer is null %d", rt);
	} else {
		memset(pDynamicInfo, 0, sizeof(MediaInfo));
	}

	mState = Uninited;

	return rt;
}

int32_t FileManager::SetInputInfo(InputInfo info)
{
	int32_t rt = ISP_SUCCESS;

	mInputInfo.type = info.type;
	mInputInfo.size = info.size;
	if (IOInfoFlag) {
		strcat(mInputInfo.path, INPUT_PATH);
		strcat(mInputInfo.path, INPUT_NAME);
	}

	return rt;
}

int32_t FileManager::SetOutputInfo(OutputInfo info)
{
	int32_t rt = ISP_SUCCESS;

	mOutputInfo.imgInfo.type = info.imgInfo.type;
	mOutputInfo.imgInfo.size = info.imgInfo.size;
	mOutputInfo.imgInfo.width = info.imgInfo.width;
	mOutputInfo.imgInfo.height = info.imgInfo.height;
	mOutputInfo.imgInfo.channels = info.imgInfo.channels;
	mOutputInfo.videoInfo.type = info.videoInfo.type;
	mOutputInfo.videoInfo.width = info.videoInfo.width;
	mOutputInfo.videoInfo.height = info.videoInfo.height;
	mOutputInfo.videoInfo.fps = info.videoInfo.fps;
	mOutputInfo.videoInfo.frameNum = info.videoInfo.frameNum;
	if (IOInfoFlag) {
		strcat(mOutputInfo.imgInfo.path, OUTPUT_PATH);
		strcat(mOutputInfo.imgInfo.path, OUTPUT_IMG_NAME);
		strcat(mOutputInfo.videoInfo.path, OUTPUT_PATH);
		strcat(mOutputInfo.videoInfo.path, OUTPUT_VIDEO_NAME);
	}

	return rt;
}

int32_t FileManager::SetOutputImgInfo(OutputImgInfo info)
{
	int32_t rt = ISP_SUCCESS;

	memcpy(&mOutputInfo.imgInfo, &info, sizeof(OutputImgInfo));

	return rt;
}

int32_t FileManager::SetOutputVideoInfo(OutputVideoInfo info)
{
	int32_t rt = ISP_SUCCESS;

	memcpy(&mOutputInfo.videoInfo, &info, sizeof(OutputVideoInfo));

	return rt;
}

int32_t FileManager::GetOutputVideoInfo(OutputVideoInfo* pInfo)
{
	int32_t rt = ISP_SUCCESS;

	if (!pInfo) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invalid input!");
	}

	if (SUCCESS(rt)) {
		memcpy(pInfo, &mOutputInfo.videoInfo, sizeof(OutputVideoInfo));
	}

	return rt;
}

int32_t FileManager::ReadData(uint8_t* buffer, int32_t bufferSize)
{
	int32_t rt = ISP_SUCCESS;

	switch(mInputInfo.type) {
		case INPUT_FILE_TYPE_RAW:
			rt = ReadRawData(buffer, bufferSize);
			break;
		case INPUT_FILE_TYPE_NUM:
		default:
			rt = ISP_FAILED;
			ILOGE("Not support input type:%d", mInputInfo.type);
			break;
	}
	return rt;
}

int32_t FileManager::ReadRawData(uint8_t* buffer, int32_t bufferSize)
{
	int32_t rt = ISP_SUCCESS;

	if (mState == Uninited) {
		rt = ISP_STATE_ERROR;
	}

	if (SUCCESS(rt)) {
		ILOGI("Raw input path:%s", mInputInfo.path);
		ifstream inputFile(mInputInfo.path, ios::in | ios::binary);
		if (inputFile.fail()) {
			rt = ISP_FAILED;
			ILOGE("Open RAW failed! path:%s rt:%d", mInputInfo.path, rt);
		}

		if (SUCCESS(rt)) {
			inputFile.seekg(0, ios::end);
			streampos fileSize = inputFile.tellg();
			mInputInfo.size = (int32_t)fileSize;
			ILOGDF("File size:%d", mInputInfo.size);
			inputFile.seekg(0, ios::beg);

			if (bufferSize >= mInputInfo.size) {
				inputFile.read((char*)buffer, mInputInfo.size);
			}
			else {
				ILOGW("File size:%d > buffer size:%d! rt:%d",
						mInputInfo.size, bufferSize, rt);
				inputFile.read((char*)buffer, bufferSize);
			}
		}

		inputFile.close();
	}
	return rt;
}

int32_t FileManager::SaveImgData(uint8_t* srcData)
{
	int32_t rt = ISP_SUCCESS;

	switch(mOutputInfo.imgInfo.type) {
		case OUTPUT_FILE_TYPE_BMP:
			rt = SaveBMPData(srcData, mOutputInfo.imgInfo.channels);
			break;
		case OUTPUT_FILE_TYPE_NUM:
		default:
			rt = ISP_FAILED;
			ILOGE("Not support output type:%d", mInputInfo.type);
			break;
	}
	return rt;
}

int32_t FileManager::SaveBMPData(uint8_t* srcData, int32_t channels)
{
	int32_t rt = ISP_SUCCESS;
	if (mState == Uninited) {
		rt = ISP_FAILED;
		ILOGE("ImageFile didnot init");
	}

	if(SUCCESS(rt)) {
		mOutputInfo.imgInfo.size = mOutputInfo.imgInfo.width *
			mOutputInfo.imgInfo.height *
			mOutputInfo.imgInfo.channels;
		BYTE* BMPdata = new BYTE[mOutputInfo.imgInfo.size];
		rt = SetBMP(srcData, mOutputInfo.imgInfo.channels, BMPdata);
		if (SUCCESS(rt)) {
			WriteBMP(BMPdata, channels);
		}
		else {
			ILOGE("SetBMP failed. rt:&d", rt);
		}
		delete[] BMPdata;
	}
	return rt;
}

int32_t FileManager::SetBMP(uint8_t* srcData, int32_t channels, BYTE* dstData)
{
	int32_t rt = ISP_SUCCESS;
	int32_t j = 0;
	BYTE temp;

	if(channels != 3 && channels != 1) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invalid BMP output channels:%d", channels);
		//TODO: does it need to be support?
	}

	if (SUCCESS(rt)) {
		if (channels == 1) {
			memcpy(dstData, srcData, mOutputInfo.imgInfo.width * mOutputInfo.imgInfo.height);
		} else if (channels == 3) {
			for (int32_t row = 0; row < mOutputInfo.imgInfo.height; row++) {
				for (int32_t col = 0; col < mOutputInfo.imgInfo.width; col++) {
					dstData[row * mOutputInfo.imgInfo.width * 3 + col * 3] = srcData[row * mOutputInfo.imgInfo.width + col];
					dstData[row * mOutputInfo.imgInfo.width * 3 + col * 3 + 1] = srcData[mOutputInfo.imgInfo.width * mOutputInfo.imgInfo.height + row * mOutputInfo.imgInfo.width + col];
					dstData[row * mOutputInfo.imgInfo.width * 3 + col * 3 + 2] = srcData[2 * mOutputInfo.imgInfo.width * mOutputInfo.imgInfo.height + row * mOutputInfo.imgInfo.width + col];
				}
			}
		}

		/* Convertion for the head & tail of data array */
		while (j < mOutputInfo.imgInfo.size - j) {
			temp = dstData[mOutputInfo.imgInfo.size - j - 1];
			dstData[mOutputInfo.imgInfo.size - j - 1] = dstData[j];
			dstData[j] = temp;
			j++;
		}

		/* Mirror flip */
		for (int32_t row = 0; row < mOutputInfo.imgInfo.height; row++) {
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

	return rt;
}

void FileManager::WriteBMP(BYTE* data, int32_t channels)
{
	BITMAPFILEHEADER header;
	BITMAPINFOHEADER headerinfo;
	int32_t tempSize;

	ILOGDF("BYTE:%d WORD:%d DWORD:%d LONG:%d", sizeof(BYTE), sizeof(WORD), sizeof(DWORD), sizeof(LONG));
	memset(&header, 0, sizeof(BITMAPFILEHEADER));
	memset(&headerinfo, 0, sizeof(BITMAPINFOHEADER));
	headerinfo.biSize = sizeof(BITMAPINFOHEADER);
	headerinfo.biHeight = mOutputInfo.imgInfo.height;
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
	ILOGDF("ch:%d head:%d headinfo:%d bfOffBits:%d(%x %x)",
			channels, sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER),
			tempSize, header.bfOffBits[1], header.bfOffBits[0]);
#elif defined WIN32_SYSTEM
	header.bfOffBits = tempSize;
#endif

	tempSize += headerinfo.biSizeImage;
#ifdef LINUX_SYSTEM
	header.bfSize[0] = (tempSize % 0x10000) & 0xffff;
	header.bfSize[1] = (tempSize / 0x10000) & 0xffff;
	ILOGDF("biSizeImage:%d bfSize:%d(%x %x)", headerinfo.biSizeImage,
			tempSize, header.bfSize[1], header.bfSize[0]);
#elif defined WIN32_SYSTEM
	header.bfSize = tempSize;
#endif

	ILOGI("BMP output path:%s", mOutputInfo.imgInfo.path);
	ofstream outputFile(mOutputInfo.imgInfo.path, ios::binary);
	if (outputFile.fail()) {
		ILOGE("Cannot open ouput file:%s", mOutputInfo.imgInfo.path);
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

int32_t FileManager::CreateVideo(void* dst)
{
	int32_t rt = ISP_SUCCESS;

	if (!mVideo) {
		mVideo = std::make_unique<ISPVideo>();
	}

	rt = mVideo->Init(dst);
	if (SUCCESS(rt)) {
		mVTParam.pVideo = mVideo.get();
		mVTParam.pFileMgr = this;
		rt = mVideo->CreateThread((void*)&mVTParam);
	}
	ILOGI("Video fps:%d total frame:%d",
			mOutputInfo.videoInfo.fps, mOutputInfo.videoInfo.frameNum);

	return rt;
}

int32_t FileManager::SaveVideoData(int32_t frameCount)
{
	int32_t rt = ISP_SUCCESS;

	rt = mVideo->Notify();
	ILOGDF("Notify F:%d", frameCount);

	return rt;
}


int32_t FileManager::DestroyVideo()
{
	int32_t rt = ISP_SUCCESS;

	rt = mVideo->DestroyThread();

	return rt;
}

int32_t FileManager::Mipi10decode(void* src, void* dst, ImgInfo* info)
{
	int32_t rt = ISP_SUCCESS;

	int32_t leftShift = 0;
	int32_t alignedW = ALIGNx(info->width, info->bitspp, CHECK_PACKAGED(info->rawFormat), info->stride);
	if (!info) {
		rt = ISP_INVALID_PARAM;
	}

	if (SUCCESS(rt)) {
		leftShift = info->bitspp - BITS_PER_WORD;
		if (leftShift < 0 || leftShift > 8)
		{
			rt = ISP_INVALID_PARAM;
		}
	}

	if (SUCCESS(rt)) {
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
				break;
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
				rt = ISP_INVALID_PARAM;
				ILOGE("Not support raw type:%d", info->rawFormat);
				break;
		}
		if (SUCCESS(rt)) {
			ILOGI("MIPI decode finished");
		}
	}

	return rt;
}

void DumpDataInt(void* pData, ...
		/* int32_t height, int32_t width, int32_t bitWidth, char* dumpPath */)
{
	int32_t rt = ISP_SUCCESS;
	int32_t width = 0;
	int32_t height = 0;
	int32_t bitWidth = 0;
	char* dumpPath = nullptr;
	va_list va;

	if (pData == nullptr) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Dump failed. data is nullptr");
	}

	if (SUCCESS(rt)) {
		va_start(va, pData);
		width = static_cast<int32_t>(va_arg(va, int32_t));
		height = static_cast<int32_t>(va_arg(va, int32_t));
		bitWidth = static_cast<int32_t>(va_arg(va, int32_t));
		dumpPath = static_cast<char*>(va_arg(va, char*));
		va_end(va);
		if (!dumpPath) {
		rt = ISP_INVALID_PARAM;
			ILOGE("dumpPath is null");
		}
	}

	if (SUCCESS(rt)) {
		ofstream dumpFile(dumpPath);
		if (!dumpFile) {
			rt = ISP_FAILED;
			ILOGE("Cannot create dump file:%s", dumpPath);
		}

		if(SUCCESS(rt)) {
			for (int32_t i = 0; i < height; i++) {
				dumpFile << i << ": ";
				for (int32_t j = 0; j < width; j++) {
					switch (bitWidth) {
						case sizeof(uint16_t) :
							dumpFile << (int)static_cast<uint16_t*>(pData)[i * width + j] << ' ';
							break;
						case sizeof(uint8_t) :
							dumpFile << (int)static_cast<uint8_t*>(pData)[i * width + j] << ' ';
							break;
						default:
							ILOGE("Dump failed. Unsopported data bitWidth:%d", bitWidth);
					}
				}
				dumpFile << endl;
			}
			ILOGI("Data saved as int at:%s", dumpPath);
		}
		dumpFile.close();
	}
}

int32_t CheckFilePath(char* pPath, void* pIn, void* pOut)
{
	int32_t rt = ISP_SUCCESS;

	if (!pPath || !pIn || !pOut) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Null input %d", rt);
	}

	if (SUCCESS(rt)) {
		if (!access(pPath, F_OK)) {
			if (!access(pPath, R_OK)) {
				ILOGI("File: %s", pPath);
			} else {
				rt = ISP_FAILED;
				ILOGE("Lack of read right for %s", pPath);
			}
		} else {
			rt = ISP_FAILED;
			ILOGE("File not exit. %s", pPath);
		}
	}

	if (SUCCESS(rt)) {
		int32_t len = 0, dotIndex = 0;
		while(pPath[len] != '\0') {
			len++;
		}
		if (len >= FILE_PATH_MAX_SIZE) {
			rt = ISP_INVALID_PARAM;
			ILOGE("Input file path size:%d > %d", len, FILE_PATH_MAX_SIZE);
		}
		if (SUCCESS(rt)) {
			dotIndex = len;
			while(pPath[dotIndex] != '.' && dotIndex > 0) {
				dotIndex--;
			}
			if (dotIndex + OUTPUT_FILE_TYPE_SIZE >= FILE_PATH_MAX_SIZE) {
				rt = ISP_INVALID_PARAM;
				ILOGE("Output file path size:%d > %d", dotIndex + 4, FILE_PATH_MAX_SIZE);
			}
		}
		if (SUCCESS(rt)) {
			memcpy(static_cast<InputInfo*>(pIn)->path, pPath, len);
			memcpy(static_cast<OutputInfo*>(pOut)->imgInfo.path, pPath, dotIndex + 1);
			memcpy(static_cast<OutputInfo*>(pOut)->imgInfo.path + dotIndex + 1, "bmp\0", OUTPUT_FILE_TYPE_SIZE);
			memcpy(static_cast<OutputInfo*>(pOut)->videoInfo.path, pPath, dotIndex + 1);
			memcpy(static_cast<OutputInfo*>(pOut)->videoInfo.path + dotIndex + 1, "avi\0", OUTPUT_FILE_TYPE_SIZE);
		}
	}

	return rt;
}

int32_t CheckImgSize(char* pCharW, char* pCharH, void* pInfo)
{
	int32_t rt = ISP_SUCCESS;

	if (!pCharW || !pCharH || !pInfo) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null");
	}

	if (SUCCESS(rt)) {
		int32_t w, h;
		w = CharNum2IntNum(pCharW);
		h = CharNum2IntNum(pCharH);

		if (w < 0 || h < 0) {
			rt = ISP_INVALID_PARAM;
			ILOGE("Invalid Size: %sx%s", pCharW, pCharH);
		} else {
			static_cast<MediaInfo*>(pInfo)->img.width = w;
			static_cast<MediaInfo*>(pInfo)->img.height = h;
			ILOGI("Size: %dx%d", w, h);
		}
	}

	return rt;
}

int32_t CheckImgFmt(char* pCharFmt, void* pInfo)
{
	int32_t rt = ISP_SUCCESS;
	uint32_t len = 0;
	uint32_t fmt = 0x0;

	if (!pCharFmt || !pInfo) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null");
	}

	if (SUCCESS(rt)) {
		while(pCharFmt[len] != '\0') {
			len++;
		}
		if (len + 1 < 4) {
			rt = ISP_INVALID_PARAM;
			ILOGE("Invalid format:%s", pCharFmt);
		}
	}

	if (SUCCESS(rt)) {
		fmt = len == 4 ?
			v4l2_fourcc(pCharFmt[0], pCharFmt[1], pCharFmt[2], pCharFmt[3]) :
			v4l2_fourcc(pCharFmt[0], pCharFmt[1], pCharFmt[2], ' ');
		for (int32_t i = sizeof(gSupportedFmt)/sizeof(ImgFmtInfo) - 1; i >= 0; i--) {
			if (gSupportedFmt[i].fmt == fmt) {
				ILOGI("Format: %s (%c%c%c%c)", gSupportedFmt[i].info,
						fmt, fmt >> 8, fmt >> 16, fmt >> 24);
				static_cast<MediaInfo*>(pInfo)->img.bitspp = gSupportedFmt[i].bitWidth;
				static_cast<MediaInfo*>(pInfo)->img.bayerOrder = gSupportedFmt[i].order;
				static_cast<MediaInfo*>(pInfo)->img.stride = 0; /* 0 as default */
				if (fmt == V4L2_PIX_FMT_SBGGR10P ||
						fmt == V4L2_PIX_FMT_SGBRG10P ||
						fmt == V4L2_PIX_FMT_SGRBG10P ||
						fmt == V4L2_PIX_FMT_SRGGB10P) {
					static_cast<MediaInfo*>(pInfo)->img.rawFormat = ANDROID_RAW10;
				/* TODO: find v4l2 fmt for ORDINAL_RAW10 */
//				} else if (fmt == ??)
//					static_cast<MediaInfo*>(pInfo)->img.rawFormat = ORDINAL_RAW10;
				} else if (fmt == V4L2_PIX_FMT_SBGGR10 ||
						fmt == V4L2_PIX_FMT_SGBRG10 ||
						fmt == V4L2_PIX_FMT_SGRBG10 ||
						fmt == V4L2_PIX_FMT_SRGGB10) {
					static_cast<MediaInfo*>(pInfo)->img.rawFormat = UNPACKAGED_RAW10_LSB;
				} else {
					static_cast<MediaInfo*>(pInfo)->img.rawFormat = RAW_FORMAT_NUM;
				}
				break;
			} else if (i == 0) {
				rt = ISP_INVALID_PARAM;
				ILOGE("Not support format:%s(0x%x)", pCharFmt, fmt);
			}
		}
	}

	return rt;
}

int32_t CheckImgStride(char* pCharStride, void* pInfo)
{
	int32_t rt = ISP_SUCCESS;

	if (!pCharStride || !pInfo) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null");
	}

	if (SUCCESS(rt)) {
		int32_t stride;
		stride = CharNum2IntNum(pCharStride);

		if (stride < 0 || stride % 2 != 0) {
			rt = ISP_INVALID_PARAM;
			ILOGE("Invalid stride: %s", pCharStride);
		} else {
			static_cast<MediaInfo*>(pInfo)->img.stride = stride;
			ILOGI("stride: %u", stride);
		}
	}

	return rt;
}

int FileManager::Input(IOInfo ioInfo)
{
	int32_t rt = ISP_SUCCESS;

	if (SUCCESS(rt)) {
		if (ioInfo.argc <= 1) {
			IOInfoFlag = 1;
			ILOGI("Use default I/O config");
			return rt;
		}
	}

	if (SUCCESS(rt)) {
		for (int32_t i = 0; i < (ioInfo.argc < MAX_IO_PARAM_CNT ? ioInfo.argc : MAX_IO_PARAM_CNT); i++) {
			if (!ioInfo.argv[i]) {
				rt = ISP_INVALID_PARAM;
				ILOGE("Null param[%d]", i);
				break;
			}

			if (SUCCESS(rt)) {
				if (i == 1) {
					if (!strcmp(ioInfo.argv[i], "-h") || !strcmp(ioInfo.argv[i], "-help")) {
						HelpMenu();
						rt = ISP_SKIP;
						break;
					}
					if (!strcmp(ioInfo.argv[i], "-l") || !strcmp(ioInfo.argv[i], "-list")) {
						SupportInfo();
						rt = ISP_SKIP;
						break;
					}

					if (ioInfo.argc - 1 < MIN_IO_PARAM_CNT) {
						rt = ISP_INVALID_PARAM;
						ILOGE("Insufficient param num:%d < expected param num:%d", ioInfo.argc - 1, MIN_IO_PARAM_CNT);
						break;
					}
					rt = CheckFilePath(ioInfo.argv[i], &mInputInfo, &mOutputInfo);
				}
			}

			if (SUCCESS(rt)) {
				if (i == 3) {
					rt = CheckImgSize(ioInfo.argv[i-1], ioInfo.argv[i], pDynamicInfo);
				}
			}
			if (SUCCESS(rt)) {
				if (i == 4) {
					rt = CheckImgFmt(ioInfo.argv[i], pDynamicInfo);
				}
			}
			if (SUCCESS(rt)) {
				if (i == 5) {
					rt = CheckImgStride(ioInfo.argv[i], pDynamicInfo);
				}
			}
		}
	}

	return rt;
}

void FileManager::SupportInfo()
{
	ILOGI("==================================================================");
	ILOGI(" Support Format List");
	ILOGI("==================================================================");
	ILOGI(" %-25s \t| %-10s \t| %s", "Image Format", "Bit Width", "FMT Param");
	ILOGI("------------------------------------------------------------------");
		for (uint32_t i = 0; i < sizeof(gSupportedFmt)/sizeof(ImgFmtInfo); i++) {
			if (i > 0 && (
					gSupportedFmt[i].bitWidth !=
					gSupportedFmt[i-1].bitWidth ||
					gSupportedFmt[i].cspace !=
					gSupportedFmt[i-1].cspace ||
					gSupportedFmt[i].dpt !=
					gSupportedFmt[i-1].dpt
					)) {
	ILOGI("------------------------------------------------------------------");
			}
			ILOGI(" %-25s \t| %-10u \t| %c%c%c%c"
#ifndef LOG_FOR_RELEASE
			"(0x%-4x)"
#endif
				  , gSupportedFmt[i].info,
					gSupportedFmt[i].bitWidth,
					gSupportedFmt[i].fmt,
					gSupportedFmt[i].fmt >> 8,
					gSupportedFmt[i].fmt >> 16,
					gSupportedFmt[i].fmt >> 24
#ifndef LOG_FOR_RELEASE
				  , gSupportedFmt[i].fmt
#endif
					);
		}
	ILOGI("------------------------------------------------------------------");
	ILOGI(" \t");
}

void FileManager::HelpMenu()
{
	ILOGI("==================================================================");
	ILOGI(" ISP v%d.%d", VERSION, SUB_VERSION);
	ILOGI(" Copyright (c) 2019 Peng Hao <635945005@qq.com>");
	ILOGI("==================================================================");
	ILOGI(" User Guide:");
	ILOGI(" 1. To get help, use cmd: ./ISP -h or ./ISP -help");
	ILOGI(" 2. To check format list, use cmd: ./ISP -l or ./ISP -list");
	ILOGI(" 3. To view an image, use cmd like this: ./ISP $path $width $height $fmt ($stride)");
	ILOGI(" \tpath\t: Target image path, like: /usr/bin/example.raw");
	ILOGI(" \twidth\t: Image width, like: 640");
	ILOGI(" \theight\t: Image height, like: 480");
	ILOGI(" \tfmt\t: Image format, like: pBAA. Please check <FMT Param> column in format list.");
	ILOGI(" \tstride\t: Optional param. If need, set stride for image");
	ILOGI(" \t");
}

int32_t FileManager::GetIOInfo(void* pInfo)
{
	int32_t rt = ISP_SUCCESS;

	if (!pInfo) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Null input! %d", rt);
	}

	if (SUCCESS(rt)) {
		switch(IOInfoFlag) {
			case 0:
				memcpy(pInfo, pDynamicInfo, sizeof(MediaInfo));
				break;
			case 1:
				memcpy(pInfo, pStaticInfo, sizeof(MediaInfo));
				break;
			default:
				ILOGE("Invalid I/O flag:%d", IOInfoFlag);
				break;
		}
	}
	return rt;
}
