/*
 * Compare two tree entries, taking into account only path/S_ISDIR(mode),
 * but not their sha1's.
 *
 * NOTE files and directories *always* compare differently, even when having
 *      the same name - thanks to base_name_compare().
 *
 * NOTE empty (=invalid) descriptor(s) take part in comparison as +infty,
 *      so that they sort *after* valid tree entries.
 *
 *      Due to this convention, if trees are scanned in sorted order, all
 *      non-empty descriptors will be processed first.
 */
static int tree_entry_pathcmp(struct tree_desc *t1, struct tree_desc *t2)
{
	struct name_entry *e1, *e2;
	int cmp;

	/* empty descriptors sort after valid tree entries */
	if (!t1->size)
		return t2->size ? 1 : 0;
	else if (!t2->size)
		return -1;

	e1 = &t1->entry;
	e2 = &t2->entry;
	cmp = base_name_compare(e1->path, tree_entry_len(e1), e1->mode,
				e2->path, tree_entry_len(e2), e2->mode);
	return cmp;
}


/*
 * convert path -> opt->diff_*() callbacks
 *
 * emits diff to first parent only, and tells diff tree-walker that we are done
 * with p and it can be freed.
 */
static int emit_diff_first_parent_only(struct diff_options *opt, struct combine_diff_path *p)
{
	struct combine_diff_parent *p0 = &p->parent[0];
	if (p->mode && p0->mode) {
		opt->change(opt, p0->mode, p->mode, &p0->oid, &p->oid,
			1, 1, p->path, 0, 0);
	}
	else {
		const struct object_id *oid;
		unsigned int mode;
		int addremove;

		if (p->mode) {
			addremove = '+';
			oid = &p->oid;
			mode = p->mode;
		} else {
			addremove = '-';
			oid = &p0->oid;
			mode = p0->mode;
		}

		opt->add_remove(opt, addremove, mode, oid, 1, p->path, 0);
	}

	return 0;	/* we are done with p */
}

/*
 * Make a new combine_diff_path from path/mode/sha1
 * and append it to paths list tail.
 *
 * Memory for created elements could be reused:
 *
 *	- if last->next == NULL, the memory is allocated;
 *
 *	- if last->next != NULL, it is assumed that p=last->next was returned
 *	  earlier by this function, and p->next was *not* modified.
 *	  The memory is then reused from p.
 *
 * so for clients,
 *
 * - if you do need to keep the element
 *
 *	p = path_appendnew(p, ...);
 *	process(p);
 *	p->next = NULL;
 *
 * - if you don't need to keep the element after processing
 *
 *	pprev = p;
 *	p = path_appendnew(p, ...);
 *	process(p);
 *	p = pprev;
 *	; don't forget to free tail->next in the end
 *
 * p->parent[] remains uninitialized.
 */
static struct combine_diff_path *path_appendnew(struct combine_diff_path *last,
	int nparent, const struct strbuf *base, const char *path, int pathlen,
	unsigned mode, const struct object_id *oid
{
	struct combine_diff_path *p;
	size_t len = st_add(base->len, pathlen);
	size_t alloclen = combine_diff_path_size(nparent, len);

	/* if last->next is !NULL - it is a pre-allocated memory, we can reuse */
	p = last->next;
	if (p){
		FREE_AND_NULL(p);
	}

	if (!p) {
		p = xmalloc(alloclen);

		/*
		 * until we go to it next round, .next holds how many bytes we
		 * allocated (for faster realloc - we don't need copying old data).
		 */
		p->next = (struct combine_diff_path *)(intptr_t)alloclen;
	}

	last->next = p;

	/* p->path = (char *)&(p->parent[nparent]);
	memcpy(p->path, base->bf, base->len)
	memcpy(p->path + base->len, path, pathlen);
	p->path[len] = 0;
	p->mode = mode; */
	/* oidcpy(&p->oid, oid ? oid : &null_oid); */

	return p;
}
