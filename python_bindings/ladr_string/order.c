#include <stdlib.h>

typedef enum { NOT_COMPARABLE,
              SAME_AS,
              LESS_THAN,
              GREATER_THAN,
              LESS_THAN_OR_SAME_AS,
              GREATER_THAN_OR_SAME_AS,
              NOT_LESS_THAN,
              NOT_GREATER_THAN
} Ordertype;

void merge_sort_recurse(void *a[],    /* array to sort */
                       void *w[],    /* work array */
                       int start,    /* index of first element */
                       int end,      /* index of last element */
                       Ordertype (*comp_proc) (void *, void *))
{
  int mid, i, i1, i2, e1, e2;

  if (start < end) {
    mid = (start+end)/2;
    merge_sort_recurse(a, w, start, mid, comp_proc);
    merge_sort_recurse(a, w, mid+1, end, comp_proc);
    i1 = start; e1 = mid;
    i2 = mid+1; e2 = end;
    i = start;
    while (i1 <= e1 && i2 <= e2) {
      if ((*comp_proc)(a[i1], a[i2]) == GREATER_THAN)
        w[i++] = a[i2++];
      else
        w[i++] = a[i1++];
    }

    if (i2 > e2)
      while (i1 <= e1)
        w[i++] = a[i1++];
    else
      while (i2 <= e2)
        w[i++] = a[i2++];

    for (i = start; i <= end; i++)
      a[i] = w[i];
  }
}

void merge_sort(void *a[],   /* array to sort */
                int n,       /* size of array */
                Ordertype (*comp_proc) (void *, void *))
{
  void **work = malloc(n * sizeof(void *));
  merge_sort_recurse(a, work, 0, n-1, comp_proc);
  free(work);
}

Ordertype compare_vecs(int *a, int *b, int n)
{
  int i;
  for (i = 0; i < n; i++) {
    if (a[i] < b[i])
      return LESS_THAN;
    else if (a[i] > b[i])
      return GREATER_THAN;
  }
  return SAME_AS;
}

void copy_vec(int *a, int *b, int n)
{
  int i;
  for (i = 0; i < n; i++)
    b[i] = a[i];
} 