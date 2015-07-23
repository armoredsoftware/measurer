dirs = $(shell ls -d */)

build-all:
	@for dir in $(dirs); do\
		cd $$dir;\
		make;\
		cd ..;\
	done		

run-all:
	@port=3000;for dir in $(dirs); do\
		cd $$dir;\
		../run.sh $$port;\
		cd ..;\
		let "port+=1";\
	done		
