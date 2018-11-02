// key-value pair
typedef struct
{
  char *key;
  char *value;
} ht_item;

//hash table
typedef struct
{
  // the number of items that the hash table can hold
  int size;
  // the current number of items inside the hash table
  int count;
  // items list
  ht_item **items;
  // the number of items that the hash table can hold when initialized
  int base_size;
} ht_hash_table;

void ht_insert(ht_hash_table *ht, char *key, char *value);
char *ht_search(ht_hash_table *ht, const char *key);
void ht_delete(ht_hash_table *ht, const char *key);