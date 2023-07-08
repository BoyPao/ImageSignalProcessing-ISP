// License: GPL-3.0-or-later
/*
 * Coder head file.
 *
 * Copyright (c) 2023 Peng Hao <635945005@qq.com>
 */

struct InputInfo {

}

struct Format {
	int32_t cs;
	int32_t order;
	int32_t bpp;
};

class Coder : public CoderItf {
	public:
		Coder();
		~CoderItf();
		int32_t MediaDecode(InputInfo info);
		int32_t MediaEncode(OutputInfo info);

	private:
		int32_t ImageDecode(void* pData, Format inFmt, Format outFmt);
		int32_t ImageEncode(void* pData, Format inFmt, Format outFmt);
		int32_t VideoEncode(void* pData, Format inFmt, Format outFmt);

};
