# for this to work only define the name of the file that would be generated - note that 
# if this is a library it would generate something like lib("you name").a which you 
# must set with the variable COMPILE_NAME, and the list of files that needs to be build
# that is any cpp file here in the variable LOBJS
# users can define their own local flags for the compilations using LOCAL_CFLAGS
# if the user here need to include special files, he/she can do so with LOCAL_INCLUDES
COMPILE_NAME = asynccallbacks

ifeq ($(TEST), YES)
WIN_LOCAL_CFLAGS += USE_SYNC_CALL_FOR_EXECUTER_OBJECT
endif
# all the other work (of the actual make process) is done in the file bellow
include $(firstword $(subst /, , $(CURDIR)))/hf_tools/makes/make.mak
