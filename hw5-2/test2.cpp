#include <cmath>
#include <iostream>
#include <numeric>
#include <omp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#define N_THREAD 16

typedef std::pair<int, int> point_t;

std::vector<point_t> points;

float prim(std::vector<int> &hull, std::vector<int> &others) {
    std::vector<point_t> all;
    for (int i = 0; i < hull.size(); i++)
        all.push_back(points[hull[i]]);
    for (int i = 0; i < others.size(); i++)
        all.push_back(points[others[i]]);

    int n = all.size();
    std::vector<bool> visited(n, false);
    std::vector<float> dist(n, 1e9);
    std::vector<int> parent(n, -1);
    dist[0] = 0;

    for (int i = 0; i < n; i++) {
        int u = -1;
        for (int j = 0; j < n; j++)
            if (!visited[j] && (u == -1 || dist[j] < dist[u]))
                u = j;

        visited[u] = true;
        for (int j = 0; j < n; j++) {
            float d = std::sqrt( 
                (all[j].first - all[u].first) * (all[j].first - all[u].first) +
                (all[j].second - all[u].second) * (all[j].second - all[u].second));
            d = std::floor(d * 10000.0) / 10000.0;
            if (!visited[j] && d < dist[j]) {
                dist[j] = d;
                parent[j] = u;
            }
        }
    }
    float ans = 0;
    for (int i = 0; i < n; i++)
    ans += dist[i];
    return ans;
}

int main(int argc, char *argv[]) {
    int n;

    char file_name[128];
    scanf("%s", file_name);
    FILE *file = fopen(file_name, "r");
    fscanf(file, "%d", &n);
    for (int i = 0; i < n; i++) {
        int x, y;
        fscanf(file, "%d %d", &x, &y);
        points.push_back(std::make_pair(x, y));
    }
    fclose(file);

    // convex hull
    std::vector<int> hull;
    int left = 0;
    for (int i = 1; i < n; i++)
        if (points[i].first < points[left].first)
            left = i;

    int p = left, q;
    do {
        hull.push_back(p);
        q = (p + 1) % n;
        for (int i = 0; i < n; i++)
            if ((points[i].first - points[p].first) * (points[q].second - points[p].second)
                -(points[i].second - points[p].second) * (points[q].first - points[p].first) < 0)
            q = i;
            p = q;
    } while (p != left);

    std::vector<int> others;
    for (int i = 0; i < n; i++) {
        bool found = false;
        for (int j = 0; j < hull.size(); j++)
            if (hull[j] == i) {
                found = true;
                break;
            }
        if (!found) others.push_back(i);
    }

    float min_dist = 1e9;
    float local_min_dist[N_THREAD];
    std::fill(local_min_dist, local_min_dist + N_THREAD, 1e9);

    #pragma omp parallel num_threads(N_THREAD)
    {
    for (int i = omp_get_thread_num(); i < (1 << (others.size())); i += N_THREAD) {
        std::vector<int> tmp;
        for (int j = 0; j < others.size(); j++)
            if (i & (1 << j)) tmp.push_back(others[j]);

        float ans = prim(hull, tmp);
        if (ans < local_min_dist[omp_get_thread_num()])
            local_min_dist[omp_get_thread_num()] = ans;
        }
    }

    for (int i = 0; i < N_THREAD; i++)
        if (local_min_dist[i] < min_dist)
            min_dist = local_min_dist[i];

    printf("%.4f", min_dist);

    return 0;
    }

