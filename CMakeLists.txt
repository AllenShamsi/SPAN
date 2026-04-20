cmake_minimum_required(VERSION 3.5)

project(SPAN VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------- Windows/MSYS2 defaults so GUI settings aren't required ----------
if (WIN32 AND MINGW)
  set(_MSYS2_PREFIX "C:/msys64/mingw64")

  if (NOT DEFINED PORTAUDIO_ROOT OR PORTAUDIO_ROOT STREQUAL "")
    set(PORTAUDIO_ROOT "${_MSYS2_PREFIX}" CACHE PATH "PortAudio root (has include/ and lib/)")
  endif()
  if (NOT DEFINED FFTW_ROOT OR FFTW_ROOT STREQUAL "")
    set(FFTW_ROOT "${_MSYS2_PREFIX}" CACHE PATH "FFTW3 root (has include/ and lib/)")
  endif()

  list(APPEND CMAKE_PREFIX_PATH "${_MSYS2_PREFIX}")
  list(APPEND CMAKE_INCLUDE_PATH "${_MSYS2_PREFIX}/include")
  list(APPEND CMAKE_LIBRARY_PATH "${_MSYS2_PREFIX}/lib")
endif()

# -------------------------
# Qt & Eigen
# -------------------------
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia DataVisualization PrintSupport)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia DataVisualization PrintSupport)

# ---- Eigen: config -> module -> header-only fallback (works on MSYS2) ----
find_package(Eigen3 3.3 QUIET CONFIG)
if(NOT Eigen3_FOUND)
  find_package(Eigen3 3.3 QUIET)  # module mode, if available
endif()
if(NOT TARGET Eigen3::Eigen)
  if(WIN32 AND MINGW AND EXISTS "C:/msys64/mingw64/include/eigen3/Eigen/Core")
    set(EIGEN3_INCLUDE_DIR "C:/msys64/mingw64/include/eigen3" CACHE PATH "Path to Eigen3 headers")
  endif()
  if(NOT DEFINED EIGEN3_INCLUDE_DIR)
    set(EIGEN3_INCLUDE_DIR "" CACHE PATH "Path to Eigen3 headers")
  endif()
  if(EIGEN3_INCLUDE_DIR AND EXISTS "${EIGEN3_INCLUDE_DIR}/Eigen/Core")
    add_library(Eigen3::Eigen INTERFACE IMPORTED)
    target_include_directories(Eigen3::Eigen INTERFACE "${EIGEN3_INCLUDE_DIR}")
    message(STATUS "Using Eigen headers at ${EIGEN3_INCLUDE_DIR}")
  else()
    message(FATAL_ERROR "Eigen3 not found. Install mingw-w64-x86_64-eigen3 (MSYS2) or set EIGEN3_INCLUDE_DIR.")
  endif()
endif()

# ---- OpenGL (required because QCUSTOMPLOT_USE_OPENGL is enabled) ----
find_package(OpenGL REQUIRED)  # Provides OpenGL::GL (maps to opengl32 on Windows)
if(QT_VERSION_MAJOR EQUAL 6)
  find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)
endif()

# -------------------------
# Sources
# -------------------------
set(PROJECT_SOURCES
    main.cpp
    mainWindow.cpp
    mainWindow.h
    mainWindow.ui
    Sweep.h Sweep.cpp
    preprocessingWindow.h
    preprocessingWindow.cpp
    preprocessingWindow.ui
    OptionManager.h
    OptionManager.cpp
    Sensor3DVisualizer.h
    Sensor3DVisualizer.cpp
    SensorData.h
    UtilityFunctions.h
    UtilityFunctions.cpp
    dr_wav.h
    spanFile.h
    AudioVisualizer.h
    AudioVisualizer.cpp
    qcustomplot.h
    qcustomplot.cpp
    KinematicVisualizer.h
    KinematicVisualizer.cpp
    ButterworthFilter.h
    ButterworthFilter.cpp

    QRangeSlider.h
    QRangeSlider.cpp
    label.h
    label.cpp
    MyQCPItemText.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SPAN
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        resources.qrc
        app.rc
        FileInfoDialog.ui
        FileInfoDialog.h
        FileInfoDialog.cpp
        GestureTemplate.h
        GestureTemplateManager.h
        GestureTemplateManager.cpp
        SignalProcessing.h
        SignalProcessing.cpp
        SpanIO.h
        SpanIO.cpp
    )
else()
    if(ANDROID)
        add_library(SPAN SHARED ${PROJECT_SOURCES})
    else()
        add_executable(SPAN ${PROJECT_SOURCES})
    endif()
endif()


if (WIN32)
  target_sources(SPAN PRIVATE app.rc)
endif()


# ---- MinGW fixes for "file too big" / "too many sections"
if(MINGW AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # Ensure COFF big object format for all files in this target
  target_compile_options(SPAN PRIVATE -Wa,-mbig-obj)
  # Keep Debug builds smaller (section count & debug info)
  target_compile_options(SPAN PRIVATE
    $<$<CONFIG:Debug>:-g1 -fno-function-sections -fno-data-sections>)
  # Heaviest TUs: go stricter to avoid section explosion
  set_source_files_properties(qcustomplot.cpp UtilityFunctions.cpp PROPERTIES
    COMPILE_OPTIONS "-Wa,-mbig-obj;-g0;-fno-function-sections;-fno-data-sections;-fno-ipa-cp;-fno-ipa-sra")
endif()

# Scope the QCustomPlot define to this target
target_compile_definitions(SPAN PRIVATE QCUSTOMPLOT_USE_OPENGL)

# Always-needed include dirs
target_include_directories(SPAN PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/qcustomplot
)

# Libraries common to all platforms
set(SPAN_LIBS
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::DataVisualization
    Qt${QT_VERSION_MAJOR}::PrintSupport
    Eigen3::Eigen
)

# ---- Add OpenGL / Qt OpenGLWidgets to link set
if(QT_VERSION_MAJOR EQUAL 6)
  list(APPEND SPAN_LIBS Qt6::OpenGLWidgets)
else()
  # Qt5: link legacy OpenGL module for safety
  find_package(Qt5 COMPONENTS OpenGL REQUIRED)
  list(APPEND SPAN_LIBS Qt5::OpenGL)
endif()
if(TARGET OpenGL::GL)
  list(APPEND SPAN_LIBS OpenGL::GL)
elseif(TARGET OpenGL::OpenGL) # older FindOpenGL
  list(APPEND SPAN_LIBS OpenGL::OpenGL)
endif()

# =========================================================
# ===== Dependencies: PortAudio and FFTW (cross-platform) ==
# =========================================================

# --- Non-Windows: prefer pkg-config (Linux/macOS) ---
if(NOT WIN32)
    find_package(PkgConfig REQUIRED)

    # PortAudio via pkg-config
    pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)
    target_include_directories(SPAN PRIVATE ${PORTAUDIO_INCLUDE_DIRS})
    link_directories(${PORTAUDIO_LIBRARY_DIRS})
    list(APPEND SPAN_LIBS ${PORTAUDIO_LIBRARIES})

    # FFTW via pkg-config if available; otherwise macOS Homebrew fallback
    pkg_check_modules(FFTW QUIET fftw3)
    if(FFTW_FOUND)
        target_include_directories(SPAN PRIVATE ${FFTW_INCLUDE_DIRS})
        link_directories(${FFTW_LIBRARY_DIRS})
        list(APPEND SPAN_LIBS ${FFTW_LIBRARIES})
    else()
        if(APPLE)
            target_include_directories(SPAN PRIVATE /opt/homebrew/opt/fftw/include /usr/local/opt/fftw/include)
            link_directories(/opt/homebrew/opt/fftw/lib /usr/local/opt/fftw/lib)
            list(APPEND SPAN_LIBS fftw3)
        endif()
    endif()

# --- Windows (MinGW/MSYS2): no pkg-config; auto-detect or use roots ---
else()
    # ---- PortAudio ----
    find_path(PORTAUDIO_INCLUDE_DIR portaudio.h
        HINTS
            "${PORTAUDIO_ROOT}/include" "$ENV{PORTAUDIO_ROOT}/include"
            "${_MSYS2_PREFIX}/include"
        PATH_SUFFIXES include
    )
    find_library(PORTAUDIO_LIBRARY
        NAMES portaudio portaudio_x64 portaudio_static
        HINTS
            "${PORTAUDIO_ROOT}/lib" "${PORTAUDIO_ROOT}/lib64"
            "$ENV{PORTAUDIO_ROOT}/lib" "$ENV{PORTAUDIO_ROOT}/lib64"
            "${_MSYS2_PREFIX}/lib"
        PATH_SUFFIXES lib lib64
    )
    if(NOT PORTAUDIO_INCLUDE_DIR OR NOT PORTAUDIO_LIBRARY)
        message(FATAL_ERROR "PortAudio not found. Install mingw-w64-x86_64-portaudio (MSYS2) or set PORTAUDIO_ROOT.")
    endif()
    target_include_directories(SPAN PRIVATE ${PORTAUDIO_INCLUDE_DIR})
    list(APPEND SPAN_LIBS ${PORTAUDIO_LIBRARY})

    # ---- FFTW3 ----
    find_path(FFTW_INCLUDE_DIR fftw3.h
        HINTS
            "${FFTW_ROOT}/include" "$ENV{FFTW_ROOT}/include"
            "${_MSYS2_PREFIX}/include"
        PATH_SUFFIXES include
    )
    find_library(FFTW_LIBRARY
        NAMES fftw3 fftw3-3 libfftw3-3
        HINTS
            "${FFTW_ROOT}/lib" "${FFTW_ROOT}/lib64"
            "$ENV{FFTW_ROOT}/lib" "$ENV{FFTW_ROOT}/lib64"
            "${_MSYS2_PREFIX}/lib"
        PATH_SUFFIXES lib lib64
    )
    if(NOT FFTW_INCLUDE_DIR OR NOT FFTW_LIBRARY)
        message(FATAL_ERROR "FFTW3 not found. Install mingw-w64-x86_64-fftw (MSYS2) or set FFTW_ROOT.")
    endif()
    target_include_directories(SPAN PRIVATE ${FFTW_INCLUDE_DIR})
    list(APPEND SPAN_LIBS ${FFTW_LIBRARY})
endif()

# -------------------------
# Link everything
# -------------------------
target_link_libraries(SPAN PRIVATE ${SPAN_LIBS})

# -------------------------
# App bundle / install
# -------------------------
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.SPAN)
endif()

set_target_properties(SPAN PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS SPAN
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SPAN)
endif()