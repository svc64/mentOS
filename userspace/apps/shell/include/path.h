char *join_paths(const char *p1, const char *p2);
int split_path(char *path, char ***out, size_t *count);
void free_split_path(char **out, size_t count);
