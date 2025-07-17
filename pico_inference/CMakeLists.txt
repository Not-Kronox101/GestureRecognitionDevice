cmake_minimum_required(VERSION 3.13)

# Change the PICO_SDK_PATH according to your own path
set(PICO_SDK_PATH "/path to you pico-sdk/pico-sdk")
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(pico_inference C CXX)

# Initialize the Pico SDK
pico_sdk_init()

# Edge Impulse 
set(EI_SDK_FOLDER ${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk)

macro(RECURSIVE_FIND_FILE_APPEND outvar dir pattern)
    file(GLOB_RECURSE TMP "${dir}/${pattern}")
    list(APPEND ${outvar} ${TMP})
endmacro()

# CMSIS DSP sources
set(_DSP_CMSIS_SOURCES "")
RECURSIVE_FIND_FILE_APPEND(_DSP_CMSIS_SOURCES "${EI_SDK_FOLDER}/CMSIS/DSP/Source" "*.c")

# CMSIS-NN sources
set(_NN_CMSIS_SOURCES "")
RECURSIVE_FIND_FILE_APPEND(_NN_CMSIS_SOURCES "${EI_SDK_FOLDER}/CMSIS/NN/Source" "*.c")

# TFLM core source files
set(_TFLM_CORE_SOURCES "")
list(APPEND _TFLM_CORE_SOURCES
    ${EI_SDK_FOLDER}/tensorflow/lite/c/common.c
    ${EI_SDK_FOLDER}/tensorflow/lite/kernels/internal/quantization_util.cc
    ${EI_SDK_FOLDER}/tensorflow/lite/kernels/internal/tensor_utils.cc
)

# TFLM micro kernels
RECURSIVE_FIND_FILE_APPEND(_TFLM_CORE_SOURCES "${EI_SDK_FOLDER}/tensorflow/lite" "*.cc")

# Edge-impulse library
add_library(ei_inference STATIC
    ${_DSP_CMSIS_SOURCES}
    ${_NN_CMSIS_SOURCES}
    ${_TFLM_CORE_SOURCES}
)

target_include_directories(ei_inference PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

# Main executables
add_executable(pico_inference
    main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/tflite-model/tflite_learn_3_compiled.cpp
    ${EI_SDK_FOLDER}/porting/raspberry/ei_classifier_porting.cpp
    ${EI_SDK_FOLDER}/dsp/kissfft/kiss_fft.cpp
    ${EI_SDK_FOLDER}/dsp/kissfft/kiss_fftr.cpp
)

target_include_directories(pico_inference PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Enable USB
pico_enable_stdio_usb(pico_inference 1)
pico_enable_stdio_uart(pico_inference 0)

target_link_libraries(pico_inference
    ei_inference
    pico_stdlib
    hardware_adc
)

pico_add_extra_outputs(pico_inference)
