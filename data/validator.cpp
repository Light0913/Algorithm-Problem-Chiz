// check_all.cpp - 批量校验所有 30 个测试点
// 编译: g++ -O2 -std=c++17 -o check_all check_all.cpp
// 用法: ./check_all
// 前提: 当前目录下存在 chiz1.in ~ chiz30.in 与 chiz1.out ~ chiz30.out

#include <bits/stdc++.h>
using namespace std;

using ll = long long;

// ============ 全局统计 ============
int g_pass = 0;
int g_fail = 0;

// ============ 单个测试点校验 ============
// 返回 true 表示校验通过
bool checkOne(int subtask, int testcase, int idx) {
    string inFile  = "chiz" + to_string(idx) + ".in";
    string outFile = "chiz" + to_string(idx) + ".out";

    auto printPrefix = [&](const string& status) {
        cout << "[" << status << "] chiz" << idx
             << ".in (subtask=" << subtask << ", test=" << testcase << "): ";
    };

    // ---------- 1. 打开输入文件 ----------
    ifstream fin(inFile);
    if (!fin) {
        printPrefix("FAIL");
        cout << "cannot open " << inFile << "\n";
        return false;
    }

    int n, m;
    if (!(fin >> n >> m)) {
        printPrefix("FAIL");
        cout << "cannot read n, m\n";
        return false;
    }
    if (n < 1 || n > 200) {
        printPrefix("FAIL");
        cout << "n out of range: " << n << "\n";
        return false;
    }
    if (m < 1 || m > 10) {
        printPrefix("FAIL");
        cout << "m out of range: " << m << "\n";
        return false;
    }

    // ---------- 2. 读取 a ----------
    vector<vector<int>> a(n, vector<int>(m));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j) {
            if (!(fin >> a[i][j])) {
                printPrefix("FAIL");
                cout << "cannot read a[" << i << "][" << j << "]\n";
                return false;
            }
            if (a[i][j] < 0 || a[i][j] > 1000000) {
                printPrefix("FAIL");
                cout << "a[" << i << "][" << j << "] out of range: " << a[i][j] << "\n";
                return false;
            }
        }

    // ---------- 3. 读取 b ----------
    vector<vector<int>> b(n, vector<int>(m));
    ll sumB = 0;
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j) {
            if (!(fin >> b[i][j])) {
                printPrefix("FAIL");
                cout << "cannot read b[" << i << "][" << j << "]\n";
                return false;
            }
            if (b[i][j] < 0 || b[i][j] > 1000000) {
                printPrefix("FAIL");
                cout << "b[" << i << "][" << j << "] out of range: " << b[i][j] << "\n";
                return false;
            }
            sumB += b[i][j];
        }
    if (sumB > 2000) {
        printPrefix("FAIL");
        cout << "sum b = " << sumB << " > 2000\n";
        return false;
    }

    // ---------- 4. 读取 K ----------
    for (int i = 0; i + 1 < n; ++i)
        for (int j = 0; j < m; ++j) {
            ll v;
            if (!(fin >> v)) {
                printPrefix("FAIL");
                cout << "cannot read K[" << i << "][" << j << "]\n";
                return false;
            }
            if (v < 1 || v > 1000000) {
                printPrefix("FAIL");
                cout << "K[" << i << "][" << j << "] out of range: " << v << "\n";
                return false;
            }
        }

    // ---------- 5. 读取 L ----------
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j)
            for (int k = 0; k < m; ++k) {
                ll v;
                if (!(fin >> v)) {
                    printPrefix("FAIL");
                    cout << "cannot read L[" << i << "][" << j << "][" << k << "]\n";
                    return false;
                }
                if (j == k) continue;
                if (v < 1 || v > 1000000) {
                    printPrefix("FAIL");
                    cout << "L[" << i << "][" << j << "][" << k
                         << "] out of range: " << v << "\n";
                    return false;
                }
            }

    // ---------- 6. 输入是否有多余内容 ----------
    string extra;
    if (fin >> extra) {
        printPrefix("FAIL");
        cout << "extra content in input: " << extra << "\n";
        return false;
    }

    // ---------- 7. 子任务特殊性质检查 ----------
    if (subtask == 1) {
        bool allEq = true;
        for (int i = 0; i < n && allEq; ++i)
            for (int j = 0; j < m && allEq; ++j)
                if (a[i][j] != b[i][j]) allEq = false;

        if (!allEq && sumB != 0) {
            printPrefix("FAIL");
            cout << "subtask 1 requires a==b or sum b==0\n";
            return false;
        }
    } else if (subtask == 2) {
        for (int j = 0; j < m; ++j) {
            ll sa = 0, sb = 0;
            for (int i = 0; i < n; ++i) {
                sa += a[i][j];
                sb += b[i][j];
                if (sa < sb) {
                    printPrefix("FAIL");
                    cout << "subtask 2 violated at j=" << j << ", t=" << i << "\n";
                    return false;
                }
            }
        }
    } else if (subtask == 3) {
        if (sumB < 1 || sumB > 20) {
            printPrefix("FAIL");
            cout << "subtask 3 requires 1 <= sum b <= 20, got " << sumB << "\n";
            return false;
        }
    } else if (subtask == 4) {
        if (n != 1) {
            printPrefix("FAIL");
            cout << "subtask 4 requires n=1, got " << n << "\n";
            return false;
        }
    } else if (subtask == 5) {
        if (n > 50 || m > 5 || sumB > 500) {
            printPrefix("FAIL");
            cout << "subtask 5 requires n<=50, m<=5, sum b<=500\n";
            return false;
        }
    }

    // ---------- 8. 校验 .out 文件 ----------
    ifstream fout(outFile);
    if (!fout) {
        printPrefix("FAIL");
        cout << "cannot open " << outFile << "\n";
        return false;
    }
    ll answer;
    if (!(fout >> answer)) {
        printPrefix("FAIL");
        cout << "cannot read integer from " << outFile << "\n";
        return false;
    }
    string leftover;
    if (fout >> leftover) {
        printPrefix("FAIL");
        cout << "extra content in " << outFile << ": " << leftover << "\n";
        return false;
    }

    // ---------- 9. 全部通过 ----------
    printPrefix("OK");
    cout << "(n=" << n << ", m=" << m << ", sum b=" << sumB
         << ", answer=" << answer << ")\n";
    return true;
}

// ============ 主函数 ============
int main() {
    // 子任务到测试点的映射：subtask -> {testcase -> idx}
    // idx = (subtask - 1) * 5 + testcase
    for (int st = 1; st <= 6; ++st) {
        cout << "=== Subtask #" << st << " ===\n";
        for (int tc = 1; tc <= 5; ++tc) {
            int idx = (st - 1) * 5 + tc;
            if (checkOne(st, tc, idx)) {
                ++g_pass;
            } else {
                ++g_fail;
            }
        }
        cout << '\n';
    }

    cout << "==============================\n";
    cout << "Total: " << (g_pass + g_fail)
         << ", Passed: " << g_pass
         << ", Failed: " << g_fail << "\n";

    return g_fail == 0 ? 0 : 1;
}