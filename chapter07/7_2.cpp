//
// Created by zhuque on 2021/3/13.
//
#include <unistd.h>
#include <cstdio>

static bool switch_to_user(uid_t user_id, gid_t gp_id) {            ////< @note 入参为待转换的用户身份
    if ((user_id == 0) && (gp_id == 0)) {       ////< @note 先确保目标用户不是root
        return false;
    }

    gid_t gid = getgid();                       ////< @note 获取真实组ID
    uid_t uid = getuid();                       ////< @note 获取真实用户ID
    if ((gid!=0 || uid!=0) && (gid!=gp_id || uid!=user_id)) {       ////< @note 若不是root用户也不是目标用户，则认为是非法用户
        return false;
    }

    if (uid != 0) {                             ////< @note 如果不是root，则已经是目标用户，即不支持从普通用户切换到普通用户
        return true;
    }

    if (setgid(gp_id) < 0 || setuid(user_id) < 0) {                 ////< @note 切换到目标用户
        return false;
    }

    return true;
}

int main() {
    if (switch_to_user(1000, 1000)) {
        printf("userid is %d, effective userid is: %d\n", getuid(), geteuid());
    }
    return 0;
}