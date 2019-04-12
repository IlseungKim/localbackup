//#include <cstdio>
//#include <vector>
//
//using namespace std;
//
//vector<char> fact(vector<char> n) {
//	char tmp = n[0] - 1;
//	unsigned long long i = 1;
//	int carry = 0;
//	if (n[0] != 0) {
//		carry = n[0] * (n[0] - 1);
//		n[0] = carry % 10;
//	}
//	else {
//		
//	}
//	while (1) {
//		
//	}
//}
//
//
//int main() {
//	int T = 0;
//	scanf("%d", &T);
//	for (int t = 0; t < T; ++t) {
//		unsigned long long tmp[2];
//		scanf("%llu %llu", &tmp[0], &tmp[1]);
//		vector<char> n, k;
//		unsigned long long i = 10;
//		while (tmp[0] / i >= 0) { i *= 10; }
//		while (i >= 10) {
//			n.push_back(tmp[0]/i);
//			i /= 10;
//		}
//		while (tmp[1] / i >= 0) { i *= 10; }
//		while (i >= 10) {
//			k.push_back(tmp[1] / i);
//			i /= 10;
//		}
//	}
//	return 0;
//}