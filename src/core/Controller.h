/* PythonXM
 * Copyright 2018-2021 SChernykh   <https://github.com/SChernykh>
 * Copyright 2016-2021 PythonXM       <https://github.com/pythonxm>, <support@pythonxm.com>
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

#ifndef PYTHONXM_CONTROLLER_H
#define PYTHONXM_CONTROLLER_H


#include "base/kernel/Base.h"


#include <memory>


namespace pythonxm {


class HwApi;
class Job;
class Miner;
class Network;


class Controller : public Base
{
public:
    PYTHONXM_DISABLE_COPY_MOVE_DEFAULT(Controller)

    Controller(Process *process);
    ~Controller() override;

    int init() override;
    void start() override;
    void stop() override;

    Miner *miner() const;
    Network *network() const;
    void execCommand(char command);

private:
    std::shared_ptr<Miner> m_miner;
    std::shared_ptr<Network> m_network;

#   ifdef PYTHONXM_FEATURE_API
    std::shared_ptr<HwApi> m_hwApi;
#   endif
};


} // namespace pythonxm


#endif /* PYTHONXM_CONTROLLER_H */
