PROJ_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

EXT_NAME=mooncake
EXT_CONFIG=${PROJ_DIR}extension_config.cmake
OVERRIDE_GIT_DESCRIBE ?= v2.0.1

include extension-ci-tools/makefiles/duckdb_extension.Makefile

format-all:
	cd moonlink_ffi && cargo fmt
	find src -iname '*.hpp' -o -iname '*.cpp' | xargs clang-format -i
