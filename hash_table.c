#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "hash_table.h"
#include "prime.h"

const int HT_INITIAL_BASE_SIZE = 53;
const int HT_PRIME_1 = 13;
const int HT_PRIME_2 = 17;
static ht_item HT_DELETED_ITEM = {NULL, NULL};

// ht_item *HT_DELETED_ITEM = ht_new_item('delete', 'delete');

// initialize a new key-value pair
static ht_item *ht_new_item(char *k, char *v)
{
  ht_item *i = malloc(sizeof(ht_item));
  i->key = k;
  i->value = v;
  return i;
}

// resizing function
static ht_hash_table *ht_new_sized(const int base_size)
{
  ht_hash_table *ht = malloc(sizeof(ht_hash_table));
  ht->base_size = base_size;
  ht->size = next_prime(ht->base_size);

  ht->count = 0;
  ht->items = calloc((size_t)ht->size, sizeof(ht_item *));
  return ht;
}

// initialize a new hash table, use calloc to fill the allocated memory to NULL, an entry of NULL indicates the bucket is empty
ht_hash_table *ht_new()
{
  return ht_new_sized(HT_INITIAL_BASE_SIZE);
}

// del key-array
static void ht_del_item(ht_item *i)
{
  free(i->key);
  free(i->value);
  free(i);
}

// del hash table
void ht_del_hash_table(ht_hash_table *ht)
{
  for (int i = 0; i < ht->size; i++)
  {
    ht_item *item = ht->items[i];
    if (item != NULL)
    {
      ht_del_item(item);
    }
  }
  free(ht->items);
  free(ht);
}

// hash function
static int ht_hash(const char *s, const int a, const int m)
{
  long hash = 0;
  const int len_s = strlen(s);
  for (int i = 0; i < len_s; i++)
  {
    hash += (long)pow(a, len_s - i - 1) * s[i];
    hash = hash % m;
  }
  return (int)hash;
}

// double hashing
static int ht_get_hash(const char *s, const int num_bucket, const int attempt)
{
  const int hash_a = ht_hash(s, HT_PRIME_1, num_bucket);
  const int hash_b = ht_hash(s, HT_PRIME_2, num_bucket);
  return (hash_a + (attempt * (hash_b + 1)) % num_bucket);
}

static void ht_resize(ht_hash_table *ht, const int base_size)
{
  //if the base size is smaller than the HT_INITIAL_BASE_SIZE, dont resize
  if (base_size < HT_INITIAL_BASE_SIZE)
  {
    return;
  }
  ht_hash_table *new_ht = ht_new_sized(base_size);
  for (int i = 0; i < ht->size; i++)
  {
    ht_item *item = ht->items[i];
    if (item != NULL && item != &HT_DELETED_ITEM)
    {
      ht_insert(new_ht, item->key, item->value);
    }
  }
  ht->base_size = new_ht->base_size;
  ht->count = new_ht->count;

  const int tmp_size = ht->size;
  ht->size = new_ht->size;
  new_ht->size = tmp_size;

  ht_item **tmp_items = ht->items;
  ht->items = new_ht->items;
  new_ht->items = tmp_items;

  ht_del_hash_table(new_ht);
}

static void ht_resize_up(ht_hash_table *ht)
{
  const int new_size = ht->base_size * 2;
  ht_resize(ht, new_size);
}

static void ht_resize_down(ht_hash_table *ht)
{
  const int new_size = ht->base_size / 2;
  ht_resize(ht, new_size);
}

// insert function
void ht_insert(ht_hash_table *ht, char *key, char *value)
{
  const int load = ht->count * 100 / ht->size;
  if (load > 70)
  {
    ht_resize_up(ht);
  }
  ht_item *item = ht_new_item(key, value);
  int index = ht_get_hash(item->key, ht->size, 0);
  ht_item *cur_item = ht->items[index];
  int i = 1;
  while (cur_item != NULL)
  {
    if (cur_item != &HT_DELETED_ITEM)
    {
      index = ht_get_hash(item->key, ht->size, i);
      i++;
      cur_item = ht->items[index];
    }
  }
  ht->items[index] = item;
  ht->count++;
}

// search function
char *ht_search(ht_hash_table *ht, const char *key)
{
  int index = ht_get_hash(key, ht->size, 0);
  ht_item *cur_item = ht->items[index];
  int i = 1;
  while (cur_item != NULL)
  {
    if (cur_item != &HT_DELETED_ITEM)
    {
      if (strcmp(cur_item->key, key) == 0)
      {
        return cur_item->value;
      }
      index = ht_get_hash(key, ht->size, i);
      i++;
      cur_item = ht->items[index];
    }
  }

  return NULL;
}

// delete function
void ht_delete(ht_hash_table *ht, const char *key)
{
  const int load = ht->count * 100 / ht->size;
  if (load < 10)
  {
    ht_resize_down(ht);
  }
  int index = ht_get_hash(key, ht->size, 0);
  ht_item *cur_item = ht->items[index];
  int i = 1;
  while (cur_item != NULL)
  {
    if (cur_item != &HT_DELETED_ITEM)
    {
      if (strcmp(cur_item->key, key) == 0)
      {
        ht_del_item(cur_item);
        ht->items[index] = &HT_DELETED_ITEM;
        ht->count--;
        return;
      }
    }
    index = ht_get_hash(key, ht->size, i);
    cur_item = ht->items[index];
    i++;
  }
  return;
}