/*
    PrimeTV2 : a visualizer for phylogenetic reconciled trees.
    Copyright (C) 2011  <Jose Fernandez Navarro> <jc.fernandez.navarro@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
    
    Author : Jose Fernandez Navarro  -  jc.fernandez.navarro@gmail.com
    
    *********************************************************************
    This class is based on the program created by : Ali Tofigh
    
    Copyright (C) 2010, 2011 Ali Tofigh

    This file is part of PhylTr, a package for phylogenetic analysis
    using duplications and transfers.

    PhylTr is released under the terms of the license contained in the
    file LICENSE.
    *********************************************************************

*/


#ifndef PHYLTR_H
#define PHYLTR_H

#include <cstdlib>
#include <cerrno>
#include <iostream>
#include <algorithm>
#include <utility>
#include <vector>
#include <stack>
#include <set>
#include <bitset>

#include <boost/smart_ptr.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/foreach.hpp>

#include <fstream>

#include "../tree/Treeextended.h"

using namespace std;
using boost::shared_ptr;
using boost::dynamic_bitset;
using boost::multi_array;
typedef unsigned vid_t;
typedef float cost_type;

//*****************************************************************************
// global variables
//
// g_options
//      Contains the command line options passed to the
//      program. Should not really be needed as the options are all
//      copied to g_input.
//
// g_input
//      Holds the input to the program. This includes both flags,
//      filenames, and the data contained in the
//      files. gene_tree_numbering is used to number the gene tree
//      vertices for output, i.e., gene_tree_numbering[u] is the
//      number of vertex u when printing solutions.
//
// g_below
// g_outside
//      The DP matrices. For a gene tree vertex u and a species
//      tree vertex x, g_below[u][x] is the minimum cost of placing u
//      at a descendant of x (possibly at x itself), and
//      g_outside[u][x] is the minimum cost of placing u at a vertex
//      incomaparable to x.
//
// g_backtrack_matrix
//      Holds information needed during backtracking. See description
//      of BacktrackElement for more details.
//*****************************************************************************



//*****************************************************************************
// class Candidate
//
// A Candidate object represents a partial solution of a
// reconciliation. It keeps track of duplications, transfers, cost,
// s-moves, and the lca-mapping of the gene tree into the species tree
// given the events. It also keeps track of the forest that is
// obtained from the gene tree by removing transfer edges and
// contracting any vertices with only one child. Finally, it is also
// able to compute whether or not the candidate is elegant, i.e.,
// whether or not a duplication or transfer is unnecessary.
//
// Most member functions are self explanatory. Here are some notes
// that should be mentioned.
//
// It is important to know that the candidate is designed to behave
// intelligently with respect to duplications and transfers. Already
// during construction of a candidate, the object computes the forced
// duplications (i.e., the internal gene tree vertices that are mapped
// to species tree leaves). When marking an edge as a transfer edge
// and when marking a vertex as a duplication, any resulting forced
// duplications are also set.
//
// Note that is_elegant() assumes that no moves remain, i.e., it
// assumes that the candidate is final.
//
// The member functions parent(), left(), and right() are used to gain
// information about the gene tree forest described above. parent(u)
// is defined for all gene tree vertices u and returns the least
// proper ancestor of u that is not a transfer vertex (i.e., no
// outgoing edge is a transfer edge). left(u) and right(u) are only
// defined for non-transfer vertices, and return tree_type::NONE when
// u is a transfer vertex.
//*****************************************************************************

class Candidate {
public:

    class bad_transfer_exception : public exception {};
    class bad_duplication_exception : public exception {};

    Candidate();
    
    void compute_highest_mapping_(vector<vid_t> &) const;
    void set_transfer_edge(vid_t);
    void set_duplication(vid_t);
    bool is_transfer_edge(vid_t) const;
    bool is_duplication(vid_t) const;
    double cost() const;
    bool is_elegant() const;
    vid_t get_s_move() const;
    vid_t lambda(vid_t) const;
    vid_t parent(vid_t) const;
    vid_t left(vid_t) const;
    vid_t right(vid_t) const;
    std::vector<vid_t> getLambda() const;
    
    dynamic_bitset<> getTransferEdges();
    dynamic_bitset<> getDuplications();
    
    Candidate& operator=(const Candidate &cp);
    
    void copy(Candidate *cp);
    
private:
    dynamic_bitset<> duplications_;
    dynamic_bitset<> transfer_edges_;
    double cost_;
    vector<vid_t> lambda_;
    mutable vector<vid_t> s_moves_;
    vector<vid_t> P_;
    vector<vid_t> left_;
    vector<vid_t> right_;

    bool is_s_move_(vid_t) const;

    friend ostream &operator<<(ostream &, const Candidate &);
    
    
};

//*****************************************************************************
// class Scenario
//
// A Scenario object is used to keep track of the duplications and
// transfer edges of solutions during backtracking. A Scenario object
// may represent a partial solution. Throughout the program, an edge
// is represented by the vertex at its head, i.e., the vertex farthest
// away from the root.
//
// The operator< is used to sort the scenarios for printing. It sorts
// first on the number of transfers, then on the number of losses, and
// lastly according to lexicographic order.
//*****************************************************************************

class Scenario {
public:
    dynamic_bitset<> duplications;
    dynamic_bitset<> transfer_edges;
    Candidate cp;

    Scenario(unsigned size);

};


//*****************************************************************************
// class BacktrackElement
//
// Used to hold the information needed for backtracking after the
// dynamic programming algorithm has run. For gene tree vertex u and
// species tree vertex x, the members have the following meaning:
//
// The enums denote biological events that can be associated with a
// gene tree vertex and a species tree vertex. The below_events member
// is used to keep track of the events that led to the optimal cost of
// placing u at a descendant of x (possibly x itself):
//
// S
//      A speciation with the left child of u below left child of x,
//      and right child of u below right child of x.
//
// S_REV
//      A speciation with left child of u below right child of x, and
//      right child of u below left child of x.
//
// D
//      A duplication
//
// T_LEFT
//      The left edge of u is a transfer edge.
//
// T_RIGHT
//      The right edge of u is a transfer edge.
//
// BELOW_LEFT
//      The optimum cost can be achieved by placing u at a descendant
//      of the left child of x.
//
// BELOW_RIGHT
//      The optimum cost can be achieved by placing u at a descendant
//      of the right child of x.
//
// below_events:
//      For an Event e, below_events[e] is set iff the event
//      represented by e led to the optimal cost of placing u at a
//      descendant of x (possibly x itself). This member is set during
//      the dynamic programming algorithm.
//
// outside_sibling:
//      The vid_t of the sibling of x, if placing u below the sibling
//      gives the optimal cost of placing u outside x, and
//      tree_type::NONE otherwise. This member is set during
//      the dynamic programming algorithm.
//
// outside_ancestor:
//      The vid_t of the nearest proper ancestor of x such that
//      placing u below the sibling of the ancestor gives the optimal
//      cost of placing u outside x, and tree_type::NONE if no such
//      ancestor exists. This member is set during the dynamic
//      programming algorithm.
//
// min_transfers:
//      The minimum number of transfers required for obtaining the
//      optimum cost of placing u at a descendant of x. This is used
//      when the --minimum-transfers flag has been set.
//
// scenarios_below_needed:
//      Set to true iff we need to compute all the scenarios
//      corresponding to placing u at a descendant of x.
//
// scenarios_at_needed:
//      Set to true iff we need to compute all the scenarios
//      corresponding to placing u _at_ x.
//
// below_placements:
//      The set of descendants of x at which u can be placed to optain
//      the optimum cost. If x is itself among this set, then it must
//      always be the first element in the vector.
//
// scenarios_at:
//      The set of scenarios corresponding to placing u _at_ x.
//*****************************************************************************

class BacktrackElement {
public:
    enum Event {S, S_REV, D, T_LEFT, T_RIGHT, BELOW_LEFT, BELOW_RIGHT, N_EVENTS};

    bitset<N_EVENTS> below_events;
    vid_t outside_sibling;
    vid_t outside_ancestor;
    unsigned min_transfers;
    bool scenarios_below_needed;
    bool scenarios_at_needed;
    vector<vid_t> below_placements;
    vector<Scenario> scenarios_at;

    BacktrackElement();
};
struct ProgramInput {
    string species_tree_fname;
    string gene_tree_fname;
    string sigma_fname;
    double min_cost;
    double max_cost;
    TreeExtended *species_tree;
    TreeExtended *gene_tree;
    vector<unsigned> sigma;
    vector<unsigned> gene_tree_numbering;
    vector<unsigned> species_tree_numbering;
    double duplication_cost;
    double transfer_cost;
    bool unsorted;
    bool print_only_minimal_transfer_scenarios;
    bool print_only_minimal_loss_scenarios;
};

class Phyltr
{

public:

    void print_error(const char *);
    //*****************************************************************************
    // read_sigma()
    //
    // Constructs the mapping g_input.sigma of the gene tree leaves to the
    // species tree leaves by reading the file whose filename is given in
    // g_input.sigma_fname. If any errors are detected, an apropriate error
    // message is written to stderr and false is returned. Otherwise, true
    // is returned.
    //*****************************************************************************
    bool read_sigma();
    bool read_sigma(map<string, string> str_sigma);
    //*****************************************************************************
    // fpt_algorithm()
    //
    // Runs the fixed-parameter-tractable algorithm. For details see the
    // relevant article cited in the beginning of the file. The vector
    // passed to the function is filled with Scenario options.
    // //*****************************************************************************
    void fpt_algorithm();

    /* print all the scenarios */
    void printScenarios();

    /* print the scenario given */
    void printScenario(Scenario &sc);

    /* print the candidate given */
    void printCandidate(Candidate &c);

    /* returns the scenario with the maximum cost */
    Scenario getMaxCostScenario();

    /* returns the scenario with the minimum cost */
    Scenario getMinCostScenario();

    /* print the lambda vector */
    void printLambda(std::vector<vid_t> lambda);
    /************************/
    void dp_algorithm();
    //*****************************************************************************
    // compute_below()
    // compute_outside()
    //
    // These are helper functions used by dp_algorithm.
    //*****************************************************************************
    void compute_below(vid_t u, vid_t x);
    void compute_outside(vid_t u, vid_t x);
    //*****************************************************************************
    // backtrack()
    //
    // Runs the backtrack algorithm and finds all optimal
    // DTL-scenarios. Must be called after dp_algorithm(). The scenarios
    // are returned via the return vector that is passed as argument. The
    // flags g_input.print_only_minimal_transfer_scenarios and
    // g_input.print_only_minimal_loss_scenarios affect the behaviour of
    // backtrack(). If both flags are set, the scenarios with minimal
    // transfers are found first, and among these, the ones with minimal
    // number of losses are inserted into the return vector.
    //*****************************************************************************
    void backtrack();
    //*****************************************************************************
    // backtrack_below_placements()
    // backtrack_outside_placements()
    // backtrack_mark_needed_scenarios_below()
    // backtrack_min_transfers()
    // backtrack_scenarios_at()
    // combine_scenarios()
    //
    // These are helper functions called by backtrack(). Since no
    // documentation is yet available on how the backtrack algorithm
    // works, some short descriptions about what the functions do is given
    // here. u denotes a gene tree vertex and x a species tree vertex.
    //
    // backtrack_below_placements(u, x)
    //      Finds the descendants of x _at_ which u can be placed to
    //      obtain the minimal cost g_below[u][x]. Inserts the vertices
    //      into g_backtrack_matrix[u][x].below_placements. If x is one of
    //      the descendants at which u may be placed, it will be the first
    //      vertex inserted. This function assumes that the
    //      below_placements of the descendants of u and descendants of x
    //      have already been computed.
    //
    // backtrack_outside_placements()
    //      Finds the vertices incomparable to x _below_ which u may be
    //      placed to obtain the minimal cost g_outside[u][x]. The
    //      vertices are inserted into the vector that is passed as
    //      argument. This function only relies on the outside_sibling and
    //      outside_ancestor members of BacktrackElement.
    //
    // backtrack_mark_needed_scenarios_below()
    //      Determines which scenarios need to be computed if u is to be
    //      placed _at_ x. This function sets the members
    //      scenarios_below_needed, and in the case of duplications,
    //      some scenarios_at_needed, of BacktrackElement.
    //
    // backtrack_min_transfers()
    //      This function computes the minimal number of transfers that
    //      are needed when placing u below x. It assumes that the minimum
    //      transfers for descendants of u and x have already been
    //      computed.
    //
    // backtrack_scenarios_at()
    //      This function actually computes the minimal cost scenarios
    //      where u is placed _at_ x, and inserts these into
    //      g_backtrack_matrix[u][x].scenarios_at. This function assumes
    //      that the needed scenarios for all descendants of u at all
    //      species tree vertices have already been computed.
    //
    // combine_scenarios()
    //      Combines each scenario in the first vector with each scenario
    //      in the other vector (basically a union operation) and inserts
    //      the resulting scenarios in the back of
    //      g_backtrack_matrix[u][x].scenarios_at. The Event passed is
    //      used to set the bits of transfer_edges or duplications in the
    //      newly created scenarios.
    //*****************************************************************************
    void backtrack_below_placements(vid_t u, vid_t x);
    void backtrack_outside_placements(vid_t u, vid_t x, vector<vid_t> &);
    void backtrack_mark_needed_scenarios_below(vid_t u, vid_t x);
    void backtrack_min_transfers(vid_t u, vid_t x);
    void backtrack_scenarios_at(vid_t u, vid_t x);
    void combine_scenarios(const vector<Scenario> &, const vector<Scenario> &,
                           vid_t u, vid_t x, BacktrackElement::Event);
    /******************************************************************************/

    static vector<Scenario> scenarios;
    multi_array<cost_type, 2> g_below;
    multi_array<cost_type, 2> g_outside;
    multi_array<BacktrackElement, 2> g_backtrack_matrix;
    static ProgramInput g_input;
    static const unsigned NONE = -1;

};

// Common operations

/* compute the lambda vector */
void compute_lambda(const TreeExtended &species_tree,
                    const TreeExtended &gene_tree,
                    const std::vector<unsigned> &sigma,
                    const boost::dynamic_bitset<> &transfer_edges,
                    std::vector<unsigned> &lambda);

/* count the number of losses of the scenario with the lateral transfer
 * given*/
int count_losses(const TreeExtended  &species_tree,
                 const TreeExtended &gene_tree,
                 const std::vector<unsigned> &sigma,
                 const boost::dynamic_bitset<> &transfer_edges);


/* create the map file */
void create_gene_species_map(const TreeExtended &species_tree, 
                             const TreeExtended &gene_tree,
                             const std::string &map_filename,
                             std::vector<unsigned> &sigma);

void
create_gene_species_map(const TreeExtended &species_tree,
                        const TreeExtended &gene_tree,
                        map<string, string> &str_sigma,
                        std::vector<unsigned> &sigma);

bool operator<(const Scenario &, const Scenario &);
ostream &operator<<(ostream &, const Scenario &);
ostream &operator<<(ostream &, const Candidate &);


#endif // PHYLTR_H
