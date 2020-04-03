#include "bits/stdc++.h"
using namespace std;
#define rep(i,n) for(int (i)=0;(i)<(int)(n);++(i))
#define rer(i,l,u) for(int (i)=(int)(l);(i)<=(int)(u);++(i))
#define reu(i,l,u) for(int (i)=(int)(l);(i)<(int)(u);++(i))
static const int INF = 0x3f3f3f3f; static const long long INFL = 0x3f3f3f3f3f3f3f3fLL;
typedef vector<int> vi; typedef pair<int, int> pii; typedef vector<pair<int, int> > vpii; typedef long long ll;
template<typename T, typename U> static void amin(T &x, U y) { if (y < x) x = y; }
template<typename T, typename U> static void amax(T &x, U y) { if (x < y) x = y; }

class IncrementalAhoCorasic {
    static const int Alphabets = 26;
    static const int AlphabetBase = 'a';
    struct Node {
        Node *fail;
        Node *next[Alphabets];
        long long sum;
        Node() : fail(NULL), next{}, sum(0) { }
    };

    struct String {
        string str;
        int weight;
    };

public:
    //totalLen = sum of (len + 1)
    void init(int totalLen) {
        nodes.resize(totalLen);
        nNodes = 0;
        strings.clear();
        roots.clear();
        sizes.clear();
        que.resize(totalLen);
    }

    void insert(const string &str, int weight) {
        strings.push_back(String{ str, weight });
        roots.push_back(nodes.data() + nNodes);
        sizes.push_back(1);
        nNodes += (int)str.size() + 1;
        auto check = [&]() { return sizes.size() > 1 && sizes.end()[-1] == sizes.end()[-2]; };
        if (!check())
            makePMA(strings.end() - 1, strings.end(), roots.back(), que);
        while (check()) {
            int m = sizes.back();
            roots.pop_back();
            sizes.pop_back();
            sizes.back() += m;
            if (!check())
                makePMA(strings.end() - m * 2, strings.end(), roots.back(), que);
        }
    }

    long long match(const string &str) const {
        long long res = 0;
        for (const Node *t : roots)
            res += matchPMA(t, str);
        return res;
    }

private:
    static void makePMA(vector<String>::const_iterator begin, vector<String>::const_iterator end, Node *nodes, vector<Node*> &que) {
        int nNodes = 0;
        Node *root = new(&nodes[nNodes ++]) Node();
        for (auto it = begin; it != end; ++ it) {
            Node *t = root;
            for (char c : it->str) {
                Node *&n = t->next[c - AlphabetBase];
                if (n == nullptr)
                    n = new(&nodes[nNodes ++]) Node();
                t = n;
            }
            t->sum += it->weight;
        }
        int qt = 0;
        for (Node *&n : root->next) {
            if (n != nullptr) {
                n->fail = root;
                que[qt ++] = n;
            } else {
                n = root;
            }
        }
        for (int qh = 0; qh != qt; ++ qh) {
            Node *t = que[qh];
            int a = 0;
            for (Node *n : t->next) {
                if (n != nullptr) {
                    que[qt ++] = n;
                    Node *r = t->fail;
                    while (r->next[a] == nullptr)
                        r = r->fail;
                    n->fail = r->next[a];
                    n->sum += r->next[a]->sum;
                }
                ++ a;
            }
        }
    }

    static long long matchPMA(const Node *t, const string &str) {
        long long res = 0;
        for (char c : str) {
            int a = c - AlphabetBase;
            while (t->next[a] == nullptr)
                t = t->fail;
            t = t->next[a];
            res += t->sum;
        }
        return res;
    }


    vector<Node> nodes;
    int nNodes;
    vector<String> strings;
    vector<Node*> roots;
    vector<int> sizes;
    vector<Node*> que;
};

int main() {
    int n;
    static char buf[2000001];
    while (~scanf("%d", &n)) {
        vector<string> patterns(n);
        int totalLen = 0;
        rep(i, n) {
            scanf("%s", buf);
            patterns[i] = buf;
            totalLen += (int)patterns[i].size() + 1;
        }
        vector<int> values(n);
        for (int i = 0; i < n; ++ i)
            scanf("%d", &values[i]);
        int Q;
        scanf("%d", &Q);
        vector<string> strings(Q);
        vector<vector<int>> queries(n + 1);
        rep(i, Q) {
            int L; int R;
            scanf("%d%d", &L, &R), ++ R;
            scanf("%s", buf);
            strings[i] = buf;
            queries[L].push_back(i * 2 + 0);
            queries[R].push_back(i * 2 + 1);
        }
        IncrementalAhoCorasic iac;
        iac.init(totalLen);
        vector<long long> ans(Q, 0);
        rep(i, n) {
            iac.insert(patterns[i], values[i]);
            for (int qi : queries[i + 1])
                ans[qi / 2] += iac.match(strings[qi / 2]) * (qi % 2 == 0 ? -1 : 1);
        }
        long long minSum = INFL, maxSum = -INFL;
        rep(i, Q) {
            amin(minSum, ans[i]);
            amax(maxSum, ans[i]);
        }
        printf("%lld %lld\n", minSum, maxSum);
    }
    return 0;
}