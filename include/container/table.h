typedef struct table_s table;

typedef long unsigned int hash;

typedef hash (*hashfunct)(void *);
typedef int  (*keyeqfunct)(void *, void *);


table *table_init(hashfunct hf, keyeqfunct eqf);

int table_insert(table *t, void *key, void *value);

int table_delete(table *t, void *key);

void *table_get(table *t, void *h);
