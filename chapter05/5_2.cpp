//
// Created by zhuque on 2021/3/9.
//
#include <cstdio>
#include <arpa/inet.h>

int main() {
    in_addr addr1, addr2;
    addr1.s_addr = inet_addr("1.2.3.4");
    addr2.s_addr = inet_addr("10.194.71.60");
    char *szValue1 = inet_ntoa(addr1);
    char *szValue2 = inet_ntoa(addr2);
    printf("address 1: %s\n", szValue1);
    printf("address 2: %s\n", szValue2);
    return 0;
}