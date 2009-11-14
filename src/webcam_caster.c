/**
 * RTSP Caster (webcam tester)
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

#define RPC_URL	"http://xmlrpc.secondlife.com/cgi-bin/xmlrpc.cgi"
#define UUID "d185cc00-2d0b-1143-28c1-2b717a493d1c"
#define XML_PATTERN "<?xml version=\"1.0\"?><methodCall><methodName>llRemoteData</methodName><params><param><value><struct><member><name>Channel</name><value><string>%s</string></value></member><member><name>IntValue</name><value><int>42</int></value></member><member><name>StringValue</name><value><string>rtsp://%s:8554/stream</string></value></member></struct></value></param></params></methodCall>"

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>


struct Write {
	const char *readptr;
	int sizeleft;
};

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

static gboolean timeout (GstRTSPServer *server, gboolean ignored) {
	GstRTSPSessionPool *pool;

	pool = gst_rtsp_server_get_session_pool (server);
	gst_rtsp_session_pool_cleanup (pool);
	g_object_unref (pool);

	return TRUE;
}


int main(int argc, char **argv){
	char buf[512];
	GMainLoop *loop;
	GstRTSPServer *server;
	GstRTSPMediaMapping *mapping;
	GstRTSPMediaFactory *stream;
	GstElement *v4l2src, *colorspace, *ffenc_h263, *rtph263pay, *audiotestsrc, *alawenc, *rtppcmapay;

	gst_init (&argc, &argv);
	loop = g_main_loop_new (NULL, FALSE);

	CURL *curl;
	CURLcode res;

	struct Write pooh;

	if(argc>1)
		sprintf(buf, XML_PATTERN, UUID, argv[1]);
	else
		sprintf(buf, XML_PATTERN, UUID, "localhost");

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

	server = gst_rtsp_server_new ();
	mapping = gst_rtsp_server_get_media_mapping (server);

	v4l2src = gst_element_factory_make("v4l2src", "webcam source");

	colorspace = gst_element_factory_make("ffmpegcolorspace", "ffmpeg color space");

	ffenc_h263 = gst_element_factory_make("ffenc_h263", "ffmpeg h263 encoder");

	rtph263pay = gst_element_factory_make("rtph263pay", "rtsp pay loader");
	g_assert(rtph263pay);
	g_object_set(G_OBJECT(rtph263pay), "name", "pay0", NULL);
	g_object_set(G_OBJECT(rtph263pay), "pt", 96, NULL);

	audiotestsrc = gst_element_factory_make("audiotestsrc", "audio");
	g_assert(audiotestsrc);
	g_object_set(G_OBJECT(audiotestsrc), "volume", 0, NULL);

	alawenc = gst_element_factory_make("alawenc", "sound encoder");
	rtppcmapay = gst_element_factory_make("rtppcmapay", "rtsp pay loader");
	g_assert(rtppcmapay);
	g_object_set(G_OBJECT(rtppcmapay), "name", "pay1", NULL);
	g_object_set(G_OBJECT(rtppcmapay), "pt", 97, NULL);

	gst_element_link_many(v4l2src, colorspace, ffenc_h263, rtph263pay, audiotestsrc, alawenc, rtppcmapay, NULL);


	stream = gst_rtsp_media_factory_new ();
	gst_rtsp_media_factory_set_launch (stream, "( "
			"v4l2src ! ffmpegcolorspace ! "
			"ffenc_h263 ! rtph263pay name=pay0 pt=96 "
			"audiotestsrc volume=0 ! audio/x-raw-int,rate=8000 ! "
			"alawenc ! rtppcmapay name=pay1 pt=97 "
			")");

	gst_rtsp_media_factory_set_shared (stream, TRUE);

	gst_rtsp_media_mapping_add_factory (mapping, "/stream", stream);
	g_object_unref (mapping);

	gst_rtsp_server_attach (server, NULL);
	g_timeout_add_seconds (2, (GSourceFunc) timeout, server);

	g_main_loop_run (loop);

	return 0;
}

