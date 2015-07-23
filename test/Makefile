dirs = $(shell ls -d */)

build-all:
	@$(foreach dir,$(dirs),cd $(dir);make)

run-all:
	@port=3000;$(foreach dir,$(dirs),cd $(dir);./run.sh $$port;let "port+=1")
