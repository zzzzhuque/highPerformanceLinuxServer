//
// Created by zhuque on 2021/3/13.
//
#include <syslog.h>
#include <unistd.h>

/*
 * ubuntu 16.04日志输出到/var/log/syslog
 */

int main(int argc, char *argv[]) {
    setlogmask(LOG_UPTO(LOG_INFO));     ////< @note 调整日志等级，低于INFO的内容不会被记录

    openlog("exampleprog", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

    syslog(LOG_NOTICE, "Program started by User %d", getuid());
    syslog(LOG_INFO, "A tree falls in a forest");

    closelog();
}