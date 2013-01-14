# for this to work only define the name of the file that would be generated - note that 
# if this is a library it would generate something like lib("you name").a which you 
# must set with the variable COMPILE_NAME, and the list of files that needs to be build
# that is any cpp file here in the variable LOBJS
# users can define their own local flags for the compilations using LOCAL_CFLAGS
# if the user here need to include special files, he/she can do so with LOCAL_INCLUDES
COMPILE_NAME = osal

#WIN_LOCAL_CFLAGS = SUPPORT_FOR_WIN32_OSAL
LOCAL_LIBS = boost_thread boost_messagequeue

LOBJS = Thread MessageQueue Mutex RWMutex ExitFunctionHolder EventNotification CountingSempahore StopWatch
ifeq (YES, $(TEST))
LOBJS += OsalTimeUtils
endif



ifeq (WIN32, $(TARGETOS))                         
SUBDIR_PARTIALS_TESTS +=  $(TOP_DIR)/drivers/boost/boost_1_48/libs/thread/win32 \
                         $(TOP_DIR)/drivers/boost/boost_1_48/libs/message_queue
endif

# all the other work (of the actual make process) is done in the file bellow
include $(firstword $(subst /, , $(CURDIR)))/hf_tools/makes/make.mak
ifeq (YES, $(TEST))
include $(firstword $(subst /, , $(CURDIR)))/hf_tools/makes/subdirs.mak
endif
