#ifndef FS_H
#define FS_H
#include <Arduino.h>
#include "File.h"

extern "C" {
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>
}

#define FILE_READ       "r"
#define FILE_WRITE      "w"
#define FILE_APPEND     "a"

class FileSystem
{
public:
    FileSystem()
    {}

    File open(const char* path, const char* mode = FILE_READ);
    File open(const String& path, const char* mode = FILE_READ);

    bool exists(const char* path);
    bool exists(const String& path);

    bool remove(const char* path);
    bool remove(const String& path);

    bool rename(const char* pathFrom, const char* pathTo);
    bool rename(const String& pathFrom, const String& pathTo);

    bool mkdir(const char *path);
    bool mkdir(const String &path);

    bool rmdir(const char *path);
    bool rmdir(const String &path);

    FILE *              _f;
    DIR *               _d;
    char *              _path;
    bool                _isDirectory;
    mutable struct stat _stat;
    mutable bool        _written;

    void _getStat() const;
};



#endif // FileSystem_H