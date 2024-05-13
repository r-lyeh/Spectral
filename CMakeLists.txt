cmake_minimum_required(VERSION 3.13...${CMAKE_VERSION})
project(Spectral LANGUAGES C CXX)

set(	Spectral_AY38910
	"floh" CACHE STRING
	"Spectral: AY-3-8910 emulator to use.")

set(	Spectral_Z80
	"floh" CACHE STRING
	"Spectral: Z80 emulator to use.")

add_executable(Spectral src/zx.c src/sys_window.cc)
target_compile_options(Spectral PRIVATE /MT /GL /GF /arch:AVX2)
target_compile_definitions(Spectral PRIVATE main=WinMain $<$<CONFIG:Release>:FLAGS=FLAGS_REL>)
target_link_options(Spectral PRIVATE /SUBSYSTEM:WINDOWS)

if(Spectral_AY38910 STREQUAL ayumi)
	target_compile_definitions(Spectral PRIVATE SPECTRAL_AY38910=SPECTRAL_AY38910_AYUMI)
elseif(Spectral_AY38910 STREQUAL floh)
	target_compile_definitions(Spectral PRIVATE SPECTRAL_AY38910=SPECTRAL_AY38910_FLOH)
else()
	message(FATAL_ERROR "Unknown AY-3-8910 emulator. Use `ayumi` or `floh`.")
endif()

if(Spectral_Z80 STREQUAL floh)
	target_compile_definitions(Spectral PRIVATE SPECTRAL_Z80=SPECTRAL_Z80_FLOH)
elseif(Spectral_Z80 STREQUAL redcode)
	target_compile_definitions(Spectral PRIVATE SPECTRAL_Z80=SPECTRAL_Z80_REDCODE)
else()
	message(FATAL_ERROR "Unknown Z80 emulator. Use `floh` or `redcode`.")
endif()
