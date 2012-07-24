#ifndef NODEMAP_H
#define NODEMAP_H
#include <cassert>
#include "Node.hh"
#include "Tree.hh"

  using namespace std;
  
  //-----------------------------------------------------------------
  //
  //! NodeMap is a class that offers a simple interface for mapping 
  //! nodes to auxiliary data of the type you need. 
  //! Author: Lars Arvestad, SBC, © the MCMC-club, SBC, all rights reserved
  //
  //-----------------------------------------------------------------
  template<typename C> class NodeMap 
  {
  public:
    //-----------------------------------------------------------------
    //
    // Construct, destruct, Assign
    //
    //-----------------------------------------------------------------
    NodeMap<C>();
    NodeMap<C>(const Tree &G);
    NodeMap<C>(const Tree &G, const C defval);
    NodeMap<C>(const NodeMap<C> &nm);
    ~NodeMap<C>();
    NodeMap<C>& operator=(const NodeMap<C> &nm); 

    //-----------------------------------------------------------------
    // 
    // Interface
    //
    //-----------------------------------------------------------------
    //
    //! \name Lookup
    //! Make it look like we are using an array.
    //-----------------------------------------------------------------
    //@{
    C& operator[](const Node &g);
    C& operator[](const Node *g);	// Overload to avoid too much type tripe
    C lookup(const Node *g);
    //@}
  protected:
    C* mapdata;
    unsigned array_size;
  };

  //-----------------------------------------------------------------
  //
  // Implementation   "cc"
  //
  //-----------------------------------------------------------------
  template<typename C> 
  NodeMap<C>::NodeMap()
  {
    mapdata = NULL;
    array_size = 0;
  }

  template<typename C> 
  NodeMap<C>::NodeMap(const Tree &G)
    : array_size(G.getNumberOfNodes())
  {
#ifdef DEBUG_SHOW_CONSTRUCTORS
    cerr<<"NodeMap constructor\n";
#endif
    mapdata = new C[array_size];

  }

  template<typename C> 
  NodeMap<C>::NodeMap(const Tree &G, const C defval)
    : array_size(G.getNumberOfNodes())
  {
#ifdef DEBUG_SHOW_CONSTRUCTORS
    cerr<<"NodeMap constructor 2\n";
#endif
    mapdata = new C[array_size];
    for (unsigned i=0; i<array_size; i++) 
      {
	mapdata[i] = defval;
      }
  }


  template<typename C> 
  NodeMap<C>::NodeMap(const NodeMap<C> &nm)
    : array_size(nm.array_size)
  {
    mapdata = new C[array_size];
    for (unsigned i=0; i < array_size; i++) 
      {
	mapdata[i] = nm.mapdata[i];
      }    
  }

  template<typename C> 
  NodeMap<C>::~NodeMap() 
  {
    delete[] mapdata;
  }


  template<typename C>
  NodeMap<C>& 
  NodeMap<C>::operator=(const NodeMap<C> &nm)
  {
    if (this->mapdata)
      {
	delete mapdata;
      }

    array_size = nm.array_size;

    mapdata = new C[array_size];
    for (unsigned i=0; i < array_size; i++) {
      mapdata[i] = nm.mapdata[i];
    }

    return *this;
  }


  template<typename C> 
  C&
  NodeMap<C>::operator[](const Node &g)
  {
    assert(g.getNumber() >= 0);
    assert(g.getNumber() <  array_size);

    return mapdata[g.getNumber()];
  }

  template<typename C> 
  C&
  NodeMap<C>::operator[](const Node *g) 
  {
    assert(g->getNumber() >= 0);
    assert(g->getNumber() <  array_size);

    return mapdata[g->getNumber()];
  }

  template<typename C> 
  C
  NodeMap<C>::lookup(const Node *g)
  {
    assert(g->getNumber() >= 0);
    assert(g->getNumber() <  array_size);

    return mapdata[g->getNumber()];
  }



#endif