int tree_from_index(Index *index, ObjectID *out) {
    if (!index || index->count == 0) return -1;

    Tree root = {0};

    for (size_t i = 0; i < index->count; i++) {
        IndexEntry *e = &index->entries[i];

        TreeEntry entry = {0};
        entry.mode = e->mode;
        entry.id = e->id;
        strncpy(entry.name, e->path, sizeof(entry.name));

        root.entries = realloc(root.entries, sizeof(TreeEntry) * (root.count + 1));
        root.entries[root.count++] = entry;
    }

    // serialize tree
    size_t buf_size = 0;
    for (size_t i = 0; i < root.count; i++) {
        buf_size += 100; // rough size
    }

    char *buffer = malloc(buf_size);
    if (!buffer) return -1;

    size_t offset = 0;

    for (size_t i = 0; i < root.count; i++) {
        TreeEntry *e = &root.entries[i];

        offset += sprintf(buffer + offset, "%06o %s ",
                          e->mode,
                          e->name);

        memcpy(buffer + offset, e->id.hash, HASH_SIZE);
        offset += HASH_SIZE;
    }

    int res = object_write(OBJ_TREE, buffer, offset, out);

    free(buffer);
    free(root.entries);

    return res;
}
