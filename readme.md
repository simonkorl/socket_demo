# Raw ip header server test

## experiment

1. compile: `make server`
2. start tshark monitoring loopback: `sudo tshark -i loopback`
3. start server: `sudo ./server`
4. get the following log: `1 0.000000000    127.0.0.1 → 127.0.0.1    TCP 54 1500 → 1700 [SYN] Seq=0 Win=2048 Len=0`

## reference

* raw header: https://blog.csdn.net/ExcaliburXK/article/details/7307324?utm_term=socket%E6%9E%84%E9%80%A0IP%E5%A4%B4&utm_medium=distribute.pc_aggpage_search_result.none-task-blog-2~all~sobaiduweb~default-0-7307324&spm=3001.4430
* tshark: https://www.cnblogs.com/liun1994/p/6142505.html