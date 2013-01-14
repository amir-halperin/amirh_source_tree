#this would be the make file for the osal unit tests input files
# note that this would generate exe file on windows
PARTIAL_BUILD = YES
COMPILE_NAME = osal_ut
LOBJS = countingSempahoreUT  eventNotificationUT messageQueueUT  mutexUT rwMutexUT  threadUT

LOCAL_INCLUDES = $(firstword $(subst /, , $(CURDIR)))/hf_src/framework/os/osal
        

include $(firstword $(subst /, , $(CURDIR)))/hf_tools/makes/make.mak
include $(firstword $(subst /, , $(CURDIR)))/hf_tools/makes/subdirs.mak
