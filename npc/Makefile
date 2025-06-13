# 顶部模块名字
TOPNAME = streamingLightTop
# 引口绑定文件名
NXDC_FILES = constr/top.nxdc

INC_PATH ?=

# 设置verilog模拟器
VERILATOR = verilator
# 启用O3优化、禁用断言、生成依赖文件、优化仿真速度
VERILATOR_CFLAGS += -MMD --build -cc  \
				-O3 --x-assign fast --x-initial fast --noassert --trace-fst

# 定义构建目标目录
BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir
# 定义可执行文件生成目录
BIN = $(BUILD_DIR)/$(TOPNAME)

# 创建构建目录
$(shell mkdir -p $(BUILD_DIR))

# 定义Verilog源码文件
VSRCS = $(shell find $(abspath ./vsrc) -name "*.*v")
# 定义C源码文件
CSRCS = $(shell find $(abspath ./csrc) -name "*.c" -or -name "*.cc" -or -name "*.cpp")
# 添加自动绑定端口文件
CSRCS += $(SRC_AUTO_BIND)

# 生成自动绑定端口文件
SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@

# 导入NVBoard的makefile文件
include $(NVBOARD_HOME)/scripts/nvboard.mk
include ../Makefile

# 定义顶层模块宏变量、添加nvboard头文件
INCFLAGS = $(addprefix -I, $(INC_PATH))
CXXFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOPNAME)\""

test:
	mill -i $(PRJ).test

verilog:
	$(call git_commit, "generate verilog")
	mkdir -p $(BUILD_DIR)
	mill -i $(PRJ).runMain Elaborate --target-dir $(BUILD_DIR)

help:
	mill -i $(PRJ).runMain Elaborate --help

reformat:
	mill -i __.reformat

checkformat:
	mill -i __.checkFormat

bsp:
	mill -i mill.bsp.BSP/install

idea:
	mill -i mill.idea.GenIdea/idea

clean:
	-rm -rf $(BUILD_DIR)

.PHONY: test verilog help reformat checkformat clean sim

sim: build
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	./build/$(TOPNAME)

build: $(VSRCS) $(CSRCS) $(NVBOARD_ARCHIVE)
	# 移除原obj_dir
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	@rm -rf $(OBJ_DIR)
	# 编译
	$(VERILATOR) $(VERILATOR_CFLAGS) \
		--top-module $(TOPNAME) $^ \
		$(addprefix -CFLAGS , $(CXXFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))
