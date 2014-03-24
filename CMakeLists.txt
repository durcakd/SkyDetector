CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(SKYDET_NAME SkyDetector)
PROJECT( ${SKYDET_NAME} C CXX )

# Install destinations
SET( CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/_install CACHE PATH "Installation Destination" FORCE)
SET( INSTALL_DES ./ CACHE PATH "Where to install to." )

# Release is default
if( NOT CMAKE_BUILD_TYPE )
	SET( CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE ) #MinSizeRel
endif()

MESSAGE("CMAKE_BUILD_TYPE  ::" ${CMAKE_BUILD_TYPE})

# ============================================
### Source files

FILE(GLOB SRC_SKYDET	"src/*.cpp")
FILE(GLOB INCL_SKYDET	"include/*.h")

SET(SRC		${SRC_SKYDET})
SET(INCL	${INCL_SKYDET})

MESSAGE("SRC  ::" ${SRC})
MESSAGE("INCL ::" ${INCL})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}
					${CMAKE_CURRENT_SOURCE_DIR}
					${CMAKE_CURRENT_SOURCE_DIR}/include
					)

# ============================================

### OpenCV 
FIND_PACKAGE( OpenCV REQUIRED )
INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )

### Qt4
FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})
# autogenerate all moc files
SET( CMAKE_AUTOMOC TRUE )
ADD_DEFINITIONS(${QT_DEFINITIONS})
SET(QT_USE_QMAIN true)

MESSAGE("add_executable of ::" ${SKYDET_NAME})
SET( SRCS 	${INCL}
			${SRC})

ADD_EXECUTABLE(${SKYDET_NAME} ${SRCS} )

TARGET_LINK_LIBRARIES(${SKYDET_NAME}  ${OpenCV_LIBS} ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY} )

# =============================================
### install

INSTALL(TARGETS ${SKYDET_NAME}
	RUNTIME DESTINATION ${INSTALL_DES} COMPONENT Runtime )

