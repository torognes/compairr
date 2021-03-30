/*
    Copyright (C) 2012-2020 Torbjorn Rognes and Frederic Mahe

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact: Torbjorn Rognes <torognes@ifi.uio.no>,
    Department of Informatics, University of Oslo,
    PO Box 1080 Blindern, NO-0316 Oslo, Norway
*/

#include "vdjsearch.h"

#define HASHFILLPCT 70

void hash_zap(struct hashtable_s * ht)
{
  memset(ht->hash_occupied, 0, (ht->hash_tablesize + 63) / 8);
}

struct hashtable_s * hash_init(uint64_t sequences)
{
  struct hashtable_s * ht = (struct hashtable_s *)
    xmalloc(sizeof(struct hashtable_s));

  ht->hash_tablesize = 1;
    while (100 * sequences > HASHFILLPCT * ht->hash_tablesize)
    ht->hash_tablesize <<= 1;

  ht->hash_mask = ht->hash_tablesize - 1;

  ht->hash_occupied = static_cast<unsigned char *>
    (xmalloc((ht->hash_tablesize + 63) / 8));

  hash_zap(ht);

  ht->hash_values = static_cast<uint64_t *>
    (xmalloc(ht->hash_tablesize * sizeof(uint64_t)));

  ht->hash_data = static_cast<unsigned int *>
    (xmalloc(ht->hash_tablesize * sizeof(unsigned int)));

  return ht;
}

void hash_exit(struct hashtable_s * ht)
{
  xfree(ht->hash_occupied);
  xfree(ht->hash_values);
  xfree(ht->hash_data);
  xfree(ht);
}
