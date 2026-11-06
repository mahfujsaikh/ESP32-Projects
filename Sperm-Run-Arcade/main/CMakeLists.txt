# Main ESP-IDF component for the Sperm Run Arcade project.
# The missing gameplay source must be restored as main/main.cpp.
# LGFX_ESP32S3.h is local to this directory and can be included by name.
idf_component_register(SRCS "main.cpp" INCLUDE_DIRS ".")
