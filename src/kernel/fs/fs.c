#include "../../types.h" /* kernel types */
#include "../../include/unistd.h" /* unistd io functions */
#include "../panic.h" /* panic the kernel (kernel the panic) */
#include "../heap.h" /* kernel heap memory */
#include "../../include/string.h" /* string functions */
#include "fs.h" /* header */

struct fs_node *fs_root = NULL; /* root filesystem node */

/* allocate node */
static struct fs_node *fs_alloc(void) {

	/* allocate node */
	struct fs_node *f = (struct fs_node *)kmalloc(sizeof(struct fs_node));
	
	if (f == NULL)
		return NULL;
	
	/* set all values to 0 and return */
	memset((void *)f, 0, sizeof(struct fs_node));
	
	return f;
}

/* intialize filesystem */
extern int fs_init(void) {

	/* create root fs node ("/") */
	fs_root = fs_alloc();
	
	/* panic */
	if (fs_root == NULL)
		kpanic(E_NOFS, "failed to mount vfs\n");
	
	/* set values */
	strcpy(fs_root->name, "/");
	fs_root->mask = 755;
	fs_root->flags = FS_DIR;
	fs_root->uid = 0;
	fs_root->gid = 0;
}
