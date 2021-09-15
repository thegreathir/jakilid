# jakilid

## Fast concurrent hash table that is placed in **shared memory** to share data between processes

jakilid uses [libcuckoo](https://github.com/efficient/libcuckoo) as an underlying hash table and tries to initialize it on shared memory.
It also provides a Python wrapper along side C++ API:
```python
>>> import jakilid
>>> sd = jakilid.SharedDict("SD1")
>>> sd.insert("key", "value")
True
```

in another process:
```python
>>> import jakilid
>>> sd = jakilid.SharedDict("SD1")
>>> sd.find("key")
'value'
```
