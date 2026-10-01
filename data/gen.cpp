#include <bits/stdc++.h>
using namespace std;

using ll = long long;

mt19937_64 rng(1145141919810ULL);

ll rnd(ll l, ll r) {
    return uniform_int_distribution<ll>(l, r)(rng);
}

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
                    d.L[i][j][k] = rnd(0, 1) ? 1 : 1000000;
                else
                    d.L[i][j][k] = rnd(1, 1000000);
            }
}

void addPath(Data &d, const vector<pair<int,int>> &p, int x) {
    if (p.empty() || x <= 0) return;
    d.a[p.front().first][p.front().second] += x;
    d.b[p.back().first][p.back().second] += x;
}

vector<pair<int,int>> timePath(int t1, int t2, int c) {
    vector<pair<int,int>> p;
    for (int t = t1; t <= t2; ++t)
        p.push_back({t, c});
    return p;
}

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

    if (c != ec)
        p.push_back({ed, ec});

    return p;
}

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
        auto p = randomPath(d.n, d.m, st, sc, ed, ec, changes);
        addPath(d, p, 1);
    }
}

void impossibleTotal(Data &d, int need) {
    d.b[0][0] = need;
    d.a[0][0] = need - 1;
}

void impossibleTime(Data &d, int need) {
    d.a[d.n - 1][0] = need;
    d.b[0][0] = need;
}

void impossibleDirection(Data &d, int need) {
    d.a[0][0] = need;
    d.b[d.n - 1][d.m - 1] = need;
    for (int i = 0; i < d.n; ++i)
        for (int j = 0; j < d.m; ++j)
            for (int k = 0; k < d.m; ++k)
                if (j != k)
                    d.L[i][j][k] = (k == 0 ? 1 : 1000000);
    for (int i = 0; i + 1 < d.n; ++i)
        for (int j = 0; j < d.m; ++j)
            d.K[i][j] = 1;
}

void output(const Data &d, const string &file) {
    ofstream out(file);
    out << d.n << ' ' << d.m << '\n';
    for (int i = 0; i < d.n; ++i)
        for (int j = 0; j < d.m; ++j)
            out << d.a[i][j] << " \n"[j == d.m - 1];
    for (int i = 0; i < d.n; ++i)
        for (int j = 0; j < d.m; ++j)
            out << d.b[i][j] << " \n"[j == d.m - 1];
    for (int i = 0; i + 1 < d.n; ++i)
        for (int j = 0; j < d.m; ++j)
            out << d.K[i][j] << " \n"[j == d.m - 1];
    for (int i = 0; i < d.n; ++i)
        for (int j = 0; j < d.m; ++j)
            for (int k = 0; k < d.m; ++k)
                out << d.L[i][j][k]
                    << " \n"[j == d.m - 1 && k == d.m - 1];
}

/* ============================================================
                        #1  a==b 或 sum b==0
   ============================================================ */

Data gen1(int tc) {
    if (tc == 1) {
        // 边界：n=1,m=1,a=b=0
        return Data(1, 1);
    }

    if (tc == 2) {
        // 最大规模，a=b 均匀分布，总需求 2000
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
        initCost(d, 0);
        return d;
    }

    if (tc == 3) {
        // sum b = 0，a 全为 1e6
        Data d(200, 10);
        for (auto &x : d.a)
            for (ll &v : x)
                v = 1000000;
        initCost(d, 0);
        return d;
    }

    if (tc == 4) {
        // 混合：a=b 部分 + sum b=0 部分
        Data d(200, 10);
        int left = 2000;
        for (int i = 0; i < d.n && left; ++i)
            for (int j = 0; j < d.m && left; ++j) {
                int x = min(left, (int)rnd(0, 20));
                d.a[i][j] = d.b[i][j] = x;
                left -= x;
            }
        initCost(d, 0);
        return d;
    }

    // tc=5：极端值覆盖，a=b 大值铺满全网格
    // 注意：本子任务数学上无法卡时间（无转移需求）
    {
        Data d(200, 10);
        int left = 2000;
        while (left) {
            int i = rnd(0, 199);
            int j = rnd(0, 9);
            int x = min(left, (int)rnd(1, 50));
            d.a[i][j] += x;
            d.b[i][j] += x;
            left -= x;
        }
        initCost(d, 3);
        return d;
    }
}

/* ============================================================
                        #2  每容器前缀自给
   ============================================================ */

Data gen2(int tc) {
    // 子任务2：每个容器独立前缀自给自足
    // 对任意 j 和 t：sum_{i<=t} a[i][j] >= sum_{i<=t} b[i][j]

    Data d(200, 10);

    int totalB = 2000;
    if (tc == 5) totalB = rnd(1500, 2000);

    // 随机分配总需求到 10 个容器
    vector<int> remB(10, 0);
    for (int k = 0; k < totalB; ++k)
        remB[rnd(0, 9)]++;

    // 对每个容器独立生成 a、b 序列
    for (int j = 0; j < 10; ++j) {
        int rem = remB[j];
        vector<int> bj(200, 0);

        if (tc == 5) {
            // 【卡时间专用】需求集中到少数几个热点时段，单格 b 值可达数百
            int slots = rnd(3, 8);
            vector<int> pos;
            for (int z = 0; z < slots; ++z)
                pos.push_back(rnd(0, 199));
            sort(pos.begin(), pos.end());
            pos.erase(unique(pos.begin(), pos.end()), pos.end());

            // 剩余全部堆到热点
            for (int z = 0; z < (int)pos.size() && rem > 0; ++z) {
                int take = (z == (int)pos.size() - 1)
                               ? rem
                               : (int)rnd(rem / 2, rem);
                bj[pos[z]] += take;
                rem -= take;
            }

            // 产出：允许超前大量产出，且逐点补齐
            int sumA = 0, sumB = 0;
            for (int i = 0; i < 200; ++i) {
                sumB += bj[i];
                int need = max(0, sumB - sumA);
                d.a[i][j] = need + rnd(0, max(1, need + 50));
                d.b[i][j] = bj[i];
                sumA += d.a[i][j];
            }
        } else {
            // 常规：均匀铺开
            for (int i = 0; i < 200 && rem > 0; ++i) {
                int v = rnd(0, min(10LL, (ll)rem));
                bj[i] = v;
                rem -= v;
            }
            if (rem > 0) bj[199] += rem;

            int sumA = 0, sumB = 0;
            for (int i = 0; i < 200; ++i) {
                sumB += bj[i];
                int need = max(0, sumB - sumA);
                d.a[i][j] = need + rnd(0, 3);
                d.b[i][j] = bj[i];
                sumA += d.a[i][j];
            }
        }
    }

    // 费用模式
    if (tc == 1) {
        initCost(d, 1);
    } else if (tc == 2) {
        // 时间贵，跨容器便宜
        initCost(d);
        for (int i = 0; i + 1 < d.n; ++i)
            for (int j = 0; j < d.m; ++j)
                d.K[i][j] = 1000000;
        for (int i = 0; i < d.n; ++i)
            for (int j = 0; j < d.m; ++j)
                for (int k = 0; k < d.m; ++k)
                    if (j != k) d.L[i][j][k] = 1;
    } else if (tc == 3) {
        initCost(d, 0);
    } else if (tc == 4) {
        initCost(d, 1);
    } else {
        // 【卡时间专用】tc=5：全 1 费用，让边充分展开
        initCost(d, 1);
    }

    return d;
}

/* ============================================================
                        #3  F <= 20
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
        for (int i = 0; i < 199; ++i) {
            d.K[i][0] = 1;
            d.K[i][1] = 1;
        }
        d.L[0][0][1] = 1;
        d.L[199][0][9] = 1;
        return d;
    }

    if (tc == 3) {
        // 多路径费用相等
        d.a[0][0] = 20;
        d.b[199][9] = 20;
        initCost(d, 1);
        for (int i = 0; i < 199; ++i) {
            d.K[i][0] = 1;
            d.K[i][1] = 1;
            d.K[i][2] = 1;
        }
        return d;
    }

    if (tc == 4) {
        // 无解：时间不可达
        impossibleTime(d, 20);
        initCost(d, 1);
        return d;
    }

    // tc=5：F=20，但撒满网格强制长路径
    // 注意：F=20 上限，数学上无法卡时间
    {
        for (int k = 0; k < 20; ++k)
            d.b[rnd(195, 199)][rnd(0, 9)]++;
        for (int k = 0; k < 20; ++k)
            d.a[rnd(0, 5)][rnd(0, 9)]++;
        initCost(d, 1);
        return d;
    }
}

/* ============================================================
                        #4  n = 1
   ============================================================ */

Data gen4(int tc) {
    Data d(1, 10);

    if (tc == 1) {
        d.a[0][0] = 2000;
        d.b[0][9] = 2000;
        initCost(d, 1);
        for (int j = 0; j < 10; ++j)
            for (int k = 0; k < 10; ++k)
                if (j != k) d.L[0][j][k] = 1000000;
        d.L[0][0][1] = 1;
        d.L[0][1][9] = 1;
        return d;
    }

    if (tc == 2) {
        d.a[0][0] = 2000;
        d.b[0][9] = 2000;
        initCost(d);
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

    // tc=5：卡时间，多级中转 + 全 1 费用
    {
        d.a[0][0] = 2000;
        for (int c = 0; c < 10; ++c)
            d.b[0][c] = 200;

        initCost(d, 1);

        // 多级中转：0 → 1 → 2 → ... → 9，让路径变长
        for (int c = 1; c < 9; ++c)
            d.L[0][c][c + 1] = 1;
        d.L[0][0][1] = 1;
        d.L[0][8][9] = 1;

        return d;
    }
}

/* ============================================================
                        #5  n<=50, m<=5, F<=500
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
                    if (j != k) d.L[i][j][k] = 1;
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

    // tc=5：卡时间，前集中产出、后集中需求，全 1 费用
    {
        for (int i = 0; i < 5; ++i)
            for (int j = 0; j < 5; ++j)
                d.a[i][j] = 20;

        for (int i = 45; i < 50; ++i)
            for (int j = 0; j < 5; ++j)
                d.b[i][j] = 20;

        initCost(d, 1);
        return d;
    }
}

/* ============================================================
                        #6  无特殊性质
   ============================================================ */

Data gen6(int tc) {
    Data d(200, 10);

    if (tc == 1) {
        d.a[0][0] = 2000;
        for (int c = 0; c < 10; ++c)
            d.b[199][c] = 200;
        initCost(d, 1);
        return d;
    }

    if (tc == 2) {
        d.a[0][0] = 2000;
        for (int c = 0; c < 10; ++c)
            d.b[199][c] = 200;
        initCost(d, 1);
        for (int c = 0; c < 10; ++c)
            d.L[0][0][c] = 1;
        return d;
    }

    if (tc == 3) {
        d.a[0][0] = 2000;
        for (int c = 0; c < 10; ++c)
            d.b[199][c] = 200;
        initCost(d, 3);
        return d;
    }

    if (tc == 4) {
        impossibleDirection(d, 2000);
        return d;
    }

    // tc=5：卡时间，最大规模 + 长距离 + 全 1 费用
    {
        // 前 20 时段集中产出
        for (int i = 0; i < 20; ++i)
            for (int j = 0; j < 10; ++j)
                d.a[i][j] = 10;

        // 后 20 时段集中需求
        for (int i = 180; i < 200; ++i)
            for (int j = 0; j < 10; ++j)
                d.b[i][j] = 10;

        initCost(d, 1);

        // 强制全连通，让 Dijkstra 每轮探索更多边
        for (int i = 0; i + 1 < d.n; ++i)
            for (int c = 0; c < d.m; ++c)
                d.K[i][c] = 1;

        return d;
    }
}

/* ============================================================
                        检查
   ============================================================ */

bool check(Data &d, int subtask) {
    ll sa = 0, sb = 0;

    for (auto &x : d.a)
        for (ll v : x) {
            if (v < 0 || v > 1000000) return false;
            sa += v;
        }
    for (auto &x : d.b)
        for (ll v : x) {
            if (v < 0 || v > 1000000) return false;
            sb += v;
        }

    if (sb > 2000) return false;

    for (int i = 0; i + 1 < d.n; ++i)
        for (int j = 0; j < d.m; ++j)
            if (d.K[i][j] < 1 || d.K[i][j] > 1000000)
                return false;

    for (int i = 0; i < d.n; ++i)
        for (int j = 0; j < d.m; ++j)
            for (int k = 0; k < d.m; ++k) {
                if (j == k) continue;
                if (d.L[i][j][k] < 1 || d.L[i][j][k] > 1000000)
                    return false;
            }

    if (subtask == 1) {
        bool allEq = true;
        for (int i = 0; i < d.n && allEq; ++i)
            for (int j = 0; j < d.m && allEq; ++j)
                if (d.a[i][j] != d.b[i][j]) allEq = false;
        if (!allEq && sb != 0) return false;
    }
    else if (subtask == 2) {
        for (int j = 0; j < d.m; ++j) {
            ll pa = 0, pb = 0;
            for (int i = 0; i < d.n; ++i) {
                pa += d.a[i][j];
                pb += d.b[i][j];
                if (pa < pb) return false;
            }
        }
    }
    else if (subtask == 3) {
        if (sb < 1 || sb > 20) return false;
    }
    else if (subtask == 4) {
        if (d.n != 1) return false;
    }
    else if (subtask == 5) {
        if (d.n > 50 || d.m > 5) return false;
        if (sb > 500) return false;
    }

    return true;
}

/* ============================================================
                        main
   ============================================================ */

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    for (int s = 1; s <= 6; ++s) {
        for (int tc = 1; tc <= 5; ++tc) {
            Data d;
            if (s == 1) d = gen1(tc);
            if (s == 2) d = gen2(tc);
            if (s == 3) d = gen3(tc);
            if (s == 4) d = gen4(tc);
            if (s == 5) d = gen5(tc);
            if (s == 6) d = gen6(tc);

            if (!check(d, s)) {
                cerr << "ERROR: invalid data for subtask "
                     << s << " testcase " << tc << '\n';
                return 1;
            }

            int id = (s - 1) * 5 + tc;
            string name = "chiz" + to_string(id) + ".in";
            output(d, name);

            ll sa = 0, sb = 0;
            for (auto &x : d.a)
                for (ll v : x) sa += v;
            for (auto &x : d.b)
                for (ll v : x) sb += v;

            ll mx_b = 0;
            for (auto &x : d.b)
                for (ll v : x) mx_b = max(mx_b, v);

            cerr << name
                 << " : n=" << d.n
                 << " m=" << d.m
                 << " sumA=" << sa
                 << " sumB=" << sb
                 << " maxB=" << mx_b
                 << '\n';
        }
    }

    return 0;
}