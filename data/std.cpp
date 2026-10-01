#include <bits/stdc++.h>
using namespace std;

using ll = long long;

const int MAXN = 2050;
const ll INF = (1LL << 62);

struct Edge {
	int v, w, rev, qid;
	ll c;
};

vector<Edge> G[MAXN];

struct Data {
	int pos;
	ll dis;
	bool operator < (const Data &x) const {
		return x.dis < dis;
	}
};

struct Quad {
	int u, pos, flow;
	ll C;
};

int s, t, V, idx_h;
int now[MAXN];
ll h[2][MAXN], dis[MAXN];
bitset<MAXN> vis;

int F;
bool changed;
vector<Quad> qedge;

void add(int u, int v, int w, ll c, int qid = -1) {
	G[u].push_back({v, w, (int)G[v].size(), qid, c});
	G[v].push_back({u, 0, (int)G[u].size() - 1,
	                qid, qid == -1 ? 0 : -c});
}

void add_quad(int u, int v, ll C) {
	// C=0 时不会发生边际费用变化，直接当普通边
	if (C == 0) {
		add(u, v, F, 0);
		return;
	}

	int id = qedge.size();
	int pos = G[u].size();

	add(u, v, F, C, id);
	qedge.push_back({u, pos, 0, C});
}

// 当前平方费用为 C*x^2，x 为当前流量
// 正向边际： C(2x+1)
// 反向边际： -C(2x-1)
void update_quad(int id, int delta) {
	Quad &q = qedge[id];
	q.flow += delta;

	int x = q.flow;

	Edge &f = G[q.u][q.pos];
	Edge &r = G[f.v][f.rev];

	f.w = F - x;
	r.w = x;

	f.c = q.C * (2LL * x + 1);
	r.c = -q.C * (2LL * x - 1);
}

void spfa() {
	queue<int> q;
	vis.reset();

	fill(h[idx_h], h[idx_h] + V, INF);

	h[idx_h][s] = 0;
	q.push(s);
	vis[s] = 1;

	while (q.size()) {
		int u = q.front();
		q.pop();
		vis[u] = 0;

		for (auto &e : G[u]) {
			if (!e.w) continue;

			int v = e.v;

			if (h[idx_h][v] > h[idx_h][u] + e.c) {
				h[idx_h][v] = h[idx_h][u] + e.c;

				if (!vis[v]) {
					vis[v] = 1;
					q.push(v);
				}
			}
		}
	}
}

bool dijkstra() {
	priority_queue<Data> q;

	vis.reset();
	fill(dis, dis + V, INF);

	dis[s] = 0;
	h[idx_h ^ 1][s] = h[idx_h][s];

	q.push({s, 0});

	while (q.size()) {
		Data f = q.top();
		q.pop();

		int u = f.pos;

		if (vis[u]) continue;
		vis[u] = 1;
		now[u] = 0;

		for (auto &e : G[u]) {
			if (!e.w) continue;

			int v = e.v;
			if (vis[v]) continue;

			ll c = e.c + h[idx_h][u] - h[idx_h][v];

			if (dis[v] > dis[u] + c) {
				dis[v] = dis[u] + c;
				h[idx_h ^ 1][v] = h[idx_h][v] + dis[v];
				q.push({v, dis[v]});
			}
		}
	}

	if (dis[t] == INF) return false;

	idx_h ^= 1;
	return true;
}

int dfs(int u, int flow, ll &mincost) {
	if (u == t) return flow;

	vis[u] = 1;

	int rest = flow;

	for (int &i = now[u];
	     i < (int)G[u].size() && rest && !changed;
	     ++i) {

		Edge &e = G[u][i];
		int v = e.v;

		if (vis[v] || !e.w) continue;

		if (h[idx_h][v] != h[idx_h][u] + e.c)
			continue;

		// 普通边可以一次送多单位
		// 平方费用边最多送 1 单位
		int lim = min(rest, e.w);

		if (e.qid != -1)
			lim = 1;

		ll oldc = e.c;

		int k = dfs(v, lim, mincost);

		if (!k) continue;

		if (e.qid == -1) {
			// 普通边
			e.w -= k;
			G[v][e.rev].w += k;
		}
		else {
			// 平方费用边
			int id = e.qid;
			Quad &q = qedge[id];

			int delta =
				(u == q.u && i == q.pos) ? 1 : -1;

			update_quad(id, delta);

			// 平方费用边改变后，本轮残量网络已经变化
			changed = true;
		}

		mincost += 1LL * k * oldc;
		rest -= k;
	}

	vis[u] = 0;
	return flow - rest;
}

pair<int, ll> mcmf() {
	int maxflow = 0;
	ll mincost = 0;

	idx_h = 0;

	/*
	 * 初始残量网络的反向边容量全为 0，
	 * 正向费用全部非负，所以 SPFA 得到的初始势能合法。
	 */
	spfa();

	while (maxflow < F && dijkstra()) {
		vis.reset();
		changed = false;

		int got = dfs(s, F - maxflow, mincost);

		if (!got) break;

		maxflow += got;
	}

	return {maxflow, mincost};
}

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	int n, m;
	cin >> n >> m;

	int N = n * m;

	s = N;
	t = N + 1;
	V = N + 2;

	vector<int> a(N), need(N);

	for (int &x : a)
		cin >> x;

	F = 0;

	for (int &x : need) {
		cin >> x;
		F += x;
	}

	vector<ll> K((n - 1) * m);

	for (ll &x : K)
		cin >> x;

	qedge.reserve((n - 1) * m + N * (m - 1));

	// 生产边、需求边
	for (int i = 0; i < N; ++i) {
		if (a[i])
			add(s, i, min(a[i], F), 0);

		if (need[i])
			add(i, t, need[i], 0);
	}

	// 跨时间
	for (int i = 0; i + 1 < n; ++i) {
		for (int k = 0; k < m; ++k) {
			add_quad(
				i * m + k,
				(i + 1) * m + k,
				K[i * m + k]
			);
		}
	}

	// 同时刻跨容器
	for (int i = 0; i < n; ++i) {
		for (int k = 0; k < m; ++k) {
			for (int l = 0; l < m; ++l) {
				ll x;
				cin >> x;

				if (k != l)
					add_quad(
						i * m + k,
						i * m + l,
						x
					);
			}
		}
	}

	if (F == 0) {
		cout << 0 << '\n';
		return 0;
	}

	auto [flow, cost] = mcmf();

	cout << (flow == F ? cost : -1) << '\n';
	return 0;
}