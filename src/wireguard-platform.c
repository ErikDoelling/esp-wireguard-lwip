/*
 * Copyright (c) 2021 Erik Dölling (www.zesys.de)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *  list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this
 *  list of conditions and the following disclaimer in the documentation and/or
 *  other materials provided with the distribution.
 *
 * 3. Neither the name of "Floorsense Ltd", "Agile Workspace Ltd" nor the names of
 *  its contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author: Erik Dölling <doelling@zesys.de>
 */

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/time.h>
#include "wireguard-platform.h"
#include "wireguard-crypto.h"
#include "esp_timer.h"
#include "esp_sntp.h"
#include "esp_system.h"
#include "time.h"
#include "lwip/sys.h"


uint32_t wireguard_sys_now() 
{
	return (uint32_t) (esp_timer_get_time() / 1000ULL); 
	// return sys_now();
} 

void wireguard_random_bytes(void *bytes, size_t size) 
{
	
	esp_fill_random(bytes, size);
}

void wireguard_tai64n_now(uint8_t *output) 
{
	uint64_t seconds;
	uint32_t nanos;

	struct timeval te;
	gettimeofday(&te, NULL);
	uint32_t milliseconds = te.tv_sec * 1000LL + (te.tv_usec / 1000LL);
	seconds = 0x400000000000000aULL + (milliseconds / 1000);
	nanos = (milliseconds % 1000) * 1000;
	
	U64TO8_BIG(output + 0, seconds);
	U32TO8_BIG(output + 8, nanos);
}

bool wireguard_is_under_load() 
{
    return false;
}
