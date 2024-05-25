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

/* read from a directory listing */
extern struct dirent *fs_readdir(struct fs_node *n, u32 i) {

	while (n->ptr != NULL) n = n->ptr;

	/* get readdir */
	if (n->fs != NULL && n->fs->rd != NULL) return n->fs->rd(n, i);
	return NULL;
}

/* find a directory */
extern struct fs_node *fs_finddir(struct fs_node *n, char *s) {

	while (n->ptr != NULL) n = n->ptr;

	/* get finddir */
	if (n->fs != NULL && n->fs->fd != NULL) return n->fs->fd(n, s);
	return NULL;
}

/* find a directory with a file path */
extern struct fs_node *fs_finddir_path(struct fs_node *n, char *s) {

	char *p = s;
	char *nx = NULL;
	u32 l = 0;
	struct fs_node *np = n;
	while (p != NULL) {

		while (np->ptr != NULL) np = np->ptr;

		if (p != s) p++; /* pass '/' char */
		nx = strchr(p, '/');
		l = nx != NULL? (u32)(nx - p): strlen(p);

		memcpy(_rand_buf, p, l);
		_rand_buf[l] = 0;

		/* search for directory entry */
		if (np->first != NULL) {

			struct dirent *d = np->first;
			while (d != NULL) {

				if (kstreq(d->name, _rand_buf) && d->node != NULL) {

					np = d->node;
					break;
				}

				d = d->next;
			}
			if (d == NULL) return NULL;
		}
		else if (FS_NODE_TYPE(np) == FS_DIR) {

			struct dirent *d = fs_readdir(np, 0);
			
			if (d != NULL) np->first = d;
			int i = 1;
			while (d != NULL) {

				struct dirent *t = fs_readdir(np, i);
				d->next = t;
				d = t;
				i++;
			}

			/* get file nodes (the lazy way) */
			d = np->first;
			struct fs_node *_np = NULL;
			for (int j = 0; j < i; j++) {

				d->node = fs_finddir(np, d->name);
				//if (kstreq(d->name, _rand_buf))
				//	_np = d->node;
				d = d->next;
			}
			//if (_np == NULL) return NULL;
			//np = _np;
		}

		p = nx;
	}

	return np;
}
