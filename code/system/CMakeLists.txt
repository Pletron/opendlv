# OpenDLV - Software for driverless vehicles realized with OpenDaVINCI
# Copyright (C) 2015 Chalmers REVERE
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

cmake_minimum_required(VERSION 2.8)

project(opendlv-system)

###########################################################################
# Set the search path for .cmake files.
SET (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake.Modules" ${CMAKE_MODULE_PATH})
SET (CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/share/cmake-${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}/Modules" ${CMAKE_MODULE_PATH})
SET (CMAKE_MODULE_PATH "${OPENDAVINCI_DIR}/share/cmake-${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}/Modules" ${CMAKE_MODULE_PATH})

###########################################################################
# Global compile flags.
include(CompileFlags)

###########################################################################
# Find CxxTest.
include(CheckCxxTestEnvironment)

###########################################################################
# Find OpenDaVINCI.
find_package(OpenDaVINCI REQUIRED)

###########################################################################
# Find odcantools.
set(AUTOMOTIVEDATA_DIR "${OPENDAVINCI_DIR}")
set(ODCANTOOLS_DIR "${OPENDAVINCI_DIR}")
find_package(odcantools REQUIRED)

###########################################################################
# Find OpenDLV.
set(OPENDLV_DIR "${OPENDAVINCI_DIR}")
find_package(OpenDLV REQUIRED)

###########################################################################
# Find reverefh16mapping.
set(REVEREFH16MAPPING_DIR "${CMAKE_INSTALL_PREFIX}")
find_package(reverefh16mapping REQUIRED)

###########################################################################
# Find OpenDLV data structures.
set(OPENDLVDATA_DIR "${CMAKE_INSTALL_PREFIX}")
find_package(opendlvdata REQUIRED)

###########################################################################
# Set common include directories.
include_directories(SYSTEM ${OPENDAVINCI_INCLUDE_DIRS})
include_directories(SYSTEM ${OPENDLV_INCLUDE_DIRS})
include_directories(SYSTEM ${ODCANTOOLS_INCLUDE_DIRS})
include_directories(SYSTEM ${OPENDLVDATA_INCLUDE_DIRS})
include_directories(SYSTEM ${REVEREFH16MAPPING_INCLUDE_DIRS})
include_directories(SYSTEM ${EIGEN3_INCLUDE_DIR})
include_directories(SYSTEM ${KALMAN_INCLUDE_DIR})
include_directories(SYSTEM ${TINYCNN_INCLUDE_DIR})

###########################################################################
# Set libraries to link against.
set(LIBRARIES ${OPENDAVINCI_LIBRARIES}
              ${OPENDLV_LIBRARIES}
              ${ODCANTOOLS_LIBRARIES}
              ${OPENDLVDATA_LIBRARIES}
              ${REVEREFH16MAPPING_LIBRARIES})

###########################################################################
# Add subfolders with sources.
add_subdirectory(core)
add_subdirectory(application)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../config/configuration DESTINATION . COMPONENT system)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../resource/system/application/perception/detectvehicle/trainedAnn.xml DESTINATION ./share/opendlv/system/application/perception/detectvehicle/ COMPONENT system)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../resource/system/application/perception/detectvehicle/haar_cars3.xml DESTINATION ./share/opendlv/system/application/perception/detectvehicle/ COMPONENT system)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../resource/system/application/perception/detectvehicle/trained_cnn DESTINATION ./share/opendlv/system/application/perception/detectvehicle/ COMPONENT system)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../resource/system/application/perception/detectvehicle/normalization_constants DESTINATION ./share/opendlv/system/application/perception/detectvehicle/ COMPONENT system)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../resource/tools/vision/projection/leftCameraTransformationMatrix.csv DESTINATION ./share/opendlv/tools/vision/projection/ COMPONENT system)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../resource/tools/vision/projection/rightCameraTransformationMatrix.csv DESTINATION ./share/opendlv/tools/vision/projection/ COMPONENT system)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../resource/tools/vision/projection/leftCameraWarpPoints.csv DESTINATION ./share/opendlv/tools/vision/projection/ COMPONENT system)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../resource/tools/vision/projection/rightCameraWarpPoints.csv DESTINATION ./share/opendlv/tools/vision/projection/ COMPONENT system)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../resource/tools/vision/projection/leftCameraTransformationMatrixWarped.csv DESTINATION ./share/opendlv/tools/vision/projection/ COMPONENT system)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../resource/tools/vision/projection/rightCameraTransformationMatrixWarped.csv DESTINATION ./share/opendlv/tools/vision/projection/ COMPONENT system)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../resource/system/application/perception/detectlane/leftCameraRegions.csv DESTINATION ./share/opendlv/system/application/perception/detectlane/ COMPONENT system)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../resource/system/application/perception/detectlane/rightCameraRegions.csv DESTINATION ./share/opendlv/system/application/perception/detectlane/ COMPONENT system)
