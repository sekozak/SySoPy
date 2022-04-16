extern void** create_table(char *argv);
extern struct memoryBlock* wc_files(int argc, char *argv[]);
extern void remove_block(void** tab, char *argv);
extern int pointerToData(void** tab, int size, struct memoryBlock *block);