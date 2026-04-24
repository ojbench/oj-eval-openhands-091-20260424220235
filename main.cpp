#include "src.hpp"
#include <iostream>
#include <cstdio>

int main() {
    ESet<long long> s[25];
    ESet<long long>::iterator it;
    int op, lst = 0, it_a = -1, valid = 0;
    
    while (scanf("%d", &op) != EOF) {
        long long a, b, c;
        
        switch (op) {
            case 0: {
                scanf("%lld%lld", &a, &b);
                auto p = s[a].emplace(b);
                if (p.second) {
                    it_a = a;
                    it = p.first;
                    valid = 1;
                }
                break;
            }
            case 1:
                scanf("%lld%lld", &a, &b);
                if (valid && it_a == a && *it == b) valid = 0;
                s[a].erase(b);
                break;
            case 2:
                scanf("%lld", &a);
                s[++lst] = s[a];
                break;
            case 3: {
                scanf("%lld%lld", &a, &b);
                auto it2 = s[a].find(b);
                if (it2 != s[a].end()) {
                    printf("true\n");
                    it_a = a;
                    it = it2;
                    valid = 1;
                } else {
                    printf("false\n");
                }
                break;
            }
            case 4:
                scanf("%lld%lld%lld", &a, &b, &c);
                printf("%zu\n", s[a].range(b, c));
                break;
            case 5:
                if (valid) {
                    auto it2 = it;
                    --it2;
                    if (it == it2) {
                        valid = 0;
                        printf("-1\n");
                    } else {
                        --it;
                        printf("%lld\n", *it);
                    }
                } else {
                    printf("-1\n");
                }
                break;
            case 6:
                if (valid) {
                    auto it2 = it;
                    ++it;
                    ++it2;
                    ++it2;
                    if (it == it2) {
                        valid = 0;
                    } else {
                        printf("%lld\n", *it);
                    }
                }
                if (!valid) {
                    printf("-1\n");
                }
                break;
        }
    }
    
    return 0;
}
