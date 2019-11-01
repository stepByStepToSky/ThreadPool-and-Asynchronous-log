## 异步日志原理文档

- writefile 负责直接写入原始数据到日志文件中，由于描述符fd不带缓冲区，增加应用层缓冲区来减少频繁调用write进行磁盘数据写入。
- logfile 负责日志文件的滚动。
- asynclog 采用双缓冲区和前后台线程对日志进行收集和写入(参考muduo的缓冲区实现)。
- log 采用单例来管理日志，保证进程只能打开一个日志文件进行写入。




## 参考资料
- [解密Redis持久化](https://searchdatabase.techtarget.com.cn/7-19848/)
- [LevelDb](https://github.com/google/leveldb/blob/3ea9147c575dd8d509ed2596a4d67ec32405c6a1/util/env_posix.cc)
- [Muduo](https://github.com/chenshuo/muduo/tree/master/muduo/base)
