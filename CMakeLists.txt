cmake_minimum_required(VERSION 3.10)
project(luna_lone VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------------------------------------------
# Qt Setup
# ----------------------------------------------------------------
# These enable automatic processing of Qt meta-objects (signals/slots, .ui, .qrc)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${PROJECT_SOURCE_DIR}/ui)

if (NOT DEFINED CMAKE_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "C:/Qt/6.9.3/mingw_64/lib/cmake" CACHE PATH "Qt cmake path")
endif()

if (NOT DEFINED QT_BIN_DIR)
    set(QT_BIN_DIR "C:/Qt/6.9.3/mingw_64/bin" CACHE PATH "Qt bin path")
endif()

# Find the required Qt modules
find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)

# ----------------------------------------------------------------
# Dependencies
# ----------------------------------------------------------------
# include(FetchContent)

# ----------------------------------------------------------------
# Build sqlite3
# ----------------------------------------------------------------
add_library(sqlite3_obj OBJECT 
    ${PROJECT_SOURCE_DIR}/external/sqlite3/sqlite3.c
)
target_include_directories(sqlite3_obj PRIVATE 
    ${PROJECT_SOURCE_DIR}/external/sqlite3
)

set_source_files_properties(
    ${PROJECT_SOURCE_DIR}/external/sqlite3/sqlite3.c
    PROPERTIES LANGUAGE C
)

# ----------------------------------------------------------------
# Project
# ----------------------------------------------------------------
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/Database.cpp
    src/AudioItemWidget.cpp
    src/TagItemWidget.cpp
    src/TagChip.cpp
    src/TagAssignmentDialog.cpp
    src/AudioPlayer.cpp
    src/TextAnimator.cpp
    ui/MainWindow.ui
    ui/AudioItemWidget.ui
    ui/TagItemWidget.ui
    ui/TagAssignmentDialog.ui
    ui/TagChip.ui
    resources.qrc
    icon.rc
)

set(HEADERS
    include/luna-lone/MainWindow.h
    include/luna-lone/AudioItemWidget.h
    include/luna-lone/TagItemWidget.h
    include/luna-lone/TagChip.h
    include/luna-lone/TagAssignmentDialog.h
    include/luna-lone/AudioPlayer.h
    include/luna-lone/TextAnimator.h
)

qt_standard_project_setup()

qt_add_executable(luna_lone
    MANUAL_FINALIZATION
    ${SOURCES}
    ${HEADERS}
)

set_target_properties(luna_lone PROPERTIES WIN32_EXECUTABLE TRUE)

target_include_directories(luna_lone PRIVATE 
    ${PROJECT_SOURCE_DIR}/include/luna-lone
    ${PROJECT_SOURCE_DIR}/external/sqlite3
)

target_link_options(luna_lone PRIVATE -mwindows)

target_link_libraries(luna_lone PRIVATE 
    Qt6::Widgets Qt6::Multimedia
)

target_sources(luna_lone PRIVATE 
    $<TARGET_OBJECTS:sqlite3_obj>
)

# ----------------------------------------------------------------
# Debug build settings
# ----------------------------------------------------------------



# ----------------------------------------------------------------
# Release build settings
# ----------------------------------------------------------------

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR})

# ----------------------------------------------------------------
# Post-build commands
# ----------------------------------------------------------------

# Option to choose if data files are copied after development build
option(COPY_DATA_FILES "Copy data folder after development build" ON)

# Source and destination directories for copying data files
set(DATA_SRC_DIR ${PROJECT_SOURCE_DIR}/data)
set(DATA_DST_DIR ${CMAKE_CURRENT_BINARY_DIR}/data)

if(COPY_DATA_FILES)
    file(GLOB_RECURSE DATA_FILES RELATIVE ${DATA_SRC_DIR} ${DATA_SRC_DIR}/*)
    
    if(NOT EXISTS ${DATA_DST_DIR})
        file(MAKE_DIRECTORY ${DATA_DST_DIR})
    endif()

    foreach(file ${DATA_FILES})
        add_custom_command(TARGET luna_lone POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${DATA_SRC_DIR}/${file}
                ${DATA_DST_DIR}/${file}
        )
    endforeach()
endif()

add_custom_command(TARGET luna_lone POST_BUILD
    COMMAND ${QT_BIN_DIR}/windeployqt.exe $<TARGET_FILE:luna_lone>
    COMMENT "Running windeployqt to deploy Qt dependencies..."
)

# ----------------------------------------------------------------
# Finalization
# ----------------------------------------------------------------
qt_finalize_executable(luna_lone)