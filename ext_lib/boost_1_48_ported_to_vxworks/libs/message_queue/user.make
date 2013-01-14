# this would build a library for boost message queue
COMPILE_NAME = boost_messagequeue
LOBJS = message_queue mq_data
include $(firstword $(subst /, , $(CURDIR)))/hf_tools/makes/make.mak
