//#pragma warning(disable:4996)
//
//#include <stdio.h>
//#include <stdlib.h>
//#include <iostream>
//#include <vector>
//
//void dp_ladder() {
//	int N, M, H;
//	std::cin >> N >> M >> H;
//}
//void dp_RGB() {
//	class RGB {
//	public:
//		int price[3];
//		int min_idx[3];
//		RGB() {};
//		RGB(int r, int g, int b) { 
//			price[0] = r; 
//			price[1] = g;
//			price[2] = b;
//			min_idx[0] = price[0] > price[1] ? 1 : 0;
//			min_idx[0] = price[2] > price[min_idx[0]] ? min_idx[0] : 2;
//			min_idx[1] = 1, min_idx[2] = 2;
//
//			if (min_idx[0] == min_idx[1]) min_idx[1] = 0;
//			else if (min_idx[0] == min_idx[2])min_idx[2] = 0;
//		};
//	};
//	
//	int N;
//	std::vector<RGB> home;
//	std::cin >> N;
//	
//	for (int i = 0; i < N; ++i) {
//		int r, g, b;
//		std::cin >> r >> g >> b;
//		RGB tmp(r, g, b);
//		home.push_back(tmp);
//	}
//	int price = 0;
//	//----------i == 0--------------
//	int i = 0, tmp = pow(2, 32) - 1;
//
//	for (i = 1; i < N; ++i) {
//		
//	}
//}
//
//void dp_123() {
//	int T = 0;
//	scanf_s("%d", &T);
//	
//	int *dp = (int*)calloc(11, sizeof(int));
//	int *test = (int*)calloc(T, sizeof(int));
//
//	dp[0] = 1;
//	dp[1] = 2;
//	dp[2] = 4;
//
//	for (int j = 3; j < 11; ++j) {
//		dp[j] = dp[j - 3] + dp[j - 2] + dp[j - 1];
//	}
//
//	for (int ts = 0; ts < T; ++ts) {
//		int n = 0;
//		scanf_s("%d", &test[ts]);
//	}
//
//	for (int ts = 0; ts < T; ++ts) {
//		printf("%d\n", dp[test[ts] - 1]);
//	}
//};
//
//void dp_pibo() {
//
//};
//
//void dp_stair() {
//	unsigned int **dp = (unsigned int**)calloc(10, sizeof(unsigned int*));
//	for (int i = 0; i < 10; ++i) {
//		dp[i] = (unsigned int*)calloc(100, sizeof(int));
//	}
//
//	int n = 0;
//
//	scanf("%d", &n);
//
//	dp[0][0] = 0;
//	for (int i = 1; i < 10; ++i) {
//		dp[i][0] = 1;
//	}
//
//	for (int j = 1; j <= 100; ++j) {
//		for (int i = 0; i < 10; ++i) {
//			if (i == 0) {
//				dp[i][j] = dp[i + 1][j - 1];
//			}
//			else if (i < 9) {
//				dp[i][j] = dp[i - 1][j - 1] + dp[i + 1][j - 1];
//			}
//			else {
//				dp[i][j] = dp[i - 1][j - 1];
//			}
//		}
//	}
//	for (int i = 1; i < 10; ++i) {
//		for (int j = 0; j <= 100; ++j) {
//			dp[0][j] += dp[i][j];
//		}
//	}
//
//	printf("%d", dp[0][n-1] % 1000000000);
//}
//
//int simul_2160() {
//	int N = 0;
//	scanf("%d", &N);
//	getchar();
//	int width = 7, height = 5, size = N * width * height;
//
//	int *arr = (int*)calloc(size, sizeof(int));
//
//	for (int i = 0; i < height*N; ++i) {
//		for (int j = 0; j < width; ++j) {
//			scanf("%c", &arr[i*width + j]);
//		}
//		getchar();
//	}
//	//for (int i = 0; i < height*N; ++i) {
//	//	for (int j = 0; j < width; ++j) {
//	//		printf("%d ", arr[i*width + j]);
//	//	}
//	//	printf("\n");
//	//}
//
//	int min_sum=1000000000,min_1=0, min_2=0,sum=0;
//
//	for (int n = 0; n < N-1; ++n) {
//		for (int i = n + 1; i < N; ++i) {
//			sum = 0;
//			for (int j = 0; j < height*width; ++j) {
//				sum += abs(int(arr[n*height*width + j] - arr[i*height*width + j]));
//			}
//			//printf("%d\n", sum);
//			if (min_sum > sum) {
//				min_1 = n;
//				min_2 = i;
//				min_sum = sum;
//			}
//		}
//	}
//
//	printf("%d %d\n",min_1+1,min_2+1);
//	return 0;
//
//}
//
//void salary_loopang() {
//	int T = 0;
//	scanf("%d", &T);
//	for (int t = 0; t < T; ++t) {
//		unsigned long long year, month;
//		scanf("%llu %llu", &year, &month);
//
//		unsigned long long y_minus = year - 2016;
//		//year = (unsigned long long)(y_minus *) + 2016;
//		unsigned long long m_minus = y_minus * 12 + month;
//		year = 2016;
//		while (m_minus > 13000000000) {
//			m_minus = m_minus - 13000000000;
//			year += 1000000000;
//		}
//		while (m_minus > 130000000) {
//			m_minus = m_minus - 130000000;
//			year += 10000000;
//		}
//		while (m_minus > 1300000) {
//			m_minus = m_minus - 1300000;
//			year += 100000;
//		}
//		while (m_minus > 13000) {
//			m_minus = m_minus - 13000;
//			year += 1000;
//		}
//		while (m_minus > 130) {
//			m_minus = m_minus - 130;
//			year += 10;
//		}
//		while (m_minus > 13) {
//			m_minus = m_minus - 13;
//			year += 1;
//		}
//		printf("#%d %llu %llu\n", t + 1, (unsigned long long)year, (unsigned long long)m_minus);
//	}
//}
//
//unsigned long long sqr[1000001] = { 0 };
//
//void hyunju_root() {
//	int T = 0;
//	scanf("%d", &T);	
//	//unsigned long long sr[50] = { 0 };	
//	//sr[0] = 1;
//	for (int t = 0; t < T; ++t) {
//		unsigned long long in = 0, tmp = 0;
//		unsigned long long ans = 0;
//		scanf("%llu", &in);
//		unsigned long long i = 0;
//		while (in != 2) {
//			tmp = 0,i = 0;
//			while (in > tmp) {
//				i += (unsigned int)(sqrt(in)+0.9999999);
//				if (sqr[i] == 0) {
//					tmp = i * i;
//					sqr[i] = tmp;
//				}
//				else tmp = sqr[i];
//			}
//			ans += tmp - in + 1;
//			in = i;
//		}
//		printf("#%d %d\n", t + 1, ans);
//	}
//}
//
//int main() {
//	hyunju_root();
//	return 0;
//}