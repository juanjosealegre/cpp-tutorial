#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <istream>
#include <algorithm>
#include <stdexcept>
#include <boost/random.hpp>
#include <ctime>

typedef std::vector<std::string> Rule;
typedef std::vector<Rule> RuleCollection;
typedef std::map<std::string, RuleCollection> Grammar;

bool space (char c);

bool
space (char c)
{
  return isspace (c);
}

bool not_space(char c);

bool
not_space(char c)
{
  return !space(c);
}

std::vector<std::string> split (const std::string& str);

std::vector<std::string>
split (const std::string& str)
{
  typedef std::string::const_iterator iter;
  std::vector<std::string> result;

  iter it = str.begin();
  while (it != str.end())
  {
    it = find_if (it, str.end(), not_space);
    iter jt = find_if (it, str.end(), space);

    if (it != str.end())
      result.push_back(std::string(it, jt));
    it = jt;
  }
  return result;
}

Grammar read_grammar (std::istream &in);

Grammar
read_grammar (std::istream &in)
{
  Grammar my_grammar;
  std::string line;

  while (getline (in, line))
  {
    std::vector<std::string> entry = split (line);
    if (!entry.empty())
      my_grammar[entry[0]].push_back (Rule (entry.begin() + 1, entry.end()));
  }
  return my_grammar;
}

bool bracketed (const std::string &s);

bool
bracketed (const std::string &s)
{
  return (1 < s.size() && '<' == s[0] && '>' == s[s.size() - 1]);
}

int nrand (int n);

int nrand (int n)
{
  /*
  static bool is_seeded = false;
  if (!is_seeded)
    {
      srand(time(0));
      is_seeded = true;
    }
  */
  static boost::mt19937 rng(static_cast<unsigned>(std::time(0)));

  if (0 >= n || RAND_MAX < n)
    throw std::logic_error ("Argument to nrand is out of range!");

  const int bucket_size = RAND_MAX / n;
  int r;

  do
  {
    r = rng() / bucket_size;
    //r = rand() / bucket_size;
  }
  while (r >= n);

  //std::cout << n << ": " << r << std::endl;

  return r;
}


void gen_aux (const Grammar &g, const std::string &word, std::vector<std::string> &result);

void
gen_aux (const Grammar &g, const std::string &word, std::vector<std::string> &result)
{
  if (!bracketed (word))
    {
      result.push_back (word);
    }
  else
    {
      Grammar::const_iterator it = g.find (word);

      if (g.end() == it)
        throw std::logic_error ("empty grammar rule");

      const RuleCollection &rc = it->second;
      const Rule &r = rc[nrand(rc.size())];

      for (Rule::const_iterator r_it = r.begin(); r_it != r.end(); ++r_it)
        gen_aux (g, *r_it, result);
    }
}

std::vector<std::string> gen_sentence (const Grammar &g);

std::vector<std::string>
gen_sentence (const Grammar &g)
{
  std::vector<std::string> result;
  gen_aux (g, "<sentence>", result);
  return result;
}

int main ()
{
  std::vector<std::string> sentence = gen_sentence (read_grammar (std::cin));
  std::vector<std::string>::const_iterator it = sentence.begin();


  if (!sentence.empty())
    {
      std::cout << *it;
      ++it;
    }

  while (sentence.end() != it)
  {
    std::cout << " " << *it;
    ++it;
  }

  std::cout << std::endl;
  return 0;
}
