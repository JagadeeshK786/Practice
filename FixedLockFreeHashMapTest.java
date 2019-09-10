package fire.pb.util;

public class FixedLockFreeHashMapTest {

	static public void test() throws Exception {
		{
			FixedLockFreeHashMap<Long, Integer> map = new FixedLockFreeHashMap<>(4);
			if (map.size() != 0)
				throw new RuntimeException();
			if (map.get(1L) != null)
				throw new RuntimeException();
			map.put(1L, 1);
			if (map.size() != 1)
				throw new RuntimeException();
			if (map.get(1L) != 1)
				throw new RuntimeException();
			map.put(4L, 4);
			if (map.size() != 2)
				throw new RuntimeException();
			if (map.get(4L) != 4)
				throw new RuntimeException();
			if (map.put(5L, 5) != null)
				throw new RuntimeException();
			if (map.size() != 3)
				throw new RuntimeException();
			if (map.put(5L, 55) != 5)
				throw new RuntimeException();
			if (map.size() != 3)
				throw new RuntimeException();
			boolean isThrowed = false;
			try {
				//test 0.75 load factor
				map.put(6L, 6);
			} catch (Exception e) {
				isThrowed = true;
			}
			if (!isThrowed)
				throw new RuntimeException();
			if (map.size() != 3)
				throw new RuntimeException();
		}

		FixedLockFreeHashMap<Long, Integer> map2 = new FixedLockFreeHashMap<>(4, 1);
		if (map2.get(3L) != null)
			throw new RuntimeException();
		if (map2.size() != 0)
			throw new RuntimeException();
		if (map2.put(3L, 3) != null)
			throw new RuntimeException();
		if (map2.size() != 1)
			throw new RuntimeException();
		if (map2.get(3L) != 3)
			throw new RuntimeException();
		if (map2.put(4L, 4) != null)
			throw new RuntimeException();
		if (map2.size() != 2)
			throw new RuntimeException();
		if (map2.get(4L) != 4)
			throw new RuntimeException();
		if (map2.put(5L, 5) != null)
			throw new RuntimeException();
		if (map2.size() != 3)
			throw new RuntimeException();
		if (map2.get(5L) != 5)
			throw new RuntimeException();
		if (map2.put(5L, 55) != 5)
			throw new RuntimeException();
		if (map2.size() != 3)
			throw new RuntimeException();
		if (map2.get(5L) != 55)
			throw new RuntimeException();
		if (map2.put(6L, 6) != null)
			throw new RuntimeException();
		if (map2.size() != 4)
			throw new RuntimeException();
		if (map2.get(6L) != 6)
			throw new RuntimeException();
		if (map2.size() != 4)
			throw new RuntimeException();
		System.out.println("okok");
	}

}
