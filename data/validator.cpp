// validate.cpp - 输入格式与约束验证器
// 编译: g++ -O2 -o validate validate.cpp
// 用法: ./validate <input>
// 返回: 0 = OK, 1 = 格式/约束错误

#include <bits/stdc++.h>
using namespace std;

using ll = long long;

int main(int argc, char** argv) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input>\n";
        return 1;
    }
    ifstream fin(argv[1]);
    if (!fin) {
        cerr << "Cannot open " << argv[1] << "\n";
        return 1;
    }

    int n, m;
    if (!(fin >> n >> m)) {
        cerr << "FAIL: cannot read n, m\n";
        return 1;
    }
    if (!(1 <= n && n <= 200)) {
        cerr << "FAIL: n out of range: " << n << "\n";
        return 1;
    }
    if (!(1 <= m && m <= 10)) {
        cerr << "FAIL: m out of range: " << m << "\n";
        return 1;
    }

    // 读取 a
    vector<vector<int>> a(n, vector<int>(m));
    ll sumB = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (!(fin >> a[i][j])) {
                cerr << "FAIL: cannot read a[" << i << "][" << j << "]\n";
                return 1;
            }
            if (a[i][j] < 0 || a[i][j] > 1000000) {
                cerr << "FAIL: a[" << i << "][" << j << "] out of range: "
                     << a[i][j] << "\n";
                return 1;
            }
        }
    }

    // 读取 b
    vector<vector<int>> b(n, vector<int>(m));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (!(fin >> b[i][j])) {
                cerr << "FAIL: cannot read b[" << i << "][" << j << "]\n";
                return 1;
            }
            if (b[i][j] < 0 || b[i][j] > 1000000) {
                cerr << "FAIL: b[" << i << "][" << j << "] out of range: "
                     << b[i][j] << "\n";
                return 1;
            }
            sumB += b[i][j];
        }
    }
    if (sumB > 2000) {
        cerr << "FAIL: sum b = " << sumB << " > 2000\n";
        return 1;
    }

    // 读取 K：(n-1) 行，每行 m 个
    for (int i = 0; i + 1 < n; ++i) {
        for (int j = 0; j < m; ++j) {
            ll v;
            if (!(fin >> v)) {
                cerr << "FAIL: cannot read K[" << i << "][" << j << "]\n";
                return 1;
            }
            if (v < 1 || v > 1000000) {
                cerr << "FAIL: K[" << i << "][" << j << "] out of range: "
                     << v << "\n";
                return 1;
            }
        }
    }

    // 读取 L：n 行，每行 m*m 个
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            for (int k = 0; k < m; ++k) {
                ll v;
                if (!(fin >> v)) {
                    cerr << "FAIL: cannot read L[" << i << "][" << j
                         << "][" << k << "]\n";
                    return 1;
                }
                if (j == k) continue; // 对角线无意义
                if (v < 1 || v > 1000000) {
                    cerr << "FAIL: L[" << i << "][" << j << "][" << k
                         << "] out of range: " << v << "\n";
                    return 1;
                }
            }
        }
    }

    // 检查是否有多余内容
    string extra;
    if (fin >> extra) {
        cerr << "FAIL: extra content after input: " << extra << "\n";
        return 1;
    }

    cerr << "OK: " << argv[1]
         << " (n=" << n << ", m=" << m
         << ", sum b=" << sumB << ")\n";
    return 0;
}