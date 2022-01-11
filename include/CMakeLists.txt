file(GLOB_RECURSE source_files CONFIGURE_DEPENDS "*.hpp")
target_sources(${PROJECT_NAME} INTERFACE ${source_files})
target_include_directories(${PROJECT_NAME} INTERFACE ".")
