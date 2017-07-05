#include <cassert>
#include <string>
#include <algorithm>

#include "metafs.h"
#include "log.h"

MetaFS* MetaFS::_instance = NULL;

#define RETURN_ERRNO(x) do {if ((x) == 0) { return 0; } else { LOG(0,"%s => %d\n", #x, errno); return (-errno);}}while(false)

inline std::string stringify(const std::set<std::string> &data, const std::string &separator=" ") {
  std::string str;
  for(auto it=data.begin(); it!=data.end(); ++it) {
    if(str.length()>0) str += separator;
    str += (*it);
  }
  return str;
}

void MetaFS::dump_stat(const std::string &fullPath, struct stat *statbuf) const
{
  LOG(0, "%s: st_dev = %d\n", fullPath.c_str(), statbuf->st_dev );  /* ID of device containing file */
  LOG(0, "%s: st_ino = %d\n", fullPath.c_str(), statbuf->st_ino );  /* inode number */
  LOG(0, "%s: st_mode = %o\n", fullPath.c_str(), statbuf->st_mode );    /* protection */
  LOG(0, "%s: st_nlink = %d\n", fullPath.c_str(), statbuf->st_nlink );   /* number of hard links */
  LOG(0, "%s: st_uid = %d\n", fullPath.c_str(), statbuf->st_uid );     /* user ID of owner */
  LOG(0, "%s: st_gid = %d\n", fullPath.c_str(), statbuf->st_gid );     /* group ID of owner */
  LOG(0, "%s: st_rdev = %d\n", fullPath.c_str(), statbuf->st_rdev );    /* device ID (if special file) */
  LOG(0, "%s: st_size = %d\n", fullPath.c_str(), statbuf->st_size );    /* total size, in bytes */
  LOG(0, "%s: st_blksize = %d\n", fullPath.c_str(), statbuf->st_blksize ); /* blocksize for file system I/O */
  LOG(0, "%s: st_blocks = %d\n", fullPath.c_str(), statbuf->st_blocks );  /* number of 512B blocks allocated */
  LOG(0, "%s: st_atime = %d\n", fullPath.c_str(), statbuf->st_atime );   /* time of last access */
  LOG(0, "%s: st_mtime = %d\n", fullPath.c_str(), statbuf->st_mtime );   /* time of last modification */
  LOG(0, "%s: st_ctime = %d\n", fullPath.c_str(), statbuf->st_ctime );   /* time of last status change */
}

MetaFS* MetaFS::Instance() {
  if(_instance == NULL) {
    _instance = new MetaFS();
  }
  return _instance;
}

MetaFS::MetaFS(): m_root() {
}

MetaFS::~MetaFS() {
  LOG(0,"Unmounted 'tagfs' rooted from '%s'", m_root.c_str() );
  _instance = NULL;
}

void MetaFS::setRootDir(const char *path) {
  do {
    char *_path = realpath(path, NULL);
    if(_path==NULL) {
      LOG(0,"Error ('%s') setting FS root to: %s\n", strerror(errno), path);
    }
    m_root = _path;
    free(_path);
  } while(false);
  LOG(0,"setting FS root to: %s\n", m_root.c_str());
}

int MetaFS::Getattr(const char *path, struct stat *statbuf) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  //auto *context = fuse_get_context();
  std::string root_path = (m_root + "/" + path);
  auto r = lstat(root_path.c_str(), statbuf);
  if(r!=0) {
    r = -errno;
    LOG(0,"lstat('%s', statbuf) => %d\n", root_path.c_str(), errno);
    return r;
  }
  return 0;
}

int MetaFS::Readlink(const char *path, char *link, size_t size) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"readlink(path=%s, link=%s, size=%d)\n", path, link, (int)size);
  return -ENOENT;
}

/** \brief Handles creating FIFOs, regular files, etc...
*/
int MetaFS::Mknod(const char *path, mode_t mode, dev_t dev) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"mknod(path=%s, mode=%o)\n", path, mode);
  std::string root_path = (m_root + "/" + path);
  int r = mknod(root_path.c_str(), mode, dev);
  if (r != 0) {
    r = -errno;
    LOG(0,"mknod('%s') => %d\n", root_path.c_str(), errno);
    return r;
  }
  return 0;
}

/** \todo Decide what to do on \em original filesystem */
int MetaFS::Mkdir(const char *path, mode_t mode) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"mkdir(path=%s, mode=%o)\n", path, (int)mode);
  return -EPERM;
}

int MetaFS::Unlink(const char *path) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"unlink(path=%s)", path);
  std::string root_path = (m_root + "/" + path);
  int r = unlink(root_path.c_str());
  if (r == -1) {
    r = -errno;
    LOG(0,"unlink('%s') => %d\n", root_path.c_str(), errno);
    return r;
  }
  return 0;
}

int MetaFS::Rmdir(const char *path) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"rmdir(path=%s\n)", path);
  return -ENOENT;
}

int MetaFS::Symlink(const char *path, const char *link) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"symlink(path=%s, link=%s)\n", path, link);
  return -ENOENT;
}

int MetaFS::Rename(const char *src, const char *tgt) {
  LOG(0,"%s: '%s' '%s'\n", __PRETTY_FUNCTION__, src, tgt );
  return -ENOENT;
}

int MetaFS::Link(const char *path, const char *newpath) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"link(path=%s, newPath=%s)\n", path, newpath);
  return -ENOENT;
}

int MetaFS::Chmod(const char *path, mode_t mode) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"chmod(path=%s, mode=%o)\n", path, mode);
  return -ENOENT;
}

int MetaFS::Chown(const char *path, uid_t uid, gid_t gid) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"chown(path=%s, uid=%d, gid=%d)\n", path, (int)uid, (int)gid);
  return -ENOENT;
}

int MetaFS::Truncate(const char *path, off_t newSize) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"truncate(path=%s, newSize=%d\n", path, (int)newSize);
  std::string root_path = (m_root + "/" + path);
  int r = truncate(root_path.c_str(), newSize);
  if (r == -1) {
    int r = -errno;
    LOG(0,"truncate('%s') => %d\n", root_path.c_str(), errno);
    return r;
  }
  return 0;
}

int MetaFS::Utime(const char *path, struct utimbuf * /*ubuf*/) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  return -ENOENT;
}

int MetaFS::Open(const char *path, struct fuse_file_info *fileInfo) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  std::string root_path = (m_root + "/" + path);
  int fd = open(root_path.c_str(), fileInfo->flags);
  if (fd == -1) {
    int r = -errno;
    LOG(0,"open('%s') => %d\n", root_path.c_str(), errno);
    return r;
  }
  fileInfo->direct_io = 1;
  fileInfo->fh = (unsigned long) fd;
    
  LOG(0,"open('%s') == %d\n", root_path.c_str(), fd);
  return 0;
}

int MetaFS::Read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  int r = pread(fileInfo->fh, buf, size, offset);
  if(r == -1) {
    int r = -errno;
    LOG(0,"read('%d') => %d\n", fileInfo->fh, errno);
    return r;
  }
  return r;
}

int MetaFS::Write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  int r = pwrite(fileInfo->fh, buf, size, offset);
  if(r == -1) {
    int r = -errno;
    LOG(0,"write('%d') => %d\n", fileInfo->fh, errno);
    return r;
  }
  return r;
}

int MetaFS::Statfs(const char *path, struct statvfs *statInfo) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"statfs(path=%s)\n", path);
  RETURN_ERRNO(statvfs(m_root.c_str(), statInfo));
  //return -ENOENT;
}

int MetaFS::Flush(const char *path, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  //noop because we don't maintain our own buffers
  //LOG(0,"flush(path=%s)\n", path);
  return 0;
}

int MetaFS::Release(const char *path, struct fuse_file_info *fileInfo) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"close(fh=%d)\n", fileInfo->fh);
  int r = close(fileInfo->fh);
  if (r == -1) {
    int r = -errno;
    LOG(0,"release('%d') => %d\n", fileInfo->fh, errno);
    return r;
  }
  return 0;
}

int MetaFS::Fsync(const char *path, int /*datasync*/, struct fuse_file_info * /*fi*/) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  return -ENOENT;
}

int MetaFS::Setxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"setxattr(path=%s, name=%s, value=%s, size=%d, flags=%d\n", path, name, value, (int)size, flags);
  return -ENOENT;
}

int MetaFS::Getxattr(const char *path, const char *name, char * /*value*/, size_t size) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"getxattr(path=%s, name=%s, size=%d\n", path, name, (int)size);
  return -ENOENT;
}

int MetaFS::Listxattr(const char *path, char * /*list*/, size_t size) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"listxattr(path=%s, size=%d)\n", path, (int)size);
  return -ENOENT;
}

int MetaFS::Removexattr(const char *path, const char *name) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"removexattry(path=%s, name=%s)\n", path, name);
  return -ENOENT;
}

int MetaFS::Opendir(const char *path, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  if(strcmp(path, "/")==0 || strcmp(path, "/test")==0) {
    return 0;
  }
  return -ENOENT;
}

int MetaFS::Readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s' %zu\n", __PRETTY_FUNCTION__, path, offset );

  DIR *dp = opendir((m_root + "/" + path).c_str());
  if(dp) {
    struct dirent *de;
    while ((de = readdir(dp)) != NULL) {
      if (filler(buf, de->d_name, NULL, 0)) break;
    }
    closedir(dp);
  }
  return 0;
}

int MetaFS::Releasedir(const char *path, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  return 0;
}

int MetaFS::Fsyncdir(const char *path, int /*datasync*/, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  return 0;
}

void *MetaFS::Init(struct fuse_conn_info * /*conn*/) {
  LOG(0,"%s\n", __PRETTY_FUNCTION__ );
  LOG(0,"init\n");
  return _instance;
}

int MetaFS::Truncate(const char *path, off_t offset, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"truncate(path=%s, offset=%zu)", path, offset);
  return -ENOENT;
}

