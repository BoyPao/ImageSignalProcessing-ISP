// License: GPL-3.0-or-later
/*
 * Project configuration infomation.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#ifdef LINUX_SYSTEM
#define PATH_FMT "/"
#define DYNAMIC_LIB_FMT ".so"
#define WORK_PATH "/home2/penghao/test_project/ISP/"
#elif defined WIN32_SYSTEM
#define PATH_FMT "\\"
#define DYNAMIC_LIB_FMT ".dll"
#define WORK_PATH "D:\\test_project\\ISP_NEW\\ISP_NEW\\ISP_NEW\\"
#endif


#define LIB_PATH "bin"
#define RES_PATH "res"

#define ALG_LIB_NAME "libbzalg"
