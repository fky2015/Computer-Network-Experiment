#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <queue>

using namespace std;

vector<string> split_space(const string &s) {
    vector<string> re;
    string x;
    for (auto c:s) {
        if (c == ' ' || c == '\t') {
            if (x.length()) {
                re.push_back(x);
                x = "";
            }
        } else {
            x.push_back(c);
        }
    }
    if (!x.empty())
        re.push_back(x);
    return re;
}

vector<vector<int> > dis;

pair<int, vector<int>> shortest(int s, int e) {

    int n = dis.size();
    vector<pair<int, int>> p;
    p.resize(n, {-1, 2e9});
    p[s] = {-1, 0};
    queue<int> q;
    vector<int> inq;
    inq.resize(n, 0);
    q.push(s);
    inq[s] = 1;

    while (!q.empty()) {
        auto u = q.front();
        q.pop();
        inq[u] = 0;
        for (int i = 0; i < n; i++) {
            if (inq[i] == 0 && p[u].second + dis[u][i] < p[i].second) {
                p[i] = {u, p[u].second + dis[u][i]};
                q.push(i);
            }
        }
    }

    int now = e;
    vector<int> re;
    while (now != -1) {
        re.push_back(now);
        now = p[now].first;
    }
    reverse(re.begin(), re.end());
    return {p[e].second, re};
}

int main() {
    ifstream config("config.txt");
    char t[1000];
    int line = 0;
    int n = 0;

    while (config.getline(t, 1000)) {
        auto x = split_space(t);
        if (n == 0) {
            n = x.size();
            dis.resize(n);
        }
        for (auto d:x) {
            dis[line].push_back(atoi(d.data()));
        }
        line++;
    }
    cout << "Link State" << endl;
    for (const auto &x:dis) {
        for (auto y:x) {
            cout << y << " ";
        }
        cout << endl;
    }
    vector<vector<int> > s;
    s.resize(n);
    for (int i = 0; i < n; i++) {
        s[i].resize(n);
        for (int j = 0; j < n; j++) {
            auto r = shortest(i, j);
            if (r.second.size() == 1)
                s[i][j] = r.second[0];
            else
                s[i][j] = r.second[1];
            cout << "Shortest Path from " << i << " to " << j << " has a length of " << r.first << endl;
            for (auto x:r.second) {
                cout << x << " ";
            }
            cout << endl;
        }
    }
    for (int i = 0; i < n; i++) {
        cout << "Switch Table of Router " << i << endl;
        for (int j = 0; j < n; j++) {
            cout << "To " << j << ", switch to " << s[i][j] << endl;
        }
    }

    return 0;
}
