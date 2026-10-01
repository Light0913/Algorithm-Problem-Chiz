#include <bits/stdc++.h>
using namespace std;

using ll = long long;

mt19937_64 rng(1145141919810ULL);

ll rnd(ll l, ll r) {
    return uniform_int_distribution<ll>(l, r)(rng);
}

struct Edge {
    int u, v;
    ll w;
};

struct Data {
    int n, m;
    vector<vector<ll>> a, b;
    vector<vector<ll>> K;
    vector<vector<vector<ll>>> L;

    Data(int n = 1, int m = 1) : n(n), m(m) {
        a.assign(n, vector<ll>(m, 0));
        b.assign(n, vector<ll>(m, 0));
        K.assign(max(0, n - 1), vector<ll>(m, 1));
        L.assign(n, vector<vector<ll>>(m, vector<ll>(m, 1)));
    }
};

void initCost(Data &d, int mode = 0) {
    for (int i = 0; i + 1 < d.n; ++i)
        for (int j = 0; j < d.m; ++j) {
            if (mode == 1) d.K[i][j] = 1;
            else if (mode == 2) d.K[i][j] = 1000000;
            else if (mode == 3)
                d.K[i][j] = rnd(0, 1) ? 1 : 1000000;
            else
                d.K[i][j] = rnd(1, 1000000);
        }

    for (int i = 0; i < d.n; ++i)
        for (int j = 0; j < d.m; ++j)
            for (int k = 0; k < d.m; ++k) {
                if (j == k) {
                    d.L[i][j][k] = 1;
                    continue;
                }

                if (mode == 1) d.L[i][j][k] = 1;
                else if (mode == 2) d.L[i][j][k] = 1000000;
                else if (mode == 3)
                    d.L[i][j][k] =
                        rnd(0, 1) ? 1 : 1000000;
                else
                    d.L[i][j][k] = rnd(1, 1000000);
            }
}

/*
    将若干条合法路径上的流量加入图。

    path:
        (time, container)

    一条路径上的流量 x：
        起点产生 x
        终点消耗 x
        中间节点保持流量守恒

    因而最终一定存在一个合法运输方案。
*/
void addPath(Data &d, const vector<pair<int,int>> &p, int x) {
    if (p.empty() || x <= 0) return;

    // 起点产生
    d.a[p.front().first][p.front().second] += x;

    // 终点消耗
    d.b[p.back().first][p.back().second] += x;
}

/*
    产生纯时间链：
    (t1,c) -> (t1+1,c) -> ... -> (t2,c)
*/
vector<pair<int,int>> timePath(int t1, int t2, int c) {
    vector<pair<int,int>> p;
    for (int t = t1; t <= t2; ++t)
        p.push_back({t, c});
    return p;
}

/*
    产生：
    时间上走到 ta
    然后在 ta 时刻跨容器
    再继续走到 tb
*/
vector<pair<int,int>> changePath(
    int t1, int tb,
    int c1, int c2,
    int tc
) {
    vector<pair<int,int>> p;

    for (int t = t1; t <= tc; ++t)
        p.push_back({t, c1});

    p.push_back({tc, c2});

    for (int t = tc + 1; t <= tb; ++t)
        p.push_back({t, c2});

    return p;
}

/*
    生成多条随机合法路径。
    每条路径只允许：
      1. t -> t+1，同容器
      2. 同 t，换容器

    因而天然符合题目中的移动规则。
*/
vector<pair<int,int>> randomPath(
    int n, int m,
    int st, int sc,
    int ed, int ec,
    int changes
) {
    vector<pair<int,int>> p;
    int t = st, c = sc;

    p.push_back({t, c});

    vector<int> ct;
    if (changes) {
        for (int z = 0; z < changes; ++z)
            ct.push_back(rnd(t + 1, max(t + 1, ed)));
        sort(ct.begin(), ct.end());
        ct.erase(unique(ct.begin(), ct.end()), ct.end());
    }

    int ptr = 0;

    while (t < ed) {
        if (ptr < (int)ct.size() && t == ct[ptr]) {
            c = rnd(0, m - 1);
            ++ptr;
            p.push_back({t, c});
        }

        ++t;
        p.push_back({t, c});
    }

    if (c != ec) {
        p.push_back({ed, ec});
    }

    return p;
}

/*
    将总需求分成若干份。
*/
vector<int> splitFlow(int total, int pieces) {
    vector<int> ret;
    if (pieces <= 0) return ret;

    pieces = min(pieces, total);

    for (int i = 0; i < pieces; ++i)
        ret.push_back(1);

    for (int i = pieces; i < total; ++i)
        ret[rnd(0, pieces - 1)]++;

    return ret;
}

/*
    强制制造“多条等价路线”的数据。

    源：
        (st,0)

    汇：
        (ed,m-1)

    中间容器可以互相切换。
*/
void makeParallel(
    Data &d,
    int st, int ed,
    int source, int sink,
    int total,
    int paths,
    bool exactEqual
) {
    vector<int> fs;

    if (exactEqual) {
        fs.assign(paths, total / paths);
        for (int i = 0; i < total % paths; ++i)
            fs[i]++;
    } else {
        fs = splitFlow(total, paths);
    }

    for (int z = 0; z < paths; ++z) {
        int c = z % d.m;

        auto p1 = timePath(st, ed, c);

        vector<pair<int,int>> p;

        p.push_back({st, source});

        if (source != c)
            p.push_back({st, c});

        for (int t = st + 1; t <= ed; ++t)
            p.push_back({t, c});

        if (c != sink)
            p.push_back({ed, sink});

        addPath(d, p, fs[z]);
    }
}

/*
    构造大量随机合法运输方案。
*/
void makeRandomFlow(
    Data &d,
    int total,
    int sources,
    int sinks,
    int maxChanges
) {
    for (int z = 0; z < total; ++z) {
        int st = rnd(0, d.n - 2);
        int ed = rnd(st + 1, d.n - 1);

        int sc = rnd(0, sources - 1);
        int ec = rnd(0, sinks - 1);

        int changes = rnd(0, maxChanges);

        auto p = randomPath(
            d.n, d.m,
            st, sc,
            ed, ec,
            changes
        );

        addPath(d, p, 1);
    }
}

/*
    生成无解：
    1. 总量不足
*/
void impossibleTotal(Data &d, int need) {
    d.b[0][0] = need;
    d.a[0][0] = need - 1;
}

/*
    生成无解：
    总量足够，但全部供给在需求之后。
*/
void impossibleTime(Data &d, int need) {
    d.a[d.n - 1][0] = need;
    d.b[0][0] = need;
}

/*
    生成无解：
    时间上允许，但容器方向无法到达。
*/
void impossibleDirection(Data &d, int need) {
    d.a[0][0] = need;
    d.b[d.n - 1][d.m - 1] = need;

    /*
        让所有跨容器边都只能朝 0 号容器，
        但需求在 m-1。
    */
    for (int i = 0; i < d.n; ++i)
        for (int j = 0; j < d.m; ++j)
            for (int k = 0; k < d.m; ++k)
                if (j != k)
                    d.L[i][j][k] =
                        (k == 0 ? 1 : 1000000);

    for (int i = 0; i + 1 < d.n; ++i)
        for (int j = 0; j < d.m; ++j)
            d.K[i][j] = 1;
}

/*
    输出。
*/
void output(const Data &d, const string &file) {
    ofstream out(file);

    out << d.n << ' ' << d.m << '\n';

    for (int i = 0; i < d.n; ++i) {
        for (int j = 0; j < d.m; ++j)
            out << d.a[i][j]
                << " \n"[j == d.m - 1];
    }

    for (int i = 0; i < d.n; ++i) {
        for (int j = 0; j < d.m; ++j)
            out << d.b[i][j]
                << " \n"[j == d.m - 1];
    }

    for (int i = 0; i + 1 < d.n; ++i) {
        for (int j = 0; j < d.m; ++j)
            out << d.K[i][j]
                << " \n"[j == d.m - 1];
    }

    for (int i = 0; i < d.n; ++i)
        for (int j = 0; j < d.m; ++j)
            for (int k = 0; k < d.m; ++k)
                out << d.L[i][j][k]
                    << " \n"[j == d.m - 1 &&
                            k == d.m - 1];
}

/* ============================================================
                        #1
   ============================================================ */

Data gen1(int tc) {
    if (tc == 1) {
        Data d(1, 1);
        return d;
    }

    if (tc == 2) {
        Data d(200, 10);

        int left = 2000;

        for (int i = 0; i < d.n; ++i)
            for (int j = 0; j < d.m; ++j) {
                int x = min(left, (int)rnd(0, 20));
                left -= x;
                d.a[i][j] = d.b[i][j] = x;
            }

        if (left)
            d.a[199][9] = d.b[199][9] += left;

        initCost(d);
        return d;
    }

    if (tc == 3) {
        Data d(200, 10);

        for (auto &x : d.a)
            for (ll &v : x)
                v = 1000000;

        initCost(d);
        return d;
    }

    if (tc == 4) {
        Data d(200, 10);

        int left = 2000;

        for (int i = 0; i < d.n && left; ++i)
            for (int j = 0; j < d.m && left; ++j) {
                int x = min(left, (int)rnd(0, 20LL));
                d.a[i][j] = d.b[i][j] = x;
                left -= x;
            }

        initCost(d);
        return d;
    }

    Data d(200, 10);

    int left = 2000;

    while (left) {
        int i = rnd(0, 199);
        int j = rnd(0, 9);

        int x = min(left, (int)rnd(0, 100LL));
        if (!x) continue;

        d.a[i][j] += x;
        d.b[i][j] += x;
        left -= x;
    }

    initCost(d);
    return d;
}

/* ============================================================
                        #2
   ============================================================ */

Data gen2(int tc) {
    Data d(200, 10);

    if (tc == 1) {
        d.a[0][0] = 2000;

        for (int j = 0; j < 10; ++j)
            d.b[199][j] = (j == 9 ? 200 : 200);

        // 补足 2000
        int cur = 2000;
        for (int j = 0; j < 10; ++j)
            d.b[199][j] = 200;

        initCost(d, 1);
        return d;
    }

    if (tc == 2) {
        /*
            需求在后期，供给在前期。
            早换容器便宜，时间运输很贵。
        */
        d.a[0][0] = 2000;

        for (int j = 0; j < 10; ++j)
            d.b[199][j] = 200;

        initCost(d);

        for (int i = 0; i < 199; ++i)
            for (int j = 0; j < 10; ++j)
                d.K[i][j] = 1000000;

        for (int i = 0; i < 200; ++i)
            for (int j = 0; j < 10; ++j)
                for (int k = 0; k < 10; ++k)
                    if (j != k)
                        d.L[i][j][k] = 1;

        return d;
    }

    if (tc == 3) {
        /*
            每个容器独立有前缀余量，
            但后期大量依赖前期库存。
        */
        int left = 2000;

        for (int j = 0; j < 10; ++j) {
            int x = 200;
            d.a[0][j] = x;
            d.b[199][j] = x;
        }

        initCost(d);
        return d;
    }

    if (tc == 4) {
        /*
            前缀恰好相等。
        */
        for (int j = 0; j < 10; ++j) {
            int x = 200;
            d.a[0][j] = x;
            d.b[199][j] = x;
        }

        initCost(d, 1);
        return d;
    }

    /*
        极端费用。
    */
    d.a[0][0] = 2000;

    for (int j = 0; j < 10; ++j)
        d.b[199][j] = 200;

    initCost(d, 3);
    return d;
}

/* ============================================================
                        #3
   ============================================================ */

Data gen3(int tc) {
    Data d(200, 10);

    if (tc == 1) {
        d.a[0][0] = 20;
        d.b[199][9] = 20;

        initCost(d, 1);
        return d;
    }

    if (tc == 2) {
        d.a[0][0] = 20;
        d.b[199][9] = 20;

        initCost(d, 1);

        // 两条路线
        for (int i = 0; i < 199; ++i) {
            d.K[i][0] = 1;
            d.K[i][1] = 1;
        }

        d.L[0][0][1] = 1;
        d.L[199][0][9] = 1;

        return d;
    }

    if (tc == 3) {
        d.a[0][0] = 20;
        d.b[199][9] = 20;

        initCost(d, 1);

        // 三条通道
        for (int i = 0; i < 199; ++i) {
            d.K[i][0] = 1;
            d.K[i][1] = 1;
            d.K[i][2] = 2;
        }

        return d;
    }

    if (tc == 4) {
        impossibleTime(d, 20);
        initCost(d, 1);
        return d;
    }

    d.a[0][0] = 20;
    d.b[199][9] = 20;

    initCost(d, 3);

    return d;
}

/* ============================================================
                        #4
   ============================================================ */

Data gen4(int tc) {
    Data d(1, 10);

    if (tc == 1) {
        d.a[0][0] = 2000;
        d.b[0][9] = 2000;

        initCost(d, 1);

        for (int j = 0; j < 10; ++j)
            for (int k = 0; k < 10; ++k)
                if (j != k)
                    d.L[0][j][k] = 1000000;

        d.L[0][0][1] = 1;
        d.L[0][1][9] = 1;

        return d;
    }

    if (tc == 2) {
        d.a[0][0] = 2000;
        d.b[0][9] = 2000;

        initCost(d);

        /*
            4 条并行通路。
        */
        for (int c = 1; c <= 4; ++c) {
            d.L[0][0][c] = 1;
            d.L[0][c][9] = 1;
        }

        return d;
    }

    if (tc == 3) {
        impossibleTotal(d, 2000);
        initCost(d);
        return d;
    }

    if (tc == 4) {
        d.a[0][0] = 2000;
        d.b[0][9] = 2000;

        initCost(d, 3);
        return d;
    }

    /*
        多级中转。
    */
    d.a[0][0] = 2000;
    d.b[0][9] = 2000;

    initCost(d, 1);

    for (int c = 1; c < 9; ++c)
        d.L[0][c][c + 1] = 1;

    d.L[0][0][1] = 1;
    d.L[0][8][9] = 1;

    return d;
}

/* ============================================================
                        #5
   ============================================================ */

Data gen5(int tc) {
    Data d(50, 5);

    if (tc == 1) {
        d.a[0][0] = 500;

        for (int c = 0; c < 5; ++c)
            d.b[49][c] = 100;

        initCost(d, 1);
        return d;
    }

    if (tc == 2) {
        d.a[0][0] = 500;

        for (int c = 0; c < 5; ++c)
            d.b[49][c] = 100;

        initCost(d);

        for (int i = 0; i < 49; ++i)
            for (int j = 0; j < 5; ++j)
                d.K[i][j] = 1000000;

        for (int i = 0; i < 50; ++i)
            for (int j = 0; j < 5; ++j)
                for (int k = 0; k < 5; ++k)
                    if (j != k)
                        d.L[i][j][k] = 1;

        return d;
    }

    if (tc == 3) {
        d.a[0][0] = 500;

        for (int c = 0; c < 5; ++c)
            d.b[49][c] = 100;

        initCost(d, 3);
        return d;
    }

    if (tc == 4) {
        impossibleTime(d, 500);
        initCost(d);
        return d;
    }

    /*
        综合：
        多源、多汇、随机合法路径。
    */
    makeRandomFlow(d, 500, 5, 5, 4);

    initCost(d, 0);

    return d;
}

/* ============================================================
                        #6
   ============================================================ */

Data gen6(int tc) {
    Data d(200, 10);

    if (tc == 1) {
        /*
            长距离运输：
            2000 单位都从早期到后期。
        */
        d.a[0][0] = 2000;

        for (int c = 0; c < 10; ++c)
            d.b[199][c] = 200;

        initCost(d, 1);
        return d;
    }

    if (tc == 2) {
        /*
            最大规模多路径拆流。
        */
        d.a[0][0] = 2000;

        for (int c = 0; c < 10; ++c)
            d.b[199][c] = 200;

        initCost(d, 1);

        /*
            多条通道：
            0 -> c
            沿 c 号容器向后
            c -> 目标
        */
        for (int c = 0; c < 10; ++c) {
            d.L[0][0][c] = 1;
            d.L[199][c][c] = 1;
        }

        return d;
    }

    if (tc == 3) {
        /*
            极端 1 / 1e6。
        */
        d.a[0][0] = 2000;

        for (int c = 0; c < 10; ++c)
            d.b[199][c] = 200;

        initCost(d, 3);
        return d;
    }

    if (tc == 4) {
        /*
            三类无解混合。
            这里采用方向无解作为主结构。
        */
        impossibleDirection(d, 2000);
        return d;
    }

    /*
        综合魔王：
        先生成 2000 条合法单位流，
        每条流随机选择时间跨度、容器、
        若干次换容器。
    */
    makeRandomFlow(d, 2000, 10, 10, 8);

    /*
        强制加入一批结构化并行流，
        防止随机路径退化成简单运输。
    */
    d.a[0][0] += 0;
    initCost(d, 3);

    /*
        再人为制造一些明显便宜的通道。
    */
    for (int i = 0; i + 1 < d.n; ++i) {
        for (int c = 0; c < d.m; ++c) {
            if (c <= 4)
                d.K[i][c] = 1;
            else
                d.K[i][c] = 1000000;
        }
    }

    for (int i = 0; i < d.n; ++i) {
        for (int j = 0; j < d.m; ++j) {
            for (int k = 0; k < d.m; ++k) {
                if (j == k) continue;

                if (j < 5 && k < 5)
                    d.L[i][j][k] = 1;
                else
                    d.L[i][j][k] = 1000000;
            }
        }
    }

    return d;
}

/* ============================================================
                    检查生成结果
   ============================================================ */

bool check(Data &d) {
    ll sa = 0, sb = 0;

    for (auto &x : d.a)
        for (ll v : x) {
            if (v < 0 || v > 1000000)
                return false;
            sa += v;
        }

    for (auto &x : d.b)
        for (ll v : x) {
            if (v < 0 || v > 1000000)
                return false;
            sb += v;
        }

    if (sb > 2000)
        return false;

    for (int i = 0; i + 1 < d.n; ++i)
        for (int j = 0; j < d.m; ++j)
            if (d.K[i][j] < 1 ||
                d.K[i][j] > 1000000)
                return false;

    for (int i = 0; i < d.n; ++i)
        for (int j = 0; j < d.m; ++j)
            for (int k = 0; k < d.m; ++k)
                if (d.L[i][j][k] < 1 ||
                    d.L[i][j][k] > 1000000)
                    return false;

    return true;
}

/* ============================================================
                         main
   ============================================================ */

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<Data> all;

    for (int s = 1; s <= 6; ++s) {
        for (int tc = 1; tc <= 5; ++tc) {
            Data d;

            if (s == 1) d = gen1(tc);
            if (s == 2) d = gen2(tc);
            if (s == 3) d = gen3(tc);
            if (s == 4) d = gen4(tc);
            if (s == 5) d = gen5(tc);
            if (s == 6) d = gen6(tc);

            if (!check(d)) {
                cerr << "ERROR: generated invalid data "
                     << s << " " << tc << '\n';
                return 1;
            }

            int id = (s - 1) * 5 + tc;

            string name =
                "chiz" + to_string(id) + ".in";

            output(d, name);

            ll sa = 0, sb = 0;
            for (auto &x : d.a)
                for (ll v : x) sa += v;
            for (auto &x : d.b)
                for (ll v : x) sb += v;

            cerr << name
                 << " : n=" << d.n
                 << " m=" << d.m
                 << " sumA=" << sa
                 << " sumB=" << sb
                 << '\n';
        }
    }

    return 0;
}