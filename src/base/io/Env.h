/* PythonXM
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

#ifndef PYTHONXM_ENV_H
#define PYTHONXM_ENV_H


#include "base/tools/String.h"


#include <map>


namespace pythonxm {


class Env
{
public:
    static String expand(const char *in, const std::map<String, String> &extra = {});
    static String get(const String &name, const std::map<String, String> &extra = {});
    static String hostname();
};


} /* namespace pythonxm */


#endif /* PYTHONXM_ENV_H */
