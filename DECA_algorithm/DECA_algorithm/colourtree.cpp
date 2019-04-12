//#include <cstdio>
//#include <vector>
//
//int N;
//
//using namespace std;
//
//int dfs(vector<char> chk, vector<int> que,int qpos,int pos) {
//	chk[que[pos]] = true;
//	if (que[pos + 2] == que[pos]) {
//		dfs(chk, que, qpos, pos + 2);
//		pos += 2;
//	}
//
//}
//int proc(vector<int> que) {
//	vector<char> chk(N,0);
//	
//}
//
//int main() {
//	int T;
//	scanf("%d", &T);
//	for (int t = 0; t < T; ++t) {
//		vector<int> que;
//		scanf("%d", &N);
//		for (int n = 0; n < N; ++n) {
//			int t1, t2;
//			scanf("%d %d", &t1, &t2);
//			que.push_back(t1);
//			que.push_back(t2);
//		}
//
//	}
//}