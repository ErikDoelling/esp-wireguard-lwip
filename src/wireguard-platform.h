/*
 * Copyright (c) 2021 Daniel Hope (www.floorsense.nz)
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
 * Author: Daniel Hope <daniel.hope@smartalock.com>
 */

#ifndef _WIREGUARD_PLATFORM_H_
#define _WIREGUARD_PLATFORM_H_

#include <stdint.h>
#include <stdbool.h>
#include <sodium/randombytes.h>
#include "wireguard-crypto.h"

// Peers are allocated statically inside the device structure to avoid malloc
#define WIREGUARD_MAX_PEERS 3
#define WIREGUARD_MAX_SRC_IPS 5

// Per device limit on accepting (valid) initiation requests - per peer
#define MAX_INITIATIONS_PER_SECOND	(500)

#ifdef __cplusplus
extern "C" {
#endif

void wireguard_random_init();

// The number of milliseconds since system boot - for LwIP systems this could be sys_now()
uint32_t wireguard_sys_now(); 

// Fill the supplied buffer with random data - random data is used for generating new session keys periodically
void wireguard_random_bytes(void *bytes, size_t size);

void wireguard_tai64n_now(uint8_t *output);

// Is the system under load - i.e. should we generate cookie reply message in response to initiation messages
bool wireguard_is_under_load();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _WIREGUARD_PLATFORM_H_ */
