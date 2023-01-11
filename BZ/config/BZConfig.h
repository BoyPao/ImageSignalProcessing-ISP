// License: GPL-3.0-or-later
/*
 * BoZhi configuration file.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */


#ifdef LINUX_SYSTEM
#define PATH_FMT "/"
#define WORK_PATH "/home/hao/dev/ISP/"
#elif defined WIN32_SYSTEM
#define PATH_FMT "\\"
#define WORK_PATH "D:\\test_project\\ISP_NEW\\ISP_NEW\\ISP_NEW\\"
#endif

#define DUMP_PATH "out"
#define DUMP_FILE_NAME "dump.txt"
