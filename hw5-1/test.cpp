#include <algorithm>
#include <functional>
#include <iostream>
#include <numeric>
#include <omp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#define N_THREAD 16

typedef struct {
  int id;
  int p;
  int r;
  int w;
} task_t;

int schedule(task_t *tasks, int n) {
  int max_r = std::max_element(tasks, tasks + n, [](task_t a, task_t b) {
                return a.r < b.r;
              })->r;
  int sum_p = std::accumulate(tasks, tasks + n, 0,
                              [](int a, task_t b) { return a + b.p; });
  int cost = 0;
  int schedule[max_r + sum_p];

  std::fill(schedule, schedule + max_r + sum_p, -1);

  for (int i = 0; i < n; i++) {
    int p = tasks[i].p;
    int r = tasks[i].r;
    while (p) {
      if (schedule[r] == -1) {
        schedule[r] = tasks[i].id;
        p--;
      }
      r++;
    }
    if (p == 0)
      cost += r * tasks[i].w;
  }

  return cost;
}

int main(int argc, char *argv[]) {
  int n;
  int p, r, w;

  char file_name[128];
  scanf("%s", file_name);
  FILE *file = fopen(file_name, "r");
  fscanf(file, "%d", &n);
  std::vector<task_t> tasks;
  for (int i = 0; i < n; i++) {
    fscanf(file, "%d %d %d", &p, &r, &w);
    task_t t = {i, p, r, w};
    tasks.push_back(t);
  }
  fclose(file);

  std::vector<std::vector<task_t>> possible_permutations;

  do {
    possible_permutations.push_back(tasks);
  } while (std::next_permutation(tasks.begin(), tasks.end(),
                [](task_t a, task_t b) { return a.id < b.id; }));

  int permutation_n = possible_permutations.size();

  int min_cost = INT32_MAX;
  int local_min_cost[N_THREAD];

  #pragma omp parallel num_threads(N_THREAD)
  {
    int tid = omp_get_thread_num();
    local_min_cost[tid] = INT32_MAX;
    for (int i = tid; i < permutation_n; i += N_THREAD) {
      int cost = schedule(possible_permutations[i].data(), n);
      local_min_cost[tid] = std::min(local_min_cost[tid], cost);
    }
  }

  printf("%d", std::min_element(local_min_cost, local_min_cost + N_THREAD,
                                    [](int a, int b) { return a < b; })[0]);

  return 0;
}