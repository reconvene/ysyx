BUILD_DIR=build/sv

include ../Makefile

verilog:
	$(call git_commit, "generate verilog")
	mkdir -p $(BUILD_DIR)
	mill -i __.runMain $(word 2, $(MAKECMDGOALS)) --target-dir $(BUILD_DIR)
	cp -prf $(BUILD_DIR)/* /home/admin123/ysyx-branches/npc/npc/vsrc
