#!/bin/bash
mkdir -p build \
	&& cd build \
	&& scan-build-8 -o clang_scan_build cmake .. \
&& scan-build-8 -o clang_scan_build -v -V make
