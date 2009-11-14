ADD_EXECUTABLE(webcam_caster webcam_caster.c)
set(CMAKE_MODULE_PATH /usr/share/apps/cmake/modules)

#include_directories(/home/tanxin/final/dist/include/gstreamer-0.10
#	/usr/include/gstreamer-0.10 /usr/include/glib-2.0
#	/usr/lib64/glib-2.0/include /usr/include/libxml2)

include_directories(/home/tanxin/final/dist/include/gstreamer-0.10)

#TARGET_LINK_LIBRARIES(webcam_caster gstrtsp-0.10 gstsdp-0.10
#	gstrtspserver-0.10 gstbase-0.10 gstreamer-0.10 gobject-2.0 gmodule-2.0
#	gthread-2.0 rt xml2 glib-2.0 curl)

TARGET_LINK_LIBRARIES(webcam_caster gstrtspserver-0.10)

FIND_PACKAGE(CURL)
IF(CURL_FOUND)
	INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIR})
	TARGET_LINK_LIBRARIES(webcam_caster ${CURL_LIBRARY})
ELSE(CURL_FOUND)
	MESSAGE(FATAL_ERROR "CURL library not found")
ENDIF(CURL_FOUND)

FIND_PACKAGE(GStreamer)
IF(GSTREAMER_FOUND)
	INCLUDE_DIRECTORIES(${GSTREAMER_INCLUDE_DIR})
	TARGET_LINK_LIBRARIES(webcam_caster ${GSTREAMER_LIBRARY})
ELSE(GSTREAMER_FOUND)
	MESSAGE(FATAL_ERROR "GStreamer library not found")
ENDIF(GSTREAMER_FOUND)

FIND_PACKAGE(GLIB2)
IF(GLIB2_FOUND)
	INCLUDE_DIRECTORIES(${GLIB2_INCLUDE_DIR})
	TARGET_LINK_LIBRARIES(webcam_caster ${GLIB2_LIBRARIES})
ELSE(GLIB2_FOUND)
	MESSAGE(FATAL_ERROR "GLIB2 library not found")
ENDIF(GLIB2_FOUND)

FIND_PACKAGE(LibXml2)
IF(LIBXML2_FOUND)
	INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR})
	TARGET_LINK_LIBRARIES(webcam_caster ${LIBXML2_LIBRARIES})
ELSE(LIBXML2_FOUND)
	MESSAGE(FATAL_ERROR "GLIB2 library not found")
ENDIF(LIBXML2_FOUND)

