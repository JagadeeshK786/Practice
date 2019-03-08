void lock_free_resize(int new_size) {
  auto array = m_current_array;
  auto copy = new int[array_length];
  for (int i = 0; i < array_length; i++)
    copy[i] = array[i];
  compare_and_swap(m_current_array, array, copy);
  //ABA?
  //when to delete?
}