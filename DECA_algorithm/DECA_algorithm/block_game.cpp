//#include <cstdio>
//#include <vector>
//
//char map[30][30] = { 0 };
//int n, m, q, yh;
//
//using namespace std;
//void show() {
//	for (int y = 0; y < n; ++y) {
//		for (int x = 0; x < m; ++x) {
//			printf("%c", map[y][x]);
//		}
//		printf("\n");
//	}
//}
//int dfs(vector<vector<int> > &chk, int y, int x, int *cnt, char sam) {
//	(*cnt) += 1;
//	chk[y][x] = 1;
//	if (y + 1 < n) {
//		if (map[y + 1][x] == sam && chk[y + 1][x] == 0) dfs(chk, y + 1, x, cnt, sam);
//	}
//	if (y - 1 >= 0) {
//		if (map[y - 1][x] == sam && chk[y - 1][x] == 0) dfs(chk, y - 1, x, cnt, sam);
//	}
//	if (x + 1 < m) {
//		if (map[y][x + 1] == sam && chk[y][x + 1] == 0) dfs(chk, y, x + 1, cnt, sam);
//	}
//	if (x - 1 >= 0) {
//		if (map[y][x - 1] == sam && chk[y][x - 1] == 0) dfs(chk, y, x - 1, cnt, sam);
//	}
//	return 0;
//}
//int del(vector<vector<int> > chk, int y, int x, char sam) {
//	map[y][x] = '#';
//	chk[y][x] = 0;
//	if (y + 1 < n) {
//		if (map[y + 1][x] == sam && chk[y + 1][x] == 1) del(chk, y + 1, x, sam);
//	}
//	if (y - 1 >= 0) {
//		if (map[y - 1][x] == sam && chk[y - 1][x] == 1) del(chk, y - 1, x, sam);
//	}
//	if (x + 1 < m) {
//		if (map[y][x + 1] == sam && chk[y][x + 1] == 1) del(chk, y, x + 1, sam);
//	}
//	if (x - 1 >= 0) {
//		if (map[y][x - 1] == sam && chk[y][x - 1] == 1) del(chk, y, x - 1, sam);
//	}
//	return 0;
//}
//int push(char dir) {
//	if (dir == 'U') {
//		for (int x = 0; x < m; ++x) {
//			if (map[0][x] != '#') {
//				for (int x = 0; x < m+1; ++x)getchar();
//				return 1;
//			}
//		}
//		for (int x = 0; x < m; ++x) {
//			for (int y = 1; y < n; ++y) {
//				map[y - 1][x] = map[y][x];
//			}
//		}
//		getchar();
//		for (int x = 0; x < m; ++x) {
//			scanf("%c", &map[n - 1][x]);
//		}
//		for (int x = 0; x < m; ++x) {
//			char *tmp = new char[n];
//			for (int k = 0; k < n; ++k)tmp[k] = '#';
//			int i = n - 1;
//			for (int y = n - 1; y >= 0; --y) {
//				if (map[y][x] != '#') {
//					tmp[i--] = map[y][x];
//					map[y][x] = '#';
//				}
//			}
//			for (int y = n - 1; y >= 0; --y) {
//				map[y][x] = tmp[y];
//			}
//		}
//	}
//	else if (dir == 'R') {
//		for (int y = 0; y < n; ++y) {
//			char *tmp = new char[m];
//			for (int k = 0; k < m; ++k)tmp[k] = '#';
//			int i = m - 1;
//			for (int x = m - 1; x >= 0; --x) {
//				if (map[y][x] != '#') {
//					tmp[i--] = map[y][x];
//					map[y][x] = '#';
//				}
//			}
//			for (int x = m - 1; x >= 0; --x) {
//				map[y][x] = tmp[x];
//			}
//		}
//	}
//	else if (dir == 'L') {
//		for (int y = 0; y < n; ++y) {
//			char *tmp = new char[m];
//			for (int k = 0; k < m; ++k)tmp[k] = '#';
//			int i = 0;
//			for (int x = 0; x < m; ++x) {
//				if (map[y][x] != '#') {
//					tmp[i++] = map[y][x];
//					map[y][x] = '#';
//				}
//			}
//			for (int x = 0; x < m; ++x) {
//				map[y][x] = tmp[x];
//			}
//		}
//	}
//	else if (dir == 'D') {
//		vector<vector<int> > chk(n, vector<int>(m, 0));
//		vector<int> max;
//		int max_value = 0;
//		for (int y = 0; y < n; ++y) {
//			for (int x = 0; x < m; ++x) {
//				if (map[y][x] != '#' && chk[y][x] == 0) {
//					int cnt = 0;
//					dfs(chk, y, x, &cnt, map[y][x]);
//					max.push_back(cnt);
//					max.push_back(y);
//					max.push_back(x);
//					if (max_value < cnt) {
//						max_value = cnt;
//					}
//				}
//			}
//		}
//
//		while (max.size() != 0) {
//			if (max[0] == max_value) {
//				int y = max[1], x = max[2];
//				del(chk, y, x, map[y][x]);
//			}
//			max.erase(max.begin(), max.begin() + 3);
//		}
//		for (int x = 0; x < m; ++x) {
//			char *tmp = new char[n];
//			for (int k = 0; k < n; ++k)tmp[k] = '#';
//			int i = n - 1;
//			for (int y = n - 1; y >= 0; --y) {
//				if (map[y][x] != '#') {
//					tmp[i--] = map[y][x];
//					map[y][x] = '#';
//				}
//			}
//			for (int y = n - 1; y >= 0; --y) {
//				map[y][x] = tmp[y];
//			}
//		}
//	}
//	return 0;
//}
//
//int main() {
//	int T = 0;
//	scanf("%d", &T);
//	for (int t = 0; t < T; ++t) {
//		scanf("%d %d %d", &n, &m, &q);
//		getchar();
//		for (int y = 0; y < n; ++y) {
//			for (int x = 0; x < m; ++x) {
//				scanf("%c", &map[y][x]);
//			}
//			getchar();
//		}
//		for (int i = 0; i < q; ++i) {
//			char tmp = getchar();
//			push(tmp);
//			getchar();
//		}
//		printf("#%d\n", t + 1);
//		for (int y = 0; y < n; ++y) {
//			for (int x = 0; x < m; ++x) {
//				printf("%c", map[y][x]);
//			}
//			printf("\n");
//		}
//		printf("\n");
//	}
//}
//
///*
//3
//5 5 2
//#R##P
//#BR#B
//BGY#G
//PRG#G
//GGB#P
//D
//U GGGGG
//5 5 1
//#RG#P
//#BR#B
//BGY#G
//PRG#G
//GGB#P
//D
//3 7 4
//YBRBB#R
//GRBPB#G
//RGRBG#B
//R
//U BGGRP#P
//R
//D
//*/