/* PythonXM
 * Copyright 2010      Jeff Garzik              <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler                   <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones              <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466                 <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee                <jayddee246@gmail.com>
 * Copyright 2017-2019 XMR-Stak                 <https://github.com/fireice-uk>, <https://github.com/psychocrypt>
 * Copyright 2018      Lee Clagett              <https://github.com/vtnerd>
 * Copyright 2018-2019 tevador                  <tevador@gmail.com>
 * Copyright 2000      Transmeta Corporation    <https://github.com/intel/msr-tools>
 * Copyright 2004-2008 H. Peter Anvin           <https://github.com/intel/msr-tools>
 * Copyright 2018-2020 SChernykh                <https://github.com/SChernykh>
 * Copyright 2016-2020 PythonXM                    <https://github.com/pythonxm>, <support@pythonxm.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#include "crypto/astrobwt/AstroBWT.h"
#include "backend/cpu/Cpu.h"
#include "base/crypto/sha3.h"
#include "crypto/cn/CryptoNight.h"


#include <limits>


constexpr int STAGE1_SIZE = 147253;
constexpr int ALLOCATION_SIZE = (STAGE1_SIZE + 1048576) + (128 - (STAGE1_SIZE & 63));

constexpr int COUNTING_SORT_BITS = 10;
constexpr int COUNTING_SORT_SIZE = 1 << COUNTING_SORT_BITS;

static bool astrobwtInitialized = false;

#ifdef ASTROBWT_AVX2
static bool hasAVX2 = false;

extern "C"
#ifndef _MSC_VER
__attribute__((ms_abi))
#endif
void SHA3_256_AVX2_ASM(const void* in, size_t inBytes, void* out);
#endif

#ifdef _MSC_VER

#include <stdlib.h>
#define bswap_64(x) _byteswap_uint64(x)

#elif defined __GNUC__

#define bswap_64(x) __builtin_bswap64(x)

#else

#include <byteswap.h>

#endif

#ifdef PYTHONXM_ARM
extern "C" {
#include "salsa20_ref/ecrypt-sync.h"
}

static void Salsa20_XORKeyStream(const void* key, void* output, size_t size)
{
	uint8_t iv[8] = {};
	ECRYPT_ctx ctx;
	ECRYPT_keysetup(&ctx, static_cast<const uint8_t*>(key), 256, 64);
	ECRYPT_ivsetup(&ctx, iv);
	ECRYPT_keystream_bytes(&ctx, static_cast<uint8_t*>(output), size);
	memset(static_cast<uint8_t*>(output) - 16, 0, 16);
	memset(static_cast<uint8_t*>(output) + size, 0, 16);
}
#else
#include "Salsa20.hpp"

static void Salsa20_XORKeyStream(const void* key, void* output, size_t size)
{
	const uint64_t iv = 0;
	ZeroTier::Salsa20 s(key, &iv);
	s.XORKeyStream(output, size);
	memset(static_cast<uint8_t*>(output) - 16, 0, 16);
	memset(static_cast<uint8_t*>(output) + size, 0, 16);
}
#endif

void sort_indices(int N, const uint8_t* v, uint64_t* indices, uint64_t* tmp_indices)
{
	uint32_t counters[2][COUNTING_SORT_SIZE] = {};

	for (int i = 0; i < N; ++i)
	{
		const uint64_t k = bswap_64(*reinterpret_cast<const uint64_t*>(v + i));
		++counters[0][(k >> (64 - COUNTING_SORT_BITS * 2)) & (COUNTING_SORT_SIZE - 1)];
		++counters[1][k >> (64 - COUNTING_SORT_BITS)];
	}

	uint32_t prev[2] = { counters[0][0], counters[1][0] };
	counters[0][0] = prev[0] - 1;
	counters[1][0] = prev[1] - 1;
	for (int i = 1; i < COUNTING_SORT_SIZE; ++i)
	{
		const uint32_t cur[2] = { counters[0][i] + prev[0], counters[1][i] + prev[1] };
		counters[0][i] = cur[0] - 1;
		counters[1][i] = cur[1] - 1;
		prev[0] = cur[0];
		prev[1] = cur[1];
	}

	for (int i = N - 1; i >= 0; --i)
	{
		const uint64_t k = bswap_64(*reinterpret_cast<const uint64_t*>(v + i));
		tmp_indices[counters[0][(k >> (64 - COUNTING_SORT_BITS * 2)) & (COUNTING_SORT_SIZE - 1)]--] = (k & (static_cast<uint64_t>(-1) << 21)) | i;
	}

	for (int i = N - 1; i >= 0; --i)
	{
		const uint64_t data = tmp_indices[i];
		indices[counters[1][data >> (64 - COUNTING_SORT_BITS)]--] = data;
	}

	auto smaller = [v](uint64_t a, uint64_t b)
	{
		const uint64_t value_a = a >> 21;
		const uint64_t value_b = b >> 21;

		if (value_a < value_b)
			return true;

		if (value_a > value_b)
			return false;

		const uint64_t data_a = bswap_64(*reinterpret_cast<const uint64_t*>(v + (a % (1 << 21)) + 5));
		const uint64_t data_b = bswap_64(*reinterpret_cast<const uint64_t*>(v + (b % (1 << 21)) + 5));
		return (data_a < data_b);
	};

	uint64_t prev_t = indices[0];
	for (int i = 1; i < N; ++i)
	{
		uint64_t t = indices[i];
		if (smaller(t, prev_t))
		{
			const uint64_t t2 = prev_t;
			int j = i - 1;
			do
			{
				indices[j + 1] = prev_t;
				--j;
				if (j < 0)
					break;
				prev_t = indices[j];
			} while (smaller(t, prev_t));
			indices[j + 1] = t;
			t = t2;
		}
		prev_t = t;
	}
}

bool pythonxm::astrobwt::astrobwt_dero(const void* input_data, uint32_t input_size, void* scratchpad, uint8_t* output_hash, int stage2_max_size, bool avx2)
{
	alignas(8) uint8_t key[32];
	uint8_t* scratchpad_ptr = (uint8_t*)(scratchpad) + 64;
	uint8_t* stage1_output = scratchpad_ptr;
	uint8_t* stage2_output = scratchpad_ptr;
	uint64_t* indices = (uint64_t*)(scratchpad_ptr + ALLOCATION_SIZE);
	uint64_t* tmp_indices = (uint64_t*)(scratchpad_ptr + ALLOCATION_SIZE * 9);
	uint8_t* stage1_result = (uint8_t*)(tmp_indices);
	uint8_t* stage2_result = (uint8_t*)(tmp_indices);

#ifdef ASTROBWT_AVX2
	if (hasAVX2 && avx2)
		SHA3_256_AVX2_ASM(input_data, input_size, key);
	else
#endif
		sha3_HashBuffer(256, SHA3_FLAGS_NONE, input_data, input_size, key, sizeof(key));

	Salsa20_XORKeyStream(key, stage1_output, STAGE1_SIZE);

	sort_indices(STAGE1_SIZE + 1, stage1_output, indices, tmp_indices);

	{
		const uint8_t* tmp = stage1_output - 1;
		for (int i = 0; i <= STAGE1_SIZE; ++i)
			stage1_result[i] = tmp[indices[i] & ((1 << 21) - 1)];
	}

#ifdef ASTROBWT_AVX2
	if (hasAVX2 && avx2)
		SHA3_256_AVX2_ASM(stage1_result, STAGE1_SIZE + 1, key);
	else
#endif
		sha3_HashBuffer(256, SHA3_FLAGS_NONE, stage1_result, STAGE1_SIZE + 1, key, sizeof(key));

	const int stage2_size = STAGE1_SIZE + (*(uint32_t*)(key) & 0xfffff);
	if (stage2_size > stage2_max_size)
		return false;

	Salsa20_XORKeyStream(key, stage2_output, stage2_size);

	sort_indices(stage2_size + 1, stage2_output, indices, tmp_indices);

	{
		const uint8_t* tmp = stage2_output - 1;
		int i = 0;
		const int n = ((stage2_size + 1) / 4) * 4;
		for (; i < n; i += 4)
		{
			stage2_result[i + 0] = tmp[indices[i + 0] & ((1 << 21) - 1)];
			stage2_result[i + 1] = tmp[indices[i + 1] & ((1 << 21) - 1)];
			stage2_result[i + 2] = tmp[indices[i + 2] & ((1 << 21) - 1)];
			stage2_result[i + 3] = tmp[indices[i + 3] & ((1 << 21) - 1)];
		}
		for (; i <= stage2_size; ++i)
			stage2_result[i] = tmp[indices[i] & ((1 << 21) - 1)];
	}

#ifdef ASTROBWT_AVX2
	if (hasAVX2 && avx2)
		SHA3_256_AVX2_ASM(stage2_result, stage2_size + 1, output_hash);
	else
#endif
		sha3_HashBuffer(256, SHA3_FLAGS_NONE, stage2_result, stage2_size + 1, output_hash, 32);

	return true;
}


void pythonxm::astrobwt::init()
{
	if (!astrobwtInitialized) {
#		ifdef ASTROBWT_AVX2
		hasAVX2 = Cpu::info()->hasAVX2();
#		endif

		astrobwtInitialized = true;
	}
}


template<>
void pythonxm::astrobwt::single_hash<pythonxm::Algorithm::ASTROBWT_DERO>(const uint8_t* input, size_t size, uint8_t* output, cryptonight_ctx** ctx, uint64_t)
{
	astrobwt_dero(input, static_cast<uint32_t>(size), ctx[0]->memory, output, std::numeric_limits<int>::max(), true);
}
