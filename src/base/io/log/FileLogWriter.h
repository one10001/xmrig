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

#ifndef PYTHONXM_FILELOGWRITER_H
#define PYTHONXM_FILELOGWRITER_H


#include <cstddef>
#include <cstdint>


namespace pythonxm {


class FileLogWriter
{
public:
    FileLogWriter() = default;
    FileLogWriter(const char *fileName) { open(fileName); }

    inline bool isOpen() const  { return m_file >= 0; }
    inline int64_t pos() const  { return m_pos; }

    bool open(const char *fileName);
    bool write(const char *data, size_t size);
    bool writeLine(const char *data, size_t size);

private:
#   ifdef PYTHONXM_OS_WIN
    char m_endl[3]  = "\r\n";
#   else
    char m_endl[2]  = "\n";
#   endif

    int m_file      = -1;
    int64_t m_pos   = 0;
};


} /* namespace pythonxm */


#endif /* PYTHONXM_FILELOGWRITER_H */
