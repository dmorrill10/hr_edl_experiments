OPEN_SPIEL :=src/open_spiel
ABSEIL :=$(OPEN_SPIEL)/abseil-cpp
BRIDGE :=$(OPEN_SPIEL)/games/bridge/double_dummy_solver
EIGEN :=src/eigen
OPEN_SPIEL_DEPS :=$(ABSEIL) $(BRIDGE)

# For IDE support based on clang
EXPORT_COMPILE_COMMANDS=1

# Build Rules
#============
default: build.optimized/Makefile build
	cd build.optimized && $(MAKE)
.PHONY: default

build.%:
	mkdir $@

build.%/Makefile: | $(OPEN_SPIEL) $(OPEN_SPIEL_DEPS) $(EIGEN) build.%
	cd build.$* && \
		cmake \
			-DCMAKE_EXPORT_COMPILE_COMMANDS=$(EXPORT_COMPILE_COMMANDS) \
			-DCMAKE_BUILD_TYPE=$* \
			-DBUILD_TYPE=$* \
			../src

build: | build.optimized
	ln -s build.optimized $@

test.%: | build.%/Makefile
	cd build.$* && $(MAKE)
	for test in `find ./build.$*/hr_edl -name *_test`; do echo $$test && $$test; done

test: | build.optimized/Makefile
	$(MAKE) test.optimized

$(OPEN_SPIEL):
	mkdir .tmp
	cd .tmp && \
		git init && \
		git remote add origin https://github.com/deepmind/open_spiel.git && \
		git fetch --depth 1 origin 53eeb6127578f91e88a6c0451983d10e3509446d && \
		git checkout FETCH_HEAD
	mv .tmp/open_spiel $@
	rm .tmp -rf

$(ABSEIL): $(OPEN_SPIEL)
	-git clone -b '20200923.3' --single-branch --depth 1 \
		https://github.com/abseil/abseil-cpp.git $@

$(BRIDGE): $(OPEN_SPIEL)
	-git clone -b 'develop' --single-branch --depth 1 \
		https://github.com/jblespiau/dds.git $@

$(EIGEN):
	mkdir .tmp
	cd .tmp && \
		git init && \
		git remote add origin https://gitlab.com/libeigen/eigen.git && \
		git fetch --depth 1 origin 39ec31c0adbdde6b8cda36b3415e9cc2af20dab6 && \
		git checkout FETCH_HEAD
	mv .tmp $@
	rm .tmp -rf

# Meta Rules
#===========
print-%:
	@echo $* = $($*)
.PHONY: print-%
