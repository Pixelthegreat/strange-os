.PHONY: setup clean

# primary targets
all: kernel stdlib user bootdisk

kernel:
	@echo ==== kernel ====
	@make -s -f src/kernel/Makefile all

stdlib:
	@echo ==== stdlib ====
	@make -s -f src/stdlib/Makefile all

user:
	@echo ===== user =====
	@make -s -f src/user/Makefile all

bootdisk: kernel
	@echo === bootdisk ===
	./bd.sh

setup:
	mkdir -pv build build/stdlib build/user

clean:
	rm build/*.o build/stdlib/*.o build/strange build/user/*
