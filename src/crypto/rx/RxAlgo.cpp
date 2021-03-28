/* PythonXM
 * Copyright (c) 2018-2019 tevador     <tevador@gmail.com>
 * Copyright (c) 2018-2020 SChernykh   <https://github.com/SChernykh>
 * Copyright (c) 2016-2020 PythonXM       <https://github.com/pythonxm>, <support@pythonxm.com>
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


#include "crypto/randomx/randomx.h"
#include "crypto/rx/RxAlgo.h"


pythonxm::Algorithm::Id pythonxm::RxAlgo::apply(Algorithm::Id algorithm)
{
    randomx_apply_config(*base(algorithm));

    return algorithm;
}


const RandomX_ConfigurationBase *pythonxm::RxAlgo::base(Algorithm::Id algorithm)
{
    switch (algorithm) {
    case Algorithm::RX_WOW:
        return &RandomX_WowneroConfig;

    case Algorithm::RX_ARQ:
        return &RandomX_ArqmaConfig;

    case Algorithm::RX_SFX:
        return &RandomX_SafexConfig;

    case Algorithm::RX_KEVA:
        return &RandomX_KevaConfig;

    default:
        break;
    }

    return &RandomX_MoneroConfig;
}


uint32_t pythonxm::RxAlgo::version(Algorithm::Id algorithm)
{
    return algorithm == Algorithm::RX_WOW ? 103 : 104;
}


uint32_t pythonxm::RxAlgo::programCount(Algorithm::Id algorithm)
{
    return base(algorithm)->ProgramCount;
}


uint32_t pythonxm::RxAlgo::programIterations(Algorithm::Id algorithm)
{
    return base(algorithm)->ProgramIterations;
}


uint32_t pythonxm::RxAlgo::programSize(Algorithm::Id algorithm)
{
    return base(algorithm)->ProgramSize;
}
