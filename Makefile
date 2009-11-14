PKG_CONFIG_PATH= /home/tanxin/final/dist/lib/pkgconfig
export PKG_CONFIG_PATH

all: compile link

compile:
	gcc webcam_caster.c `pkg-config --cflags gstreamer-rtsp-0.10 gst-rtsp-server-0.10 libcurl` -o webcam_caster.o -c

link:
	gcc `pkg-config --libs gstreamer-rtsp-0.10 gst-rtsp-server-0.10 libcurl` webcam_caster.o -o webcam_caster

clean:
	rm -rf *.o webcam_caster
