# This makefile extends the flag ALL_CXXFLAGS to include
# the include/ directory in the compiler's search path.
knnl_dir := $(dir $(lastword $(MAKEFILE_LIST)))
ALL_CXXFLAGS += -isystem $(knnl_dir)include/
