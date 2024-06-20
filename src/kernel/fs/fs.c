#include "../../types.h" /* kernel types */
#include "../../include/unistd.h" /* unistd io functions */
#include "../panic.h" /* panic the kernel (kernel the panic) */
#include "../heap.h" /* kernel heap memory */
#include "../../include/string.h" /* string functions */
#include "../kprint.h" /* debug */
#include "fs.h" /* header */

static char _rand_buf[128]; /* random buffer */

struct fs_node *fs_root = NULL; /* root filesystem node */

/* allocate dirent */
extern struct dirent *fs_dirent_alloc(void) {

	struct dirent *d = (struct dirent *)kmalloc(sizeof(struct dirent));

	if (d == NULL) return NULL;

	memset((void *)d, 0, sizeof(struct dirent));
	return d;
}

/* allocate node */
extern struct fs_node *fs_alloc(struct fs_node *p) {

	/* allocate node */
	struct fs_node *f = (struct fs_node *)kmalloc(sizeof(struct fs_node));
	
	if (f == NULL)
		return NULL;
	
	/* set values */
	memset((void *)f, 0, sizeof(struct fs_node));
	if (p != NULL) {

		f->dev = p->dev;
		f->part = p->part;
		f->fs = p->fs;
		f->parent = p;
	}
	
	return f;
}

/* intialize filesystem */
extern int fs_init(void) {

	/* create root fs node ("/") */
	fs_root = fs_alloc(NULL);
	
	/* panic */
	if (fs_root == NULL)
		kpanic(E_NOFS, "failed to mount vfs\n");
	
	/* set values */
	strcpy(fs_root->name, "/");
	fs_root->flags = FS_MNTP;
	fs_root->mask = 755;
	fs_root->uid = 0;
	fs_root->gid = 0;
}

/* read file */
extern u32 fs_read(struct fs_file *fp, u32 c, void *buf) {

	if (fp->n->fs != NULL && fp->n->fs->r != NULL) {
		
		u32 r = fp->n->fs->r(fp, c, buf);
		fp->idx += r;
		return r;
	}
	return 0;
}

/* write file */
extern u32 fs_write(struct fs_file *fp, u32 c, void *buf) {

	if (fp->n->fs != NULL && fp->n->fs->w != NULL) {

		u32 w = fp->n->fs->w(fp, c, buf);
		fp->idx += w;
		return w;
	}
	return 0;
}

/* open file */
extern struct fs_file *fs_open(struct fs_node *n, u32 m) {

	struct fs_file *fp = (struct fs_file *)kmalloc(sizeof(struct fs_file));

	/* set values */
	fp->n = n;
	fp->m = m;
fp->idx = 0;
	fp->d = NULL;

	/* callback */
	if (n->mask & FS_TYPE != FS_DIR && n->fs != NULL && n->fs->o != NULL) n->fs->o(fp);
	return fp;
}

/* close file */
extern void fs_close(struct fs_file *fp) {

	if (fp->n->mask & FS_TYPE != FS_DIR && fp->n->fs != NULL && fp->n->fs->c != NULL) fp->n->fs->c(fp);
	kfree(fp);
}

/* fill directory */
extern void fs_filldir(struct fs_node *n) {

	while (n->ptr != NULL) n = n->ptr;

	/* get filldir */
	if (n->fs != NULL && n->fs->fd != NULL) n->fs->fd(n);
	n->impl = 1;
}

/* create file */
extern void fs_create(struct fs_node *n, char *fn, u32 tp) {

	while (n->ptr != NULL) n = n->ptr;

	/* create dirent */
	struct dirent *dent = fs_dirent_alloc();
	struct fs_node *ch = fs_alloc(n);
	dent->node = ch;

	/* get create */
	if (n->fs != NULL && n->fs->cr != NULL) n->fs->cr(n, fn, tp, dent);

	/* copy info */
	strcpy(dent->name, fn);
	strcpy(dent->node->name, fn);
	dent->next = NULL;
	dent->node->mask = tp;
}

/* read from a directory listing */
extern struct dirent *fs_readdir(struct fs_node *n, u32 i) {

	while (n->ptr != NULL) n = n->ptr;
	if (n->mask & FS_TYPE != FS_DIR) return NULL;

	/* fill directory */
	if (!n->first && !n->impl) fs_filldir(n);

	/* loop through dirents */
struct dirent *dent = n->first;
	for (int j = 0; j < i; j++) {

		dent = dent->next;
		if (!dent) break;
	}
	
	/* return */
	return dent;
}

/* find a directory */
extern struct fs_node *fs_finddir(struct fs_node *n, char *s) {

	while (n->ptr != NULL) n = n->ptr;
	if (n->mask & FS_TYPE != FS_DIR) return NULL;

	/* fill directory */
	if (!n->first && !n->impl) fs_filldir(n);

	/* loop through dirents */
	struct dirent *dent = n->first;
	while (dent != NULL && !kstreq(dent->name, s))
		dent = dent->next;

	/* found */
	if (dent) return dent->node;
	return NULL;
}

/* find in directory with path */
extern struct fs_node *fs_finddir_path(struct fs_node *n, char *s) {

	if (s[0] == 0) {

		struct fs_node *np = n;
		while (np->ptr != NULL) np = np->ptr;
		return np;
	}

	char *p = s;
	char *nx = NULL;
	size_t l = 0;
	struct fs_node *np = n;
	while (p != NULL && np != NULL) {

		while (np->ptr != NULL) np = np->ptr;

		/* isolate file name */
		while (p[0] == '/') p++;
		nx = strchr(p, '/');
		l = nx != NULL? (u32)(nx - p): strlen(p);

		memcpy(_rand_buf, p, l);
		_rand_buf[l] = 0;

		/* find entry */
		np = fs_finddir(np, _rand_buf);
		p = nx;
	}
	return np;
}
