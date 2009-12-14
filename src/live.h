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

#include <gst/gst.h>

struct Write {
	const char *readptr;
	int sizeleft;
};

static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp);


