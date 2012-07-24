#ifndef BEEPVECTOR_HH
#define BEEPVECTOR_HH

#include <cassert>
#include <vector>
#include <sstream>
#include "Node.hh"
#include "SetOfNodes.hh"
#include "Tree.hh"

namespace beep
{
  
  //--------------------------------------------------------------
  //
  //! Simple vector template, on top of vector template, indexed by Nodes
  //! This will always has elements in it but the value of the 
  //! elements may be empty
  //! This is handy in many ways!! Some 'aliases' for typed 
  //! BeepVectors are given below!
  //! \todo{Would it be better to implement BeepVector using inheritance?}
  //! Author: Bengt Sennblad, SBC, © the MCMC-club, SBC, all rights reserved
  //
  //--------------------------------------------------------------
  template<typename Type>
  class BeepVector
  {

  public:
    //--------------------------------------------------------------
    //
    // Construction/Destruction
    //
    //--------------------------------------------------------------
    BeepVector(unsigned n_elems)	//!< All elems are initialized to zero.
      : pv(n_elems)
    {
    };

    BeepVector(unsigned n_elems, Type& T) //!< All elems are initialized to T.
      : pv(n_elems, T)
    {
    };

    BeepVector(unsigned n_elems, const Type& T) //!< All elems are initialized to T.
      : pv(n_elems, T)
    {
    };

    BeepVector(const Tree& T)	//!< All elems are initialized to zero.
      : pv(T.getNumberOfNodes())
    {
    };

    BeepVector(const Tree& T, Type& type) //!< All elems are initialized to type.
      : pv(T.getNumberOfNodes(), type)
    {
    };

    BeepVector(const Tree& T, const Type& type) //!< All elems are initialized to type.
      : pv(T.getNumberOfNodes(), type)
    {
    };

    BeepVector(const BeepVector<Type>& bv)	
      : pv(bv.pv)
    {
    };

    virtual ~BeepVector()
    {
    };

    BeepVector& operator=(const BeepVector<Type>& bv)
    {
      if (this != &bv)
	{
	  pv = bv.pv;
	}
      return *this;
    };

    //--------------------------------------------------------------
    //
    // Interface
    //
    //--------------------------------------------------------------

    //! equality operator turned out to be practical when debugging
    //--------------------------------------------------------------
    bool operator==(const BeepVector<Type>& bv) const
    {
      bool ret = true;
      for(unsigned i = 0; i < size(); i++)
	ret = ret && (pv[i] == bv.pv[i]);
      return ret;
    }

    //! \name Access
    //! Vector is indexed by nodes or pointers to node!. 
    //! Also please note return value: 
    //! You can do v[node] = 0.5;
    //--------------------------------------------------------------
    //${
    Type& operator[](const Node& i)
    {
      return this->operator[](i.getNumber());
    }; 

    Type operator[](const Node& i) const
    {
      return this->operator[](i.getNumber());
    };

    Type& operator[](const Node* i)
    {
      assert(i != NULL);
      return this->operator[](*i);
    }; 

    Type operator[](const Node* i) const
    {
      assert(i != NULL);
      return this->operator[](*i);
    };

    //! Vector can also accessed by unsgined corresponding to node's number
    //--------------------------------------------------------------
    Type& operator[](unsigned i)
    {
      assert(i < pv.size());
#ifndef NDEBUG
      return pv.at(i);
#else
      return pv[i];
#endif
    }; 

    Type operator[](unsigned i) const
    {
      assert(i < pv.size());
#ifndef NDEBUG
      return pv.at(i);
#else
      return pv[i];
#endif
    };
    //@}

	
    //! Iterator for vector.
    typedef typename std::vector<Type>::iterator iterator;
	
    //! Const iterator for vector.
    typedef typename std::vector<Type>::const_iterator const_iterator;
	
    //! Reverse iterator for vector.
    typedef typename std::vector<Type>::reverse_iterator reverse_iterator;
		
    //! Const reverse iterator for vector.
    typedef typename std::vector<Type>::const_reverse_iterator const_reverse_iterator;
	
    //! Iterator to first element in vector.
    iterator begin() { return pv.begin(); };

    //! Const iterator to first element in vector.
    const_iterator begin() const { return pv.begin(); };
	
    //! Iterator to end of vector.
    iterator end() { return pv.end(); };

    //! Const iterator to end of vector.
    const_iterator end() const { return pv.end(); };

    //! Reverse iterator to last element in vector.
    reverse_iterator rbegin() { return pv.rbegin(); };

    //! Const reverse iterator to last element in vector.
    const_reverse_iterator rbegin() const { return pv.rbegin(); };
		
    //! Reverse iterator to reverse end of vector.
    reverse_iterator rend() { return pv.rend(); };

    //! Const reverse iterator to reverse end of vector.
    const_reverse_iterator rend() const { return pv.rend(); };
	
    //! Size of Vector
    virtual unsigned size() const
    {
      return pv.size();
    }
    //! reset all values
    void clearValues()
    {
      pv = ::std::vector<Type>(pv.size());
    }

    //--------------------------------------------------------------
    //
    //! \name I/O
    //
    //--------------------------------------------------------------
    //@{
    friend std::ostream& operator<<(std::ostream &o, 
				    const BeepVector<Type>& e)
    {
      return o << e.print();
    };

    //! \todo{This could perhaps be improved to give a table mapping
    //! a node to its content}
    std::string print() const
    {
      std::ostringstream oss;
      for(unsigned i = 0; i < size(); i++)
	{
	  oss << pv[i] << ";\t";
	}
      return oss.str();
    }
    //@}

  protected:
    //--------------------------------------------------------------
    //
    // Attributes
    //
    //--------------------------------------------------------------
    std::vector<Type> pv; //!< The storage
  };


  //! \name Provide alias for instantiations of BeepVector
  //--------------------------------------------------------------
  //@{
//   typedef beep::BeepVector<Probability> ProbVector; //initializes elems to 0
  typedef beep::BeepVector<unsigned> UnsignedVector;//initializes elems to 0
  typedef beep::BeepVector<beep::Node*> NodeVector; //initializes elems to NULL
//   typedef beep::BeepVector<beep::NodeExtended*> NodeVectorEx; //initializes elems to NULL
//   typedef beep::BeepVector<beep::LA_Vector> LikeVector; //init elems to (0,.,0)
  //@}


  //--------------------------------------------------------------
  //
  // Subclass
  //
  //--------------------------------------------------------------
  class RealVector: public BeepVector<Real>
  {
  public:
    RealVector(unsigned n_elems)	//!< All elems are initialized to zero.
      : BeepVector<Real>(n_elems),
	theSize(n_elems)
    {};

    RealVector(unsigned n_elems, Real& T) //!< All elems are initialized to T.
      : BeepVector<Real>(n_elems, T),
	theSize(n_elems)
    {};

    RealVector(unsigned n_elems, const Real& T) //!< All elems are initialized to T.
      : BeepVector<Real>(n_elems, T),
	theSize(n_elems)
    {};

    RealVector(const Tree& T)	//!< All elems are initialized to zero.
      : BeepVector<Real>(T),
	theSize(T.getNumberOfNodes())
    {};

    RealVector(const Tree& T, Real& type) //!< All elems are initialized to type.
      : BeepVector<Real>(T, type),
	theSize(T.getNumberOfNodes())

    {};

    RealVector(const Tree& T, const Real& type) //!< All elems are initialized to type.
      : BeepVector<Real>(T, type),
	theSize(T.getNumberOfNodes())

    {};

    RealVector(const RealVector& bv)	
      : BeepVector<Real>(bv),
	theSize(bv.theSize)

    {};

    ~RealVector()
    {};

    void addElement(unsigned i, const Real& T) 
    {
      assert(i <= theSize);
      if(theSize == i)
	{
	  theSize++;
	  while(pv.size() < theSize)
	    {
	      pv.resize(2 * pv.size(), 0);
	    }
	}
      pv[i] = T;
    }

    void rmElement(unsigned i)
    {
      // Copy last active element to the position of element to remove
      std::copy(pv.begin() + size()-1, pv.begin() + size(), pv.begin() + i);
      theSize--;
    }

    //! Size of Vector
    unsigned size() const
    {
      return theSize;
    }
  protected:
    //--------------------------------------------------------------
    //
    // Attributes
    //
    //--------------------------------------------------------------
    unsigned theSize;

  };

}// end namespace beep


#endif
