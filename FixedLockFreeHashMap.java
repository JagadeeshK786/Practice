package fire.pb.util;

import java.util.Objects;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicReferenceArray;

/**
 * @author liuziang
 * @contact liuziang@liuziangexit.com
 * @date 9/9/2019
 * @description 大小不可变的无锁Map，具有acquire-release语义（隐式获得此语义自Atomic访问)
 * @参见 The Java® Language Specification - §17.4.4 "Synchronization Order"
 */

//TODO:删除功能、处理map满的情况
//删除功能这个类先不做，考虑是不是线性搜索算法不适合做删除功能
//支持自定义hash
//参考一个实现https://github.com/boundary/high-scale-lib/blob/master/src/main/java/org/cliffc/high_scale_lib/NonBlockingHashMap.java
//无锁hash结构的两个主要难点：
//1.hash冲突
//2.何时回收某些内存（因为它可能正被某个线程访问）http://erdani.com/publications/cuj-2004-12.pdf
//3.考虑aba问题?

public class FixedLockFreeHashMap<K, V> implements java.util.Map<K, V> {

	public FixedLockFreeHashMap(int capacity) {
		this.data = new AtomicReferenceArray<>(capacity);
		//TODO: more cache-friendly allocation
		for (int i = 0; i < this.data.length(); i++)
			this.data.set(i, new InternalEntry(0, null));
	}

	@Override
	public int size() {
		return this.size.get();
	}

	@Override
	public boolean isEmpty() {
		return this.size.get() == 0;
	}

	@Override
	public boolean containsKey(Object key) {
		throw new RuntimeException();
	}

	@Override
	public boolean containsValue(Object value) {
		throw new UnsupportedOperationException();
	}

	@Override
	@SuppressWarnings("unchecked")
	public V get(Object key) {
        int keyHash = key.hashCode();
		int keyHashIncreaseSequence = keyHash;
		for (int entryKey = keyHashIncreaseSequence % this.data.length(); ; entryKey = ++keyHashIncreaseSequence % this.data.length()) {
			InternalEntry entry = (InternalEntry) this.data.get(entryKey);
			if (entry.key == keyHash)
				return (V) entry.value;//found it
			if (entry.key == 0)
				return null;//if there is no value associated with the key
		}
	}

	@Override
	@SuppressWarnings("unchecked")
	public V put(K key, V value) {
		return (V) this.putImpl(new InternalEntry(key.hashCode(), value)).value;
	}

	@Override
	public V remove(Object key) {
		throw new RuntimeException();
	}

	@Override
	public void putAll(java.util.Map<? extends K, ? extends V> m) {
		m.forEach(this::put);
	}

	@Override
	public void clear() {
		for (int i = 0; i < this.data.length(); i++)
			this.data.set(i, null);
	}

	@Override
	public java.util.Set<K> keySet() {
		throw new RuntimeException();
	}

	@Override
	public java.util.Collection<V> values() {
		throw new RuntimeException();
	}

	@Override
	public java.util.Set<Entry<K, V>> entrySet() {
		throw new RuntimeException();
	}

	@Override
	public boolean equals(Object o) {
		throw new RuntimeException();
	}

	@Override
	public int hashCode() {
		throw new RuntimeException();
	}

	private InternalEntry putImpl(InternalEntry newEntry) {
		int keyHash = key.hashCode();
		int keyHashIncreaseSequence = keyHash;
		InternalEntry previousEntry = null;
		for (int entryKey = keyHashIncreaseSequence % this.data.length(); previousEntry == null; entryKey = ++keyHashIncreaseSequence % this.data.length()) {
			do {
				previousEntry = (InternalEntry) this.data.get(entryKey);
				if (previousEntry.key != keyHash && previousEntry.key != 0) {
					//this entry has been used by other key(hash collision occurs), try next entry
					previousEntry = null;
					break;
				}
				//if this key is already associated with a value...
				//...or this entry is empty...
				//...try CAS
			} while (!this.data.weakCompareAndSet(entryKey, previousEntry, newEntry));
		}
		return previousEntry;
	}

	static private class InternalEntry {

		InternalEntry(int k, Object v) {
			this.key = k;
			this.value = v;
		}

		@Override
		public boolean equals(Object rhs) {
			if (!(rhs instanceof InternalEntry))
				return false;
			return this.hashCode() == rhs.hashCode();
		}

		@Override
		public int hashCode() {
			return Objects.hashCode(key);
		}

		public int key;
		public Object value;

	}

	private AtomicReferenceArray<Object> data;
	private AtomicInteger size = new AtomicInteger(0);

}
