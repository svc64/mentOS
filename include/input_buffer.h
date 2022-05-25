struct input_buffer *input_buffer_new(void);
void input_buffer_push(struct input_buffer *ibuf, char c);
char input_buffer_pop(struct input_buffer *ibuf);
char input_buffer_last(struct input_buffer *ibuf);
void input_buffer_remove(struct input_buffer *ibuf);
void input_buffer_free(struct input_buffer *ibuf);
