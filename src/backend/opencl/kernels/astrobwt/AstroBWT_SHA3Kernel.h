/* PythonXM
 * Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
 * Copyright 2017-2018 XMR-Stak    <https://github.com/fireice-uk>, <https://github.com/psychocrypt>
 * Copyright 2018-2020 SChernykh   <https://github.com/SChernykh>
 * Copyright 2016-2020 PythonXM       <https://github.com/pythonxm>, <support@pythonxm.com>
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

#ifndef PYTHONXM_ASTROBWT_SHA3KERNEL_H
#define PYTHONXM_ASTROBWT_SHA3KERNEL_H


#include "backend/opencl/wrappers/OclKernel.h"


namespace pythonxm {


class AstroBWT_SHA3Kernel : public OclKernel
{
public:
    inline AstroBWT_SHA3Kernel(cl_program program) : OclKernel(program, "sha3") {}

    void enqueue(cl_command_queue queue, size_t threads);
    void setArgs(cl_mem inputs, cl_mem input_sizes, uint32_t input_stride, cl_mem output_salsa20_keys);
};


} // namespace pythonxm


#endif /* PYTHONXM_ASTROBWT_SHA3KERNEL_H */
