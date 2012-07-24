#ifndef NODENODEMAP_HH
#define NODENODEMAP_HH

#include <assert.h>
#include "Tree.hh"
#include "Node.hh"
  
  using namespace std;

  //! Author: Lars Arvestad, SBC, © the MCMC-club, SBC, all rights reserved
  template<class T> 
  class NodeNodeMap 
  {
  public:
    NodeNodeMap() {K = 0;}
    NodeNodeMap(Tree &G, Tree &S) 
      : K(S.getNumberOfNodes()),
	data(K * G.getNumberOfNodes()) {};

    NodeNodeMap(Tree &G, Tree &S, const T& initValue) 
      : K(S.getNumberOfNodes()),
	data(K * G.getNumberOfNodes(), initValue) {};

    NodeNodeMap(const NodeNodeMap& f) 
      : K(f.K),
	data(f.data) {};

    ~NodeNodeMap() {};

    NodeNodeMap&
    operator=(const NodeNodeMap& f) 
    {
      if(this != &f)
	{
	  K = f.K;
	  data = f.data;
	}
      return *this;
    };

    const T& operator()(Node &g,  Node &s) const
    {
      unsigned id1 = g.getNumber();
      unsigned id2 = s.getNumber();
      unsigned idx = id1 * K + id2;

      assert(id2 < K);		// This should catch errors of putting nodes in 
                                // the wrong order.
                                // But hey, who would ever do that?! /arve
      assert(idx < data.size());
      assert(idx >= 0);

      return data[idx];
//       return operator()(g,s); 
    }

    T& operator()(Node &g,  Node &s)
    {
      unsigned id1 = g.getNumber();
      unsigned id2 = s.getNumber();
      unsigned idx = id1 * K + id2;

      assert(id2 < K);	 // This should catch errors of putting nodes in the
                         // wrong order.But hey, who would ever do that?! /arve
      assert(idx < data.size());
      assert(idx >= 0);

      return data[idx];
    };

    T& operator()(Node *g, Node *s)
    {
      unsigned id1 = g->getNumber();
      unsigned id2 = s->getNumber();
      unsigned idx = id1 * K + id2;

      assert(id2 < K);
      assert(idx < data.size());
      assert(idx >= 0);

      return data[idx];
    };

  private:
    unsigned       K;		// The largest id in S.
    std::vector<T> data;		// indexed by g->id * K + s->id for some K depending on S.

  };

#endif