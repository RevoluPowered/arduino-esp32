/*
 FS.h - file system wrapper
 Copyright (c) 2015 Ivan Grokhotkov. All rights reserved.
 This file is part of the esp8266 core for Arduino environment.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef FS_H
#define FS_H
#include <Arduino.h>

extern "C" {
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>
}

#define FILE_READ       "r"
#define FILE_WRITE      "w"
#define FILE_APPEND     "a"



enum SeekMode {
    SeekSet = 0,
    SeekCur = 1,
    SeekEnd = 2
};

class File : public Stream
{
public:
    size_t write(uint8_t) override;
    size_t write(const uint8_t *buf, size_t size) override;
    int available() override;
    int read() override;
    int peek() override;
    void flush() override;
    size_t read(uint8_t* buf, size_t size);
    size_t readBytes(char *buffer, size_t length)
    {
        return read((uint8_t*)buffer, length);
    }

    bool seek(uint32_t pos, SeekMode mode);
    bool seek(uint32_t pos)
    {
        return seek(pos, SeekSet);
    }
    size_t position() const;
    size_t size() const;
    void close();
    operator bool() const;
    time_t getLastWrite();
    const char* name() const;

    boolean isDirectory(void);
    File openNextFile(const char* mode = FILE_READ);
    void rewindDirectory(void);
};


#endif //FS_H
