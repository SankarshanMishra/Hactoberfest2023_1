#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

const int MAXN = 100005; // Maximum number of nodes

vector<int> tree[MAXN]; // Adjacency list to store the tree
int depth[MAXN]; // Depth of nodes in the tree
int parent[MAXN][20]; // Binary Lifting array to store ancestors
int chainHead[MAXN]; // Head of the heavy-light chain containing node i
int chainIndex[MAXN]; // Index of node i in its heavy-light chain
int chainSize[MAXN]; // Size of each heavy-light chain
int chainPos[MAXN]; // Position of node i in its heavy-light chain
int chainNo, pos;

// Segment Tree to store maximum weights for each chain
vector<int> segTree[4 * MAXN];

// Function to perform a depth-first search to calculate depths and ancestors
void dfs(int node, int par, int d) {
    depth[node] = d;
    parent[node][0] = par;
    for (int i = 0; i < tree[node].size(); ++i) {
        int child = tree[node][i];
        if (child != par) {
            dfs(child, node, d + 1);
        }
    }
}

// Function to perform binary lifting to calculate ancestors
void precomputeAncestors(int n) {
    for (int j = 1; (1 << j) < n; ++j) {
        for (int i = 1; i <= n; ++i) {
            if (parent[i][j - 1] != -1) {
                parent[i][j] = parent[parent[i][j - 1]][j - 1];
            }
        }
    }
}

// Function to find LCA (Lowest Common Ancestor) using binary lifting
int lca(int u, int v) {
    if (depth[u] < depth[v]) swap(u, v);
    int log = 1;
    while ((1 << log) <= depth[u]) ++log;
    --log;

    for (int i = log; i >= 0; --i) {
        if (depth[u] - (1 << i) >= depth[v]) {
            u = parent[u][i];
        }
    }

    if (u == v) return u;

    for (int i = log; i >= 0; --i) {
        if (parent[u][i] != -1 && parent[u][i] != parent[v][i]) {
            u = parent[u][i];
            v = parent[v][i];
        }
    }

    return parent[u][0];
}

// Function to decompose the tree into heavy-light chains
void hld(int curNode, int weight, int prev) {
    if (chainHead[chainNo] == -1) {
        chainHead[chainNo] = curNode;
    }
    chainIndex[curNode] = chainNo;
    chainPos[curNode] = pos;
    ++pos;

    // Find the heavy child
    int heavyChild = -1, maxWeight = -1;
    for (int i = 0; i < tree[curNode].size(); ++i) {
        int child = tree[curNode][i];
        if (child != prev && (heavyChild == -1 || chainSize[heavyChild] < chainSize[child])) {
            heavyChild = child;
            maxWeight = weight;
        }
    }

    // Recur for heavy child
    if (heavyChild != -1) {
        hld(heavyChild, maxWeight, curNode);
    }

    // Recur for light children
    for (int i = 0; i < tree[curNode].size(); ++i) {
        int child = tree[curNode][i];
        if (child != prev && child != heavyChild) {
            ++chainNo;
            hld(child, weight, curNode);
        }
    }
}

// Function to build the segment tree for a chain
void build(int node, int start, int end, vector<int>& weights) {
    if (start == end) {
        segTree[node] = vector<int>(1, weights[start]);
        return;
    }
    int mid = (start + end) / 2;
    build(2 * node, start, mid, weights);
    build(2 * node + 1, mid + 1, end, weights);
    merge(segTree[2 * node].begin(), segTree[2 * node].end(), segTree[2 * node + 1].begin(), segTree[2 * node + 1].end(), back_inserter(segTree[node]));
}

// Function to query the segment tree for a chain
int query(int node, int start, int end, int l, int r, int k) {
    if (start == end) {
        return segTree[node][0];
    }
    int mid = (start + end) / 2;
    int totalLeft = upper_bound(segTree[2 * node].begin(), segTree[2 * node].end(), r) - lower_bound(segTree[2 * node].begin(), segTree[2 * node].end(), l);
    if (totalLeft >= k) {
        return query(2 * node, start, mid, l, r, k);
    } else {
        return query(2 * node + 1, mid + 1, end, l, r, k - totalLeft);
    }
}

// Function to query the maximum weight between two nodes in the tree
int query(int u, int v, vector<int>& weights) {
    int lcaNode = lca(u, v);
    int ans = 0;

    // Query the chain of node u
    while (chainIndex[u] != chainIndex[lcaNode]) {
        int chainHeadNode = chainHead[chainIndex[u]];
        ans = max(ans, query(1, 0, chainSize[chainIndex[u]] - 1, chainPos[chainHeadNode], chainPos[u], weights[u]));
        u = parent[chainHeadNode][0];
    }

    // Query the chain of node v
    ans = max(ans, query(1, 0, chainSize[chainIndex[u]] - 1, chainPos[lcaNode], chainPos[u], weights[u]));
    while (chainIndex[v] != chainIndex[lcaNode]) {
        int chainHeadNode = chainHead[chainIndex[v]];
        ans = max(ans, query(1, 0, chainSize[chainIndex[v]] - 1, chainPos[chainHeadNode], chainPos[v], weights[v]));
        v = parent[chainHeadNode][0];
    }

    // Query
