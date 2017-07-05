#include "wrap.hh"
#include <fuse.h>
#include <stdio.h>
#include <stddef.h>

struct fuse_operations metafs_oper;

/** options for fuse_opt.h */
struct metafs_options {
  char* root;
};

struct metafs_options options;

/** macro to define options */
#define TAGFS_OPT_KEY(t, p, v) { t, offsetof(struct metafs_options, p), v }

/** keys for FUSE_OPT_ options */
enum
{
  KEY_VERSION,
  KEY_HELP,
};

static struct fuse_opt metafs_opts[] =
{
  TAGFS_OPT_KEY("--source %s", root, 0),
  TAGFS_OPT_KEY("source=%s", root, 0),

  // #define FUSE_OPT_KEY(templ, key) { templ, -1U, key }
  FUSE_OPT_KEY("-V",             KEY_VERSION),
  FUSE_OPT_KEY("--version",      KEY_VERSION),
  FUSE_OPT_KEY("-h",             KEY_HELP),
  FUSE_OPT_KEY("--help",         KEY_HELP),
  FUSE_OPT_END
};

void setup() {
  metafs_oper.getattr = wrap_getattr;
  metafs_oper.readlink = wrap_readlink;
  metafs_oper.getdir = NULL;
  metafs_oper.mknod = wrap_mknod;
  metafs_oper.mkdir = wrap_mkdir;
  metafs_oper.unlink = wrap_unlink;
  metafs_oper.rmdir = wrap_rmdir;
  metafs_oper.symlink = wrap_symlink;
  metafs_oper.rename = wrap_rename;
  metafs_oper.link = wrap_link;
  metafs_oper.chmod = wrap_chmod;
  metafs_oper.chown = wrap_chown;
  metafs_oper.truncate = wrap_truncate;
  metafs_oper.utime = wrap_utime;
  metafs_oper.open = wrap_open;
  metafs_oper.read = wrap_read;
  metafs_oper.write = wrap_write;
  metafs_oper.statfs = wrap_statfs;
  metafs_oper.flush = wrap_flush;
  metafs_oper.release = wrap_release;
  metafs_oper.fsync = wrap_fsync;
#if 0
  metafs_oper.setxattr = wrap_setxattr;
  metafs_oper.getxattr = wrap_getxattr;
  metafs_oper.listxattr = wrap_listxattr;
  metafs_oper.removexattr = wrap_removexattr;
#else
  metafs_oper.setxattr = NULL;
  metafs_oper.getxattr = NULL;
  metafs_oper.listxattr = NULL;
  metafs_oper.removexattr = NULL;
#endif
  metafs_oper.opendir = wrap_opendir;
  metafs_oper.readdir = wrap_readdir;
  metafs_oper.releasedir = wrap_releasedir;
  metafs_oper.fsyncdir = wrap_fsyncdir;
  metafs_oper.init = wrap_init;
  metafs_oper.destroy = wrap_destroy;
}

int main(int argc, char *argv[]) {
  int ret;
  struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

  /* clear structure that holds our options */
  memset(&options, 0, sizeof(struct metafs_options));

  if (fuse_opt_parse(&args, &options, metafs_opts, NULL) == -1) {
    /** error parsing options */
    return -1;
  }

  if(options.root==NULL) {
    fprintf(stderr, "ERROR: No source provided. Aborting.");
    exit(-1);
  }

  setup();
  set_rootdir(options.root);
  scan_rootdir();
  printf("mounting file system...\n");

  ret = fuse_main(args.argc, args.argv, &metafs_oper, NULL);

  if (ret) {
    printf("\n");
  }

  /** free arguments */
  fuse_opt_free_args(&args);

  return ret;
}


