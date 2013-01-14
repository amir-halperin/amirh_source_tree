# this file would build a library for boost thread
COMPILE_NAME = boost_thread
LOBJS = thread tss_dll tss_pe ../tss_null
LOCAL_CFLAGS = BOOST_THREAD_BUILD_LIB
include $(firstword $(subst /, , $(CURDIR)))/hf_tools/makes/make.mak
