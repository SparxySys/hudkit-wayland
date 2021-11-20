HUDKIT_OBJS := $(addprefix build/objs/,$(patsubst %.cc,%.o,$(wildcard *.cc)))

PKG_CONFIG_FLAGS := $(shell pkg-config --cflags --libs keybinder-3.0 x11 gtk+-3.0)

all: | build/hudkit dist

hudkit: build/hudkit

build/objs/%.o: %.cc | build/objs build/libcef_dll_wrapper.a
	clang++ \
		-c \
		-DWRAPPING_CEF_SHARED \
		-DNDEBUG \
		$< \
		-o build/objs/$(patsubst %.cc,%.o,$<) \
		-isystem ./cef-project \
		-Wl,-rpath,. \
		-Wno-unused-command-line-argument \
		-I./cef-project \
		-L./build \
		-L./cef-project/Release \
		$(PKG_CONFIG_FLAGS) \
		-lcef_dll_wrapper \
		-lcef \
		-std=c++17

build/hudkit: $(HUDKIT_OBJS) | build/libcef_dll_wrapper.a
	@mkdir -p build
	clang++ \
		-DWRAPPING_CEF_SHARED \
		-DNDEBUG \
		$(HUDKIT_OBJS) \
		-o build/hudkit \
		-isystem ./cef-project \
		-Wl,-rpath,. \
		-I./cef-project \
		-L./build \
		-L./cef-project/Release \
		$(PKG_CONFIG_FLAGS) \
		-lcef_dll_wrapper \
		-lcef \
		-std=c++17
	cp fetch-required-files.sh build/

build/libcef_dll_wrapper.a: build/libcef_dll_wrapper
	@cd build/libcef_dll_wrapper; \
	clang++ \
		-c \
		-DWRAPPING_CEF_SHARED \
		-DNDEBUG \
		-pthread \
		../../cef-project/libcef_dll/{.,base,{cpptoc,ctocpp}{/views,},wrapper}/*.cc \
		-isystem ../../cef-project \
		-I../../cef-project \
		-std=c++17
	ar cr build/libcef_dll_wrapper.a build/libcef_dll_wrapper/*

build/libcef_dll_wrapper:
	@mkdir -p build/libcef_dll_wrapper

build/objs:
	@mkdir -p build/objs

clean:
	rm build/hudkit
	rm build/fetch-required-files.sh
	rm build/release.tar.gz

dist:
	cd build && tar zcf release.tar.gz hudkit fetch-required-files.sh
