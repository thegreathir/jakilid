# jakilid

## Fast concurrent hash table that is placed in **shared memory** to share data between processes

jakilid uses [libcuckoo](https://github.com/efficient/libcuckoo) (a concurrent hash table implementation) as an underlying hash table and tries to initialize it on shared memory.
It also provides a Python binding along side main C++ API (using [Pybind11](https://github.com/pybind/pybind11)):
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
