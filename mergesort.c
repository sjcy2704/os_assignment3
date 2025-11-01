/**
 * This file implements parallel mergesort.
 */

#include "mergesort.h"
#include <stdio.h>
#include <stdlib.h> /* for malloc */
#include <string.h> /* for memcpy */

/* this function will be called by mergesort() and also by parallel_mergesort().
 */
void merge(int leftstart, int leftend, int rightstart, int rightend) {
  if (leftstart > leftend || rightstart > rightend)
    return;

  int totalSize = rightend - leftstart + 1;
  // copy range of elements to be merged 
  memcpy(B + leftstart, A + leftstart, totalSize * sizeof(int));


  // initialize pointers (as in tracking the index) for the left array, right array, and the merged array
  int left = leftstart;
  int right = rightstart;
  int merged = leftstart;

  // merge while both subarrays have elements
  while (left <= leftend && right <= rightend) {
    A[merged++] = (B[left] <= B[right]) ? B[left++] : B[right++];
  }

  // copy remaining elements from the left array
  while (left <= leftend) {
    A[merged++] = B[left++];
  }

  // copy remaining elements for the right array
  while (right <= rightend) {
    A[merged++] = B[right++];
  }
}

/* this function will be called by parallel_mergesort() as its base case. */
void my_mergesort(int left, int right) {
  if (left >= right)
    return;

  int mid = left + (right - left) / 2;
  my_mergesort(left, mid);
  my_mergesort(mid + 1, right);
  merge(left, mid, mid + 1, right);
}

/* this function will be called by the testing program. */
void *parallel_mergesort(void *arg) {
  // casting arg with struct argument as arg is void
  struct argument *a = (struct argument *)arg;

  int left = a->left;
  int right = a->right;
  int level = a->level;

  // base case
  if (left >= right) {
    return NULL;
  }

  // once reach the cutoff threshold, stop creating threads and use sequential sort
  if (level >= cutoff) {
    my_mergesort(left, right);
    return NULL;
  }

  int mid = left + (right - left) / 2;

  // create arguments for the children
  struct argument *leftArg = buildArgs(left, mid, level + 1);
  struct argument *rightArg = buildArgs(mid + 1, right, level + 1);

  pthread_t leftThread, rightThread;

  // create new threads for each childrean
  pthread_create(&leftThread, NULL, parallel_mergesort, leftArg);
  pthread_create(&rightThread, NULL, parallel_mergesort, rightArg);

  // wait for all threads to finish before merging
  pthread_join(leftThread, NULL);
  pthread_join(rightThread, NULL);

  merge(left, mid, mid + 1, right);

  free(leftArg);
  free(rightArg);

  return NULL;
}

/* we build the argument for the parallel_mergesort function. */
struct argument *buildArgs(int left, int right, int level) {
  struct argument *arg = malloc(sizeof(struct argument));
  arg->left = left;
  arg->right = right;
  arg->level = level;
  return arg;
}
