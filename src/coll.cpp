#include <iostream>
#include <deque>
#include <list>
#include <stdexcept>

int main()  // valid in C++, (void) in C
{
  std::list<std::deque<float> > list;

  for (int i = 1; i < 6; ++i)
  {
    std::deque<float> coll;
    for (int j = 1; j < 6; ++j)
    {
      coll.push_front(((float)i/j));
    }
    list.push_back(coll);

  }

  for (std::list<std::deque<float> >::const_iterator list_iter = list.begin();
       list_iter != list.end();
       ++list_iter)
  {
    for (std::deque<float>::const_iterator deque_iter = list_iter->begin();
         deque_iter != list_iter->end();
         ++deque_iter)
    {
      float item = *deque_iter;
      std::cout << item << ", ";
    }
    std::cout << std::endl;
  }

  return 0;
}
