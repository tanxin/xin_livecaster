FIND_PATH(GSTRtspServer_INCLUDE_DIR gst/rtsp-server/rtsp-server.h
	PATHS
	/usr/include/gstreamer-0.10
	/usr/local/include/gstreamer-0.10
	../gst-rtsp-server
	)

FIND_LIBRARY(GSTRtspServer_LIBRARY NAMES gstrtspserver-0.10
	PATHS
	/usr/lib/gstreamer-0.10
	/usr/local/lib
	)

if(GSTRtspServer_INCLUDE_DIR AND GSTRtspServer_LIBRARY)
	#MESSAGE(STATUS "Found GST Rtsp Server: ${GSTRtspServer_INCLUDE_DIR}")
	set(GSTRtspServer_FOUND TRUE)
endif(GSTRtspServer_INCLUDE_DIR AND GSTRtspServer_LIBRARY)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GSTRtspServer DEFAULT_MSG GSTRtspServer_LIBRARY GSTRtspServer_INCLUDE_DIR)
