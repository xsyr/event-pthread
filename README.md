event-pthread
=============

## Demo ##

Thread1:     
```c++
infra::thread::Event e;
sleep(60);
e.Signal();
...
e.Reset();
...
```


Thread2:     
```c++
e.Wait()/TimedWait(500);
```
