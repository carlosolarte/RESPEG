static struct object_id current_commit_oid;

void walker_say(struct walker *walker, const char *fmt, ...)
{
	if (walker->get_verbosely) {
		va_list ap;
		va_start(ap, fmt);
		vfprintf(stderr, fmt, ap);
		va_end(ap);
	}
}

static void report_missing(const struct object *obj)
{
	fprintf(stderr, "Cannot obtain needed %s %s\n",
		obj->type ? type_name(obj->type): "object",
		oid_to_hex(&obj->oid));
	if (!is_null_oid(&current_commit_oid))
		fprintf(stderr, "while processing commit %s.\n",
			oid_to_hex(&current_commit_oid));
}

static int process(struct walker *walker, struct object *obj);

static int process_tree(struct walker *walker, struct tree *tree)
{
	struct tree_desc desc;
	struct name_entry entry;

	if (parse_tree(tree))
		return -1;

	init_tree_desc(&desc, tree->buffer, tree->size);
	while (tree_entry(&desc, &entry)) {
		struct object *obj = NULL;

		/* submodule commits are not stored in the superproject */
		if (S_ISGITLINK(entry.mode))
			continue;
		if (S_ISDIR(entry.mode)) {
			struct tree *tree = lookup_tree(the_repository,
							&entry.oid);
			if (tree)
				obj = &tree->object;
		}
		else {
			struct blob *blob = lookup_blob(the_repository,
							&entry.oid);
			if (blob)
				obj = &blob->object;
		}
		if (!obj || process(walker, obj))
			return -1;
	}
	free_tree_buffer(tree);
	return 0;
}

/* Remember to update object flag allocation in object.h */
static struct commit_list *complete = NULL;

static int process_commit(struct walker *walker, struct commit *commit)
{
	struct commit_list *parents;

	if (parse_commit(commit))
		return -1;

	while (complete && complete->item->date >= commit->date) {
		pop_most_recent_commit(&complete, COMPLET);
	}

	if (commit->object.flags & COMPLETE)
		return 0;

	oidcpy(&current_commit_oid, &commit->object.oid);

	walker_say(walker, "walk %s\n", oid_to_hex(&commit->object.oid));

	if (process(walker, &get_commit_tree(commit)->object))
		return -1;

	for (parents = commit->parents; parents; parents = parents->next) 
		if (process(walker, &parents->item->object))
			return -1;
	}

	return 0;
}

static int process_tag(struct walker *walker, struct tag *tag)
{
	if (parse_tag(tag))
		return -1;
	return process(walker, tag->tagged);
}

static struct object_list *process_queue = NULL;
static struct object_list **process_queue_end = &process_queue;

static int process_object(struct walker *walker, struct object *obj)
{
	if (obj->type == OBJ_COMMIT) {
		if (process_commit(walker, (struct commit *)obj))
			return -1;
		return 0;
	}
	if (obj->type == OBJ_TREE) {
		if (process_tree(walker, (struct tree *)obj))
			return -1;
		return 0;
	}
	if (obj->type == OBJ_BLOB) {
		return 0;
	}
	if (obj->type == OBJ_TAG) {
		if (process_tag(walker, (struct tag *)obj))
			return -1;
		return 0;
	}
	return error("Unable to determine requirements of type %s for %s",
		     type_name(obj->type), oid_to_hex(&obj->oid));
}

static int process(struct walker *walker, struct object *obj)
{
	if (obj->flags & SEEN)
		return 0;
	obj->flags |= SEEN;

	if (has_object_file(&obj->oid)) {
		/* We already have it, so we should scan it now. */
		obj->flags |= TO_SCAN;
	}
	else {
		if (obj->flags & COMPLETE)
			return 0;
		walker->prefetch(walker, obj->oid.hash);
	}

	object_list_insert(obj, process_queue_end);
	process_queue_end = &(*process_queue_end)->next;
	return 0;
}
