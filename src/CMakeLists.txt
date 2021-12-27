file(GLOB_RECURSE source_files CONFIGURE_DEPENDS "*.cpp")
target_sources(${PROJECT_NAME} PRIVATE ${source_files})
