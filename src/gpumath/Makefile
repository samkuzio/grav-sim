OBJS := libgpumath.so

libgpumath.so:
	@nvcc --compiler-options '-fPIC' --shared gpumath.cu -o $(PROJECT_PATH)/libgpumath.so

# clean:
# 	@echo CLEAN $(PROJECT_PATH)/$(OBJS)
# 	@rm -f $(PROJECT_PATH)/$(OBJS)

# NOTE: here should be SUB_MAKE_INCLUDE, different from the top Makefile
include $(SUB_MAKE_INCLUDE)