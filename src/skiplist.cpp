#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <stdexcept>
#include <boost/random.hpp>
#include <ctime>
#include <algorithm>

template <typename Tval> class SkipListElement;

template <typename Tval>
class SkipListElement
{
public:
  typedef std::map<int, SkipListElement<Tval>* > SkipListEdges;

  Tval value;
  int next_count;
  int prev_count;
  int counter;

  SkipListElement():
    value(0),
    next_count(0),
    prev_count(0)
  {
  }

  SkipListElement(Tval val):
    value(val),
    next_count(0),
    prev_count(0)
  {
    static int instance_counter = 0;
    ++instance_counter;
   this->counter = instance_counter;
  }

  virtual ~SkipListElement() throw()
  {
  }

  void set_next_at(int level, SkipListElement<Tval>* elem) throw(std::logic_error)
  {
    if (elem == this)
      throw std::logic_error("Cannot add self-reference.");

    this->next_elements[level] = elem;
  }

  void set_prev_at(int level, SkipListElement<Tval>* elem) throw(std::logic_error)
  {
    if (elem == this)
      throw std::logic_error("Cannot add self-reference.");

    this->prev_elements[level] = elem;
  }

  void unset_next_at(int level)
  {
    // check for range?
    this->next_elements.erase(level);
  }

  void unset_prev_at(int level)
  {
    // check for range?
    this->next_elements.erase(level);
  }

  SkipListElement<Tval>* get_next_at(int level)
  {
    return this->next_elements[level];
  }

  SkipListElement<Tval>* set_next_at(int level)
  {
    return this->next_elements[level];
  }


private:
  SkipListEdges prev_elements;
  SkipListEdges next_elements;

  void append_follower(SkipListElement<Tval>* elem) throw(std::logic_error)
  {
    if (elem != this)
      {
        this->next_elements.push_back(elem);
        ++this->followers;
      }
    else
      throw std::logic_error ("Cannot append self-reference as follower!");
  }
};


template <typename Tval>
class SkipList
{
public:
  typedef std::map<int, SkipListElement<Tval>* > SkipListEdges;

  int max_level;

  SkipList():
    max_level(0),
    new_index_probability(0.5),
    find_steps(0)
  {
  }

  SkipList(double probability) throw (std::logic_error):
    max_level(0),
    new_index_probability(probability),
    find_steps(0)
  {
    if (0.0 > probability|| 1.0 <= probability)
      throw std::logic_error("probability out of [0,1)!");
  }

  virtual ~SkipList() throw()
  {
     // huh, lots of cleaning up code will have to go here ...
  }

  void print()
  {
    std::cout
      << std::endl
      << "Skiplist contents (last find() run for "
      << this->find_value << " took "
      << this->find_steps << " steps): "
      << std::endl;

    for (int level = this->max_level; level > -1; --level)
    {
      std::cout << "level " << level << ":  ";
      SkipListElement<Tval>* head = this->first_elements[level];
      while(head)
      {
         std::cout << head->value << "  ";
         head = head->get_next_at(level);
      }
      std::cout << std::endl;
    }
  }

  void print_last_visited_elements()
  {
    std::cout << "max_level: " << this->max_level << std::endl;
    for (int level = this->max_level; level > -1; --level)
    {
      if (this->last_visited[level])
        std::cout << "last_visited[" << level << "] contains " << this->last_visited[level]->value << std::endl;
      else
        std::cout << "last_visited[" << level << "] not set!" << std::endl;
    }
  }

  void print_first_elements()
  {
    std::cout << "max_level: " << this->max_level << std::endl;
    for (int level = this->max_level; level > -1; --level)
    {
      if (this->first_elements[level])
      {
        std::cout << "first_elements[" << level << "] contains " << this->first_elements[level]->value << std::endl;
        if (this->first_elements[level]->get_next_at(level))
          std::cout << "followed by " << this->first_elements[level]->get_next_at(level)->value << std::endl;
      }
      else
        std::cout << "first_elements[" << level << "] not set!" << std::endl;
    }
  }

  void insert (const Tval& value)
  {
    SkipListElement<Tval>* insert = new SkipListElement<Tval>(value);

    // find() updates last_visited for us, so it is very important to run it
    // before any insertion!
    if (this->find(value))
      return;

    // Skiplist is empty => first insertion.
    // Actually we would have to flip the coin for this (repeated) insertion as
    // well ...
    if (!this->get_first_element())
    {
      this->first_elements[0] = insert;
    }
    else
    {
      this->insert_element(insert);
    }
  }

  void insert_element(SkipListElement<Tval>* insert)
  {
    // This loop potentially never ends, or "The endless fun of randomness" =)
    // The level < 1 guard guarantees that we run through this loop at least
    // once (for level = 0, else nothing ever gets inserted).
    for (int level = 0; level < 1 || this->flip_a_coin(); ++level)
    {
      this->max_level = std::max(this->max_level, level);

      // If we haven't visited at least one element on this level we know that
      // we have to append this element to the front of the list.
      // Else a normal insertion takes place.
      if (!this->last_visited[level])
      {
        if (this->first_elements[level])
          insert->set_next_at(level, this->first_elements[level]);
        this->first_elements[level] = insert;
      }
      else
      {
        insert->set_next_at(level, this->last_visited[level]->get_next_at(level));
        this->last_visited[level]->set_next_at(level, insert);
      }
    }
  }


  bool find(const Tval& value)
  {
    // Just a helper metric, to see how fast we are.
    this->find_steps = 0;
    this->find_value = value;

    // We have to empty the last_visited map for each run of find.
    this->last_visited.clear();
    SkipListElement<Tval>* head = this->get_first_element();

    // Initial check to see whether value must be appended to front
    if (!head || this->first_elements[0]->value > value)
      return false;

   // head is smaller/equal to value -> visit
   this->last_visited[this->max_level] = head;
   ++(this->find_steps);

   // head is valid
   for (int level = this->max_level; level > -1; --level)
   {
     // The first entry in the list[level+1] only contained bigger elements,
     // restart at first element of list[level]
     if (head->value > value)
     {
       // This begs the question on how to reset last_visited for level+1
       // when we overshot (or even better, prevent overshooting!).
       // The bug becomes visible quickly once we start to look at greater
       // sequences (unordered lists for level > 0).
       // It means that last_visited will sometimes be bigger than value, which
       // is a logical error. The effect is that we effectivly truncat the
       // level+1 list upon insertion, thus making find() very costly (as in O(n)).
       head = this->first_elements[level];
     }

     this->last_visited[level] = head;

     while (head->value <= value)
     {
       this->last_visited[level] = head;
       SkipListElement<Tval>* next = head->get_next_at(level);
       if (!next)
         break;
       head = next;
       ++(this->find_steps);
     }
   }
   return (this->last_visited[0]->value == value ||
           this->first_elements[0]->value == value);
  }

  SkipListElement<Tval>* get_first_element()
  {
    return this->first_elements[this->max_level];
  }

private:

  SkipListEdges first_elements;
  SkipListEdges last_visited;
  double new_index_probability;
  int find_steps; // metric
  Tval find_value; // metric

  // If the probability is not 0.5 then this coin is loaded ...
  bool flip_a_coin ()
  {
    static boost::mt19937 rng(static_cast<unsigned>(std::time(0)));
    static boost::uniform_real<> uni_dist(0,1);
    static boost::variate_generator<boost::mt19937, boost::uniform_real<> > uni(rng, uni_dist);

    return (uni() > this->new_index_probability) ? false : true;
  }
};

// Test program, insert R random elements with range [0, R).
int main ()
{
  SkipList<int>* sl = new SkipList<int>(.5);

  sl = new SkipList<int>(.5);

  srand(std::time(0));
  const int range = 100;
  const int size = RAND_MAX / range;
  for (int i = 0; i < range; ++i)
  {
    int r;
    do
    {
       r = rand() / size;
    }
    while ( r >= range);

    sl->insert(r);
    sl->print();
  }

  return 0;
}
