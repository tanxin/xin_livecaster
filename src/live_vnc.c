/**
 * Live Camera Caster
 *
 * Copyright 2005-2009 Shin Tan <tanxincn@gmail.com>
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "live_vnc.h"
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>


static void event_loop (GstElement * pipe){
	GstBus *bus;
	GstMessage *message = NULL;

	bus = gst_element_get_bus (GST_ELEMENT (pipe));

	while (TRUE) {
		message = gst_bus_poll (bus, GST_MESSAGE_ANY, -1);

		g_assert (message != NULL);

		switch (message->type) {
			case GST_MESSAGE_EOS:
				gst_message_unref (message);
				return;
			case GST_MESSAGE_WARNING:
			case GST_MESSAGE_ERROR:{
						       GError *gerror;
						       gchar *debug;

						       gst_message_parse_error (message, &gerror, &debug);
						       gst_object_default_error (GST_MESSAGE_SRC (message), gerror, debug);
						       gst_message_unref (message);
						       g_error_free (gerror);
						       g_free (debug);
						       return;
					       }
			default:
					       gst_message_unref (message);
					       break;
		}
	}
}


int main(int argc, char **argv){
	
	char buf[512];

	CURL *curl;
	CURLcode res;

	GstElement *bin;
	GstElement *rfbsrc, *videorate, *flt1, *videoscale, *flt2, *colorspace, *theoraenc, *oggmux, *shout2send;

	gst_init (&argc, &argv);

	struct Write pooh;

	sprintf(buf, XML_PATTERN, UUID);

	g_printerr("%s", buf);

	pooh.readptr = buf;
	pooh.sizeleft = strlen(buf);

	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, RPC_URL);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
		curl_easy_setopt(curl, CURLOPT_READDATA, &pooh);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		
		struct curl_slist *chunk = NULL;
		chunk = curl_slist_append(chunk, "Transfer-Encoding: chunked");
		res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}

	bin = gst_pipeline_new ("pipeline");
	g_assert(bin);

	rfbsrc = gst_element_factory_make("rfbsrc", "rfb source");
	g_assert(rfbsrc);

	if(argc>1)
		g_object_set (G_OBJECT (rfbsrc), "host", argv[1], NULL);
	else
		g_object_set (G_OBJECT (rfbsrc), "host", "127.0.0.1", NULL);

	if(argc>2)
		g_object_set (G_OBJECT (rfbsrc), "password", argv[2], NULL);


	flt1 = gst_element_factory_make("capsfilter", "scale filter");
	g_assert(flt1);
	g_object_set(G_OBJECT(flt1), "caps",
			gst_caps_new_simple("video/x-raw-yuv",
				"width", G_TYPE_INT, 800,
				"height", G_TYPE_INT, 450,
				NULL), NULL);

	flt2 = gst_element_factory_make("capsfilter", "rate filter");
	g_assert(flt2);
	g_object_set(G_OBJECT(flt2), "caps",
			gst_caps_new_simple("video/x-raw-yuv",
				"framerate", GST_TYPE_FRACTION, 5, 1,
				NULL), NULL);

	colorspace = gst_element_factory_make("ffmpegcolorspace", "ffmpeg color space");
	g_assert(colorspace);

	videoscale = gst_element_factory_make ("videoscale", "video scale");
	g_assert(videoscale);


	videorate = gst_element_factory_make ("videorate", "video rate");
	g_assert(videoscale);


	theoraenc = gst_element_factory_make("theoraenc", "theora encoder");
	g_assert(theoraenc);
	g_object_set (G_OBJECT (theoraenc), "quality", 30, NULL);

	oggmux = gst_element_factory_make("oggmux", "ogg muxer");
	g_assert(oggmux);

	shout2send = gst_element_factory_make("shout2send", "libshout sender");
	g_assert(shout2send);
	g_object_set (G_OBJECT (shout2send), "ip", "nucri.nu.edu", NULL);
	g_object_set (G_OBJECT (shout2send), "port", 6009, NULL);
	g_object_set (G_OBJECT (shout2send), "password", "rtsp", NULL);
	g_object_set (G_OBJECT (shout2send), "mount", MOUNT, NULL);

	gst_bin_add_many(GST_BIN(bin), rfbsrc, colorspace, videoscale, flt1, videorate, flt2, theoraenc, oggmux, shout2send, NULL);
	gst_element_link_many(rfbsrc, colorspace, videoscale, flt1, videorate, flt2, theoraenc, oggmux, shout2send, NULL);

	gst_element_set_state(bin, GST_STATE_PLAYING);

	event_loop(bin);
	
	gst_element_set_state(bin, GST_STATE_NULL);

	return 0;

}


static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp){
	struct Write *pooh = (struct Write *)userp;

	if(size*nmemb < 1)
		return 0;

	if(pooh->sizeleft) {
		*(char*)ptr = pooh->readptr[0];
		pooh->readptr++;
		pooh->sizeleft--;
		return 1;
	}

	return 0;
}
