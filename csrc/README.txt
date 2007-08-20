Mostly complete. This is a translation of the JVM version.
Uses a simple instance counting gc (not reference counting
- common c++ technique) so the speed may not be as fast as
ref counting or other GC's but the native V implementation
is faster than the java V implementation.

