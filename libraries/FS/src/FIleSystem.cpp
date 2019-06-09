#include "FileSystem.h"

FileImplPtr VFSImpl::open(const char* path, const char* mode)
{
    if(!_mountpoint) {
        log_e("File system is not mounted");
        return FileImplPtr();
    }

    if(!path || path[0] != '/') {
        log_e("%s does not start with /", path);
        return FileImplPtr();
    }

    char * temp = (char *)malloc(strlen(path)+strlen(_mountpoint)+2);
    if(!temp) {
        log_e("malloc failed");
        return FileImplPtr();
    }

    sprintf(temp,"%s%s", _mountpoint, path);

    struct stat st;
    //file lound
    if(!stat(temp, &st)) {
        free(temp);
        if (S_ISREG(st.st_mode) || S_ISDIR(st.st_mode)) {
            return std::make_unique<VFSFileImpl>(this, path, mode);
        }
        log_e("%s has wrong mode 0x%08X", path, st.st_mode);
        return FileImplPtr();
    }

    //file not found but mode permits creation
    if(mode && mode[0] != 'r') {
        free(temp);
        return std::make_unique<VFSFileImpl>(this, path, mode);
    }

    //try to open this as directory (might be mount point)
    DIR * d = opendir(temp);
    if(d) {
        closedir(d);
        free(temp);
        return std::make_unique<VFSFileImpl>(this, path, mode);
    }

    log_e("%s does not exist", temp);
    free(temp);
    return FileImplPtr();
}

bool VFSImpl::exists(const char* path)
{
    if(!_mountpoint) {
        log_e("File system is not mounted");
        return false;
    }

    VFSFileImpl f(this, path, "r");
    if(f) {
        f.close();
        return true;
    }
    return false;
}

bool VFSImpl::rename(const char* pathFrom, const char* pathTo)
{
    if(!_mountpoint) {
        log_e("File system is not mounted");
        return false;
    }

    if(!pathFrom || pathFrom[0] != '/' || !pathTo || pathTo[0] != '/') {
        log_e("bad arguments");
        return false;
    }
    if(!exists(pathFrom)) {
        log_e("%s does not exists", pathFrom);
        return false;
    }
    char * temp1 = (char *)malloc(strlen(pathFrom)+strlen(_mountpoint)+1);
    if(!temp1) {
        log_e("malloc failed");
        return false;
    }
    char * temp2 = (char *)malloc(strlen(pathTo)+strlen(_mountpoint)+1);
    if(!temp2) {
        free(temp1);
        log_e("malloc failed");
        return false;
    }
    sprintf(temp1,"%s%s", _mountpoint, pathFrom);
    sprintf(temp2,"%s%s", _mountpoint, pathTo);
    auto rc = ::rename(temp1, temp2);
    free(temp1);
    free(temp2);
    return rc == 0;
}

bool VFSImpl::remove(const char* path)
{
    if(!_mountpoint) {
        log_e("File system is not mounted");
        return false;
    }

    if(!path || path[0] != '/') {
        log_e("bad arguments");
        return false;
    }

    VFSFileImpl f(this, path, "r");
    if(!f || f.isDirectory()) {
        if(f) {
            f.close();
        }
        log_e("%s does not exists or is directory", path);
        return false;
    }
    f.close();

    char * temp = (char *)malloc(strlen(path)+strlen(_mountpoint)+1);
    if(!temp) {
        log_e("malloc failed");
        return false;
    }
    sprintf(temp,"%s%s", _mountpoint, path);
    auto rc = unlink(temp);
    free(temp);
    return rc == 0;
}

bool VFSImpl::mkdir(const char *path)
{
    if(!_mountpoint) {
        log_e("File system is not mounted");
        return false;
    }

    VFSFileImpl f(this, path, "r");
    if(f && f.isDirectory()) {
        f.close();
        //log_w("%s already exists", path);
        return true;
    } else if(f) {
        f.close();
        log_e("%s is a file", path);
        return false;
    }

    char * temp = (char *)malloc(strlen(path)+strlen(_mountpoint)+1);
    if(!temp) {
        log_e("malloc failed");
        return false;
    }
    sprintf(temp,"%s%s", _mountpoint, path);
    auto rc = ::mkdir(temp, ACCESSPERMS);
    free(temp);
    return rc == 0;
}

bool VFSImpl::rmdir(const char *path)
{
    if(!_mountpoint) {
        log_e("File system is not mounted");
        return false;
    }

    VFSFileImpl f(this, path, "r");
    if(!f || !f.isDirectory()) {
        if(f) {
            f.close();
        }
        log_e("%s does not exists or is a file", path);
        return false;
    }
    f.close();

    char * temp = (char *)malloc(strlen(path)+strlen(_mountpoint)+1);
    if(!temp) {
        log_e("malloc failed");
        return false;
    }
    sprintf(temp,"%s%s", _mountpoint, path);
    auto rc = unlink(temp);
    free(temp);
    return rc == 0;
}