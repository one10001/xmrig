/* PythonXM
 * Copyright (c) 2018-2021 SChernykh   <https://github.com/SChernykh>
 * Copyright (c) 2016-2021 PythonXM       <https://github.com/pythonxm>, <support@pythonxm.com>
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


#include "base/io/log/Tags.h"
#include "base/io/log/Log.h"


const char *pythonxm::Tags::config()
{
    static const char *tag = CYAN_BG_BOLD(WHITE_BOLD_S " config  ");

    return tag;
}


const char *pythonxm::Tags::network()
{
    static const char *tag = BLUE_BG_BOLD(WHITE_BOLD_S " net     ");

    return tag;
}


const char* pythonxm::Tags::origin()
{
    static const char* tag = YELLOW_BG_BOLD(WHITE_BOLD_S " origin  ");

    return tag;
}


const char *pythonxm::Tags::signal()
{
    static const char *tag = YELLOW_BG_BOLD(WHITE_BOLD_S " signal  ");

    return tag;
}


#ifdef PYTHONXM_MINER_PROJECT
const char *pythonxm::Tags::cpu()
{
    static const char *tag = CYAN_BG_BOLD(WHITE_BOLD_S " cpu     ");

    return tag;
}


const char *pythonxm::Tags::miner()
{
    static const char *tag = MAGENTA_BG_BOLD(WHITE_BOLD_S " miner   ");

    return tag;
}


#ifdef PYTHONXM_ALGO_RANDOMX
const char *pythonxm::Tags::randomx()
{
    static const char *tag = BLUE_BG(WHITE_BOLD_S " randomx ") " ";

    return tag;
}
#endif


#ifdef PYTHONXM_FEATURE_BENCHMARK
const char *pythonxm::Tags::bench()
{
    static const char *tag = GREEN_BG_BOLD(WHITE_BOLD_S " bench   ");

    return tag;
}
#endif
#endif


#ifdef PYTHONXM_PROXY_PROJECT
const char *pythonxm::Tags::proxy()
{
    static const char *tag = MAGENTA_BG_BOLD(WHITE_BOLD_S " proxy   ");

    return tag;
}
#endif


#ifdef PYTHONXM_FEATURE_CUDA
const char *pythonxm::Tags::nvidia()
{
    static const char *tag = GREEN_BG_BOLD(WHITE_BOLD_S " nvidia  ");

    return tag;
}
#endif


#ifdef PYTHONXM_FEATURE_OPENCL
const char *pythonxm::Tags::opencl()
{
    static const char *tag = MAGENTA_BG_BOLD(WHITE_BOLD_S " opencl  ");

    return tag;
}
#endif


#ifdef PYTHONXM_FEATURE_PROFILING
const char* pythonxm::Tags::profiler()
{
    static const char* tag = CYAN_BG_BOLD(WHITE_BOLD_S " profile ");

    return tag;
}
#endif
