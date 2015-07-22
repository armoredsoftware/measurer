dirs = $(shell ls -d */)

build-all:
	@$(foreach dir,$(dirs),cd $(dir);make)

run-all:
	@$(foreach dir,$(dirs),cd $(dir);./run.sh)
