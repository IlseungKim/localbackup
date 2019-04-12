#include <cstdio>
#include <vector>

using namespace std;

int n, m;
vector<char> board(2000*2000,0);

int same(int *a, int *b, char prev) {
	char min = prev;
	int ym = *a, xm = *b, flag = 1, miny, minx;
	int t = 1;
	while (flag) {
		if (ym + 1 < n)ym++;
		if (xm + 1 < m)xm++;
		t++;

		for (int y = *a; y <= ym; ++y) {
			if (xm + y == t) {
				if (board[y * m + xm] < min) {
					min = board[y * m + xm];
					miny = y;
					minx = xm;
				}
			}
		}
		for (int x = *b; x < xm; ++x) {
			if (ym + x == t) {
				if (board[ym * m + x] < min) {
					min = board[ym * m + x];
					miny = ym;
					minx = x;
				}
			}
		}
		if (min < prev) flag = 0;
		if (ym == n - 1 && xm == m - 1) {
			*a = n - 1, *b = m - 1;
			return -1;
		}
	}
	*a = miny, *b = minx;
	return 0;
}
int bfs(vector<char> *name, int y, int x) {
	(*name).push_back(board[y*m + x]);

	if (y == n - 1 && x == m - 1) {
		return 0;
	}

	if (y + 1 >= n) {
		bfs(name, y, x + 1);
	}
	else if (x + 1 >= m) {
		bfs(name, y + 1, x);
	}
	else if (board[(y + 1)*m + x] < board[y*m + (x + 1)]) {
		bfs(name, y + 1, x);
	}
	else if (board[(y + 1)*m + x] > board[y*m + (x + 1)]) {
		bfs(name, y, x + 1);
	}
	else {
		int a = y, b = x;
		char tmp;
		if (x + 1 < m)tmp = board[y*m + x + 1];
		else if (y + 1 < n)tmp = board[(y + 1)*m + x];

		same(&a, &b, tmp);

		for (int i = 0; i < (a - y + b - x - 1); ++i) {
			(*name).push_back(tmp);
		}
		bfs(name, a, b);
	}
}

int main() {
	int T = 0;
	scanf("%d", &T);
	for (int t = 0; t < T; ++t) {
		vector<char> name;
		scanf("%d %d", &n, &m);
		getchar();
		for (int y = 0; y < n; ++y) {
			for (int x = 0; x < m; ++x) {
				scanf("%c", &board[y*m+x]);
			}
			getchar();
		}
		bfs(&name, 0, 0);
		printf("#%d ", t + 1);
		for (int i = 0; i < name.size(); ++i) {
			printf("%c", name[i]);
		}
		printf("\n");
	}
}