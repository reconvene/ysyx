TOPNAME = npc
NXDC_FILES = constr/$(TOPNAME).nxdc
INC_PATH ?=

VERILATOR = verilator
VERILATOR_CFLAGS += -MMD --build -cc  \
				-O3 --x-assign fast --x-initial fast --noassert --trace-fst

BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir
BIN = $(BUILD_DIR)/$(TOPNAME)

default: $(BIN)

$(shell mkdir -p $(BUILD_DIR))

# constraint file
SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/$(TOPNAME)_auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@

# project source

# 判断是否被拆分为多模块
ifeq ($(wildcard vsrc/$(TOPNAME)),)
    VSRCS = $(shell find $(abspath ./vsrc) -name "$(TOPNAME).*v")
else
    VSRCS = $(shell sed 's|^|vsrc/$(TOPNAME)/|' vsrc/$(TOPNAME)/filelist.f)
	VSRCS += $(shell sed 's|^\./||; s|^|vsrc/$(TOPNAME)/|' vsrc/$(TOPNAME)/firrtl_black_box_resource_files.f)
endif

CSRCS = $(shell find $(abspath ./csrc) -name "$(TOPNAME)_sim_main.c" -or -name "$(TOPNAME)_sim_main.cc" -or -name "$(TOPNAME)_sim_main.cpp")
CSRCS += $(SRC_AUTO_BIND)

# rules for NVBoard
include $(NVBOARD_HOME)/scripts/nvboard.mk

# rules for verilator
INCFLAGS = $(addprefix -I, $(INC_PATH))
CXXFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOPNAME)\""

NPCFLAGS ?=


#all:sim
#	@echo "Write this Makefile by your self."

sim: $(VSRCS) $(CSRCS) $(NVBOARD_ARCHIVE)
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	$(VERILATOR) $(VERILATOR_CFLAGS) \
   		--top-module $(TOPNAME) $^ \
   		$(addprefix -CFLAGS , $(CXXFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
   		--Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))
	$(BUILD_DIR)/$(TOPNAME) $(NPCFLAGS)

clean:
	rm -rf $(BUILD_DIR)

include ../Makefile
