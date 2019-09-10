package fire.pb.util;

import java.util.Objects;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicReferenceArray;

/**
 * @author liuziang
 * @contact liuziang@liuziangexit.com
 * @date 9/9/2019
 * @description 无锁HashMap，大小不可变，不提供remove操作，只有当你知道自己在做什么时才应该使用这个类。
 * 在给定的key没有出现地址冲突的情况下，对该key的put、get复杂度将是O(1)。否则，对该key的put、get复杂度最多可达O(n)。
 * 因此，给FixedLockFreeHashMap提供充足的空间和一个好的hash算法是重要的。当储存的元素数逼近负载因子(load factor)所指的上限时，put操作可能抛出异常以阻止加入新元素
 * @memory-order 对其的读写操作具有顺序一致语义（隐式获得自Java Atomic访问)
 * @参见 The Java® Language Specification - §17.4 "Memory Model"
 */

//TODO:删除功能、处理map满的情况
//删除功能这个类先不做，考虑是不是线性搜索算法不适合做删除功能
//支持自定义hash
//参考一个实现https://github.com/boundary/high-scale-lib/blob/master/src/main/java/org/cliffc/high_scale_lib/NonBlockingHashMap.java
//https://www.youtube.com/watch?v=HJ-719EGIts
//无锁hash结构的两个主要难点：
//1.hash冲突
//2.何时回收某些内存（因为它可能正被某个线程访问）http://erdani.com/publications/cuj-2004-12.pdf
//http://ifeve.com/hazard-pointer/
//3.考虑aba问题?

public class FixedLockFreeHashMap<K, V> implements java.util.concurrent.ConcurrentMap<K, V> {

	public FixedLockFreeHashMap(int capacity) {
		this(capacity, DEFAULT_LOAD_FACTOR);
	}

	public FixedLockFreeHashMap(int capacity, float loadFactor) {
		this.loadFactor = loadFactor;
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
		return this.getEntry(key.hashCode()) != null;
	}

	@Override
	public boolean containsValue(Object value) {
		throw new UnsupportedOperationException();
	}

	@Override
	@SuppressWarnings("unchecked")
	public V get(Object key) {
		InternalEntry entry = this.getEntry(key.hashCode());
		if (entry != null)
			return (V) entry.value;
		return null;
	}

	@Override
	@SuppressWarnings("unchecked")
	public V put(K key, V value) {
		return (V) this.putEntry(new InternalEntry(key.hashCode(), value)).value;
	}

	@Override
	public V remove(Object key) {
		throw new UnsupportedOperationException();
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
		throw new UnsupportedOperationException();
	}

	@Override
	public java.util.Collection<V> values() {
		throw new UnsupportedOperationException();
	}

	@Override
	public java.util.Set<Entry<K, V>> entrySet() {
		throw new UnsupportedOperationException();
	}

	@Override
	public boolean equals(Object o) {
		throw new UnsupportedOperationException();
	}

	@Override
	public int hashCode() {
		throw new UnsupportedOperationException();
	}

	@Override
	public V putIfAbsent(K key, V value) {
		throw new UnsupportedOperationException();
	}

	@Override
	public boolean remove(Object key, Object value) {
		throw new UnsupportedOperationException();
	}

	@Override
	public boolean replace(K key, V oldValue, V newValue) {
		throw new UnsupportedOperationException();
	}

	@Override
	public V replace(K key, V value) {
		throw new UnsupportedOperationException();
	}

	@Override
	public String toString() {
		return this.data.toString();
	}

	//implementation↓

	private InternalEntry putEntry(InternalEntry newEntry) {
		//weakCompareAndSet is not an alternative approach here because its doesn't provide any ordering guarantees

		if (newEntry.key == 0)
			throw new IllegalArgumentException();

		final int keyHash = newEntry.key;
		int keyHashIncreaseSequence = keyHash;
		InternalEntry previousEntry = null;
		for (int entryKey = keyHashIncreaseSequence % this.data.length(); previousEntry == null; entryKey = ++keyHashIncreaseSequence % this.data.length()) {
			do {
				//no fence needed(relaxed)
				previousEntry = (InternalEntry) this.data.get(entryKey);

				if (previousEntry.key == 0) {
					//execute this branch represent this entry is empty...
					//...add new entry into the map, try increase size first

					//check if the size is reaching load factor limit
					int peekSize;
					do {
						//no fence needed(relaxed)
						peekSize = this.size.get();
						if (peekSize + 1 > this.data.length() * this.loadFactor)
							throw new RuntimeException("load factor limit reached");
					}
					//need load-load & store-store & load-store fence(acquire-release)
					while (!this.size.compareAndSet(peekSize, peekSize + 1));
					//..try CAS
				} else if (previousEntry.key != keyHash) {
					//this entry has been used by other key(different hashes are mapped into same entry), try next entry(linear search strategy)
					previousEntry = null;
					break;
				}
				//otherwise...this key is already associated with a value...try CAS

				//need load-load & store-store & load-store fence(acquire-release)
			} while (!this.data.compareAndSet(entryKey, previousEntry, newEntry));
		}
		return previousEntry;
	}

	private InternalEntry getEntry(final int keyHash) {
		int keyHashIncreaseSequence = keyHash;
		for (int entryKey = keyHashIncreaseSequence % this.data.length(); ; entryKey = ++keyHashIncreaseSequence % this.data.length()) {
			InternalEntry entry = (InternalEntry) this.data.get(entryKey);
			if (entry.key == keyHash)
				return entry;//found it!
			if (entry.key == 0)
				return null;//if there is no value associated with the key
		}
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

		int key;
		Object value;

	}

	private AtomicReferenceArray<Object> data;
	private AtomicInteger size = new AtomicInteger(0);
	private final float loadFactor;

	// The load factor used when none specified in constructor
	static private final float DEFAULT_LOAD_FACTOR = 0.75F;

}