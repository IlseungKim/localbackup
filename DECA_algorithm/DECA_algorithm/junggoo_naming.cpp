#include <stdio.h>

int n, m, pos;
unsigned char board[2010 * 2010];
unsigned char name[5000];

void bfs(int y, int x) {
	while (1) {
		if (y == n - 1 && x == m - 1) {
			name[pos++] = board[m*(n - 1) + (m - 1)];
			return;
		}
		name[pos++] = board[y*m + x];
		if (y + 1 >= n) {
			x += 1;
			continue;
		}
		else if (x + 1 >= m) {
			y += 1;
			continue;
		}
		else if (board[(y + 1) * m + x] < board[y * m + (x + 1)]) {
			y += 1;
			continue;
		}
		else if (board[(y + 1) * m + x] > board[y * m + (x + 1)]) {
			x += 1;
			continue;
		}
		else {
			int flag = 1, miny, minx, t = 1, min_cnt = 0;
			unsigned char tmp = board[y * m + x + 1], min;
			while (flag) {
				min = 255;
				t += 1;
				name[pos++] = tmp;
				min_cnt = 0;
				for (int a = 0; a <= t; ++a) {
					int b = t - a;
					if (y + a < n && x + b < m) {
						if (board[(y + a) * m + (x + b)] < min) { // <= min ÀÌ¸י run time??
							min = board[(y + a) * m + (x + b)];
							miny = y + a;
							minx = x + b;
						}
					}
				}
				for (int a = 0; a <= t; ++a) {
					int b = t - a;
					if (y + a < n && x + b < m) {
						if (board[(y + a) * m + (x + b)] == min) min_cnt++;
					}
				}
				if (min_cnt > 1) {
					tmp = min;
				}
				else if(min_cnt == 1) {
					flag = 0;
					y = miny, x = minx;
				}
				else if (t + y >= n && t + x >= m) {
					y = n - 1, x = m - 1;
					flag = 0;
				}
			}
		}
	}
	return;
}

int main() {
	int T = 0;
	scanf("%d", &T);
	for (int t = 0; t < T; ++t) {
		pos = 0;
		//n = 2000, m = 2000;
		scanf("%d %d", &n, &m);
		getchar();
		//getchar();
		for (int y = 0; y < n; ++y) {
			for (int x = 0; x < m; ++x) {
				board[y * m + x] = getchar();
			}
			getchar();
			//getchar();
		}
		bfs(0, 0);
		printf("#%d ", t + 1);
		for (int i = 0; i < pos; ++i) {
			printf("%c", name[i]);
			name[i] = 0;
		}
		printf("\n");
	}
}
/*
1
5 5
adddb
dadbb
ddadd
dbdad
dddda

*/