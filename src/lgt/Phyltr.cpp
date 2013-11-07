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

#include "Phyltr.h"
#include "../tree/Node.hh"

using namespace std;

static const unsigned NONE = -1;
ProgramInput Phyltr::g_input;
vector<Scenario> Phyltr::scenarios;
const cost_type COST_INF = numeric_limits<cost_type>::infinity();

void Phyltr::fpt_algorithm()
{
    typedef std::shared_ptr<Candidate> cand_ptr;
    scenarios.clear();

    // We will do a depth first search, so we need a stack.
    stack<cand_ptr> cand_stack;

    // Push the initial candidate onto the stack. Note that the
    // initial candidate may have some duplications set already!
    cand_ptr initial_candidate(new Candidate);
    if (initial_candidate->cost() <= g_input.max_cost)
    {
        cand_stack.push(initial_candidate);
    }

    // Do the depth-first search.
    //unsigned gene_tree_size = g_input.gene_tree->size();
    unsigned gene_tree_size = g_input.gene_tree->getNumberOfNodes();
    
    while (!cand_stack.empty())
    {
        cand_ptr cp = cand_stack.top(); cand_stack.pop();


        vid_t s_move = cp->get_s_move();
        double cp_cost = cp->cost();

        if (s_move != NONE)
        {
            // Resvole the s-move in three ways.
            if (cp_cost + g_input.duplication_cost <= g_input.max_cost)
            {
                cand_ptr cp1(new Candidate(*cp));
                cp1->set_duplication(cp->parent(s_move));
                if (cp1->cost() <= g_input.max_cost)
                {
                    cand_stack.push(cp1);
                }
            }

            if (cp_cost + g_input.transfer_cost <= g_input.max_cost)
            {
                cand_ptr cp2(new Candidate(*cp));
                cand_ptr cp3(new Candidate(*cp));

                cp2->set_transfer_edge(g_input.gene_tree->getNode(s_move)->getLeftChild()->getNumber());
                cp3->set_transfer_edge(g_input.gene_tree->getNode(s_move)->getRightChild()->getNumber());

                if (cp2->cost() <= g_input.max_cost)
                {
                    cand_stack.push(cp2);
                }
                if (cp3->cost() <= g_input.max_cost)
                {
                    cand_stack.push(cp3);
                }
            }
        }
        else
        {
            // Insert elegant final candidates with cost in
            // the given range into the return-vector.
            if (cp->cost() >= g_input.min_cost &&
                    cp->cost() <= g_input.max_cost &&
                    cp->is_elegant())
            {
                Scenario sc(gene_tree_size);
                for (vid_t u = 0; u < gene_tree_size; ++u)
                {
                    sc.duplications[u] = cp->is_duplication(u);
                    sc.transfer_edges[u] = cp->is_transfer_edge(u);
                    sc.cp.copy(new Candidate(*cp));
                }
                scenarios.push_back(sc);
            }
        }
    }
}

void
Phyltr::print_error(const char *msg)
{
    cerr << "PrimeTV" << ": " << msg << "\n";
}

void Phyltr::printScenarios()
{
    if (!g_input.unsorted)
    {
        sort(scenarios.begin(), scenarios.end());
    }

    BOOST_FOREACH (Scenario &sc, scenarios)
    {
        cout << sc << "\n";
    }
}

void Phyltr::printScenario(Scenario &sc)
{
    cout << sc << "\n";
}

void Phyltr::printCandidate(Candidate &c)
{
    cout << c << "\n";
}

bool
Phyltr::read_sigma()
{
    Phyltr::g_input.sigma.resize(g_input.gene_tree->getNumberOfNodes());
    try
    {
        create_gene_species_map(g_input.species_tree,
                                g_input.gene_tree,
                                g_input.sigma_fname,
                                g_input.sigma);
    }
    catch (logic_error &e)
    {
        print_error(e.what());
        return false;
    }
    return true;
}

bool Phyltr::read_sigma(map< string, string > str_sigma)
{
    Phyltr::g_input.sigma.resize(g_input.gene_tree->getNumberOfNodes());
    try
    {
        create_gene_species_map(g_input.species_tree,
                                g_input.gene_tree,
                                str_sigma,
                                g_input.sigma);
    }
    catch (logic_error &e)
    {
        print_error(e.what());
        return false;
    }

    return true;
}


Scenario Phyltr::getMaxCostScenario()
{
    if(!scenarios.empty())
    {
        sort(scenarios.begin(), scenarios.end());
        Scenario max = scenarios.at(0);
        BOOST_FOREACH (Scenario &sc, scenarios)
        {
            if(max < sc)
            {
                max = sc;
            }
        }
        return max;
    }
    else
    {
        return Scenario(0);
    }

}

void Phyltr::printLambda(vector< vid_t > lambda)
{    
    for (vid_t u = 0; u < g_input.gene_tree->getNumberOfNodes(); ++u)
    {
        unsigned speciesid = lambda[u];
        cout << u << "-" << speciesid << " , ";
    }
    cout << "\n";
}

Scenario Phyltr::getMinCostScenario()
{
    if(!scenarios.empty())
    {
        sort(scenarios.begin(), scenarios.end());
        Scenario min = scenarios.at(0);

        BOOST_FOREACH (Scenario &sc, scenarios)
        {
            if(sc < min)
            {
                min = sc;
            }
        }
        return min;
    }
    else
    {
        return Scenario(0);
    }
}

ostream &
operator<<(ostream &out, const Scenario &sc)
{
    vector<unsigned> transfer_edges;

    for (vid_t u = 0; u < Phyltr::g_input.gene_tree->getNumberOfNodes(); ++u)
    {
        if (sc.transfer_edges[u])
        {
            transfer_edges.push_back(u);
        }
    }
    
    sort(transfer_edges.begin(), transfer_edges.end());
    vector<unsigned> duplications;

    for (vid_t u = 0; u < Phyltr::g_input.gene_tree->getNumberOfNodes(); ++u)
    {
        if (sc.duplications[u])
        {
            duplications.push_back(u);
        }
    }
    
    sort(duplications.begin(), duplications.end());

    out << "Transfer edges Numbers:\t";
    copy(transfer_edges.begin(), transfer_edges.end(),
         ostream_iterator<unsigned>(out, " "));
    out << "\nDuplications Numbers:\t";
    copy(duplications.begin(), duplications.end(),
         ostream_iterator<unsigned>(out, " "));
    out << "\nNumber of losses: " << count_losses(*Phyltr::g_input.species_tree,
                                                  *Phyltr::g_input.gene_tree,
                                                  Phyltr::g_input.sigma,
                                                  sc.transfer_edges);
    out << "\n";

    return out;
}



bool
operator<(const Scenario &sc1, const Scenario &sc2)
{
    double cost1, cost2;
    cost1 =
            sc1.transfer_edges.count() * Phyltr::g_input.transfer_cost +
            sc1.duplications.count() * Phyltr::g_input.duplication_cost;
    cost2 =
            sc2.transfer_edges.count() * Phyltr::g_input.transfer_cost +
            sc2.duplications.count() * Phyltr::g_input.duplication_cost;
    if (cost1 < cost2)
    {
        return true;
    }
    if (cost1 > cost2)
    {
        return false;
    }

    if (sc1.transfer_edges.count() < sc2.transfer_edges.count())
    {
        return true;
    }
    if (sc1.transfer_edges.count() > sc2.transfer_edges.count())
    {
        return false;
    }

    int losses1 = count_losses(*Phyltr::g_input.species_tree, *Phyltr::g_input.gene_tree,
                               Phyltr::g_input.sigma, sc1.transfer_edges);
    int losses2 = count_losses(*Phyltr::g_input.species_tree, *Phyltr::g_input.gene_tree,
                               Phyltr::g_input.sigma, sc2.transfer_edges);
    if (losses1 < losses2)
    {
        return true;
    }
    if (losses1 > losses2)
    {
        return false;
    }

    dynamic_bitset<> transfer_edges1(sc1.transfer_edges.size());
    dynamic_bitset<> transfer_edges2(sc1.transfer_edges.size());
    for (unsigned i = 0; i < sc1.transfer_edges.size(); ++i)
    {
        if (sc1.transfer_edges[i])
        {
            transfer_edges1.set(i);
        }
    }
    for (unsigned i = 0; i < sc2.transfer_edges.size(); ++i)
    {
        if (sc2.transfer_edges[i])
        {
            transfer_edges2.set(i);
        }
    }
    if (transfer_edges1 < transfer_edges2)
    {
        return true;
    }
    if (transfer_edges1 > transfer_edges2)
    {
        return false;
    }

    dynamic_bitset<> duplications1(sc1.duplications.size());
    dynamic_bitset<> duplications2(sc1.duplications.size());
    for (unsigned i = 0; i < sc1.duplications.size(); ++i)
    {
        if (sc1.duplications[i])
        {
            duplications1.set(i);
        }
    }
    for (unsigned i = 0; i < sc2.duplications.size(); ++i)
    {
        if (sc2.duplications[i])
        {
            duplications2.set(i);
        }
    }
    if (duplications1 < duplications2)
    {
        return true;
    }
    if (duplications1 > duplications2)
    {
        return false;
    }

    return false;
}

ostream &
operator<<(ostream &out, const Candidate &c)
{
    out << "duplications:\t" << c.duplications_ << "\n";
    out << "transfers:\t" << c.transfer_edges_ << "\n";
    out << "potential smoves:\t";
    copy(c.s_moves_.begin(), c.s_moves_.end(),
         ostream_iterator<vid_t>(out, " "));
    out << "\n";
    out << "real smoves:\t\t";
    for (vid_t u = 0; u < Phyltr::g_input.gene_tree->getNumberOfNodes(); ++u)
    {
        if (c.is_s_move_(u))
        {
            out << u << " ";
        }
    }
    out << "\n";
    return out;
}

Scenario::Scenario(unsigned size) :
    duplications(size),
    transfer_edges(size)
{
}

Candidate& Candidate::operator=(const Candidate &cp)
{
    /*Candidate *x = new Candidate();
    x->duplications_ = cp.duplications_;
    x->transfer_edges_ = cp.transfer_edges_;
    x->cost_ = cp.cost_;
    x->lambda_ =  cp.lambda_;
    x->P_ = cp.P_;
    x->left_ = cp.left_;
    x->right_ = cp.right_;

    return *x;
    */
    duplications_ = cp.duplications_;
    transfer_edges_ = cp.transfer_edges_;
    cost_ = cp.cost_;
    lambda_ =  cp.lambda_;
    P_ = cp.P_;
    left_ = cp.left_;
    right_ = cp.right_;

    return *this;
}

Candidate::Candidate() :
    duplications_(Phyltr::g_input.gene_tree->getNumberOfNodes()),
    transfer_edges_(Phyltr::g_input.gene_tree->getNumberOfNodes()),
    cost_(0.0),
    lambda_(Phyltr::g_input.gene_tree->getNumberOfNodes()),
    P_(Phyltr::g_input.gene_tree->getNumberOfNodes()),
    left_(Phyltr::g_input.gene_tree->getNumberOfNodes()),
    right_(Phyltr::g_input.gene_tree->getNumberOfNodes())
{

    const TreeExtended &G = *Phyltr::g_input.gene_tree;
    const TreeExtended &S = *Phyltr::g_input.species_tree;

    compute_lambda(S, G, Phyltr::g_input.sigma, transfer_edges_, lambda_);

    for (vid_t u = 0; u < G.getNumberOfNodes(); ++u)
    {
        if(G.getNode(u) == G.getRootNode())
        {
            P_[u] = NONE;
        }
        else
        {
            P_[u] = G.getNode(u)->getParent()->getNumber();
        }

        if(G.getNode(u)->isLeaf())
        {
            left_[u] = NONE;
            right_[u] = NONE;
        }
        else
        {
            left_[u] = G.getNode(u)->getLeftChild()->getNumber();
            right_[u] = G.getNode(u)->getRightChild()->getNumber();
        }
    }

    // Find forced duplications, i.e., internal gene tree vertices
    // that are mapped to leaves of S.

    for (vid_t u = 0; u < G.getNumberOfNodes(); ++u)
    {
        if (!G.getNode(u)->isLeaf() && S.getNode(lambda_[u])->isLeaf())
        {
            duplications_.set(u);
            cost_ += Phyltr::g_input.duplication_cost;
        }
    }

    // Find all s-moves.
    for (vid_t u = 0; u < G.getNumberOfNodes(); ++u)
    {
        if (is_s_move_(u))
        {
            s_moves_.push_back(u);
        }
    }
}

dynamic_bitset< long unsigned int > Candidate::getDuplications()
{
    return duplications_;
}

dynamic_bitset< long unsigned int > Candidate::getTransferEdges()
{
    return transfer_edges_;
}

void
Candidate::set_transfer_edge(vid_t u)
{

    const TreeExtended &G = *Phyltr::g_input.gene_tree;
    const TreeExtended &S = *Phyltr::g_input.species_tree;

    if (u == G.getRootNode()->getNumber()) //0
    {
        throw bad_transfer_exception();
    }

    vid_t parent_u = G.getNode(u)->getParent()->getNumber();
    vid_t sibling_u = G.getNode(parent_u)->getLeftChild()->getNumber() == u ?
                G.getNode(parent_u)->getRightChild()->getNumber() : G.getNode(parent_u)->getLeftChild()->getNumber();

    // parent_u must be an anchor
    if (lambda_[parent_u] == lambda_[u] ||
            lambda_[parent_u] == lambda_[sibling_u])
    {
        throw bad_transfer_exception();
    }

    // Set the transfer and update the cost.
    transfer_edges_.set(u);
    cost_ += Phyltr::g_input.transfer_cost;

    // update P_, left_, and right_
    vid_t v = u == G.getNode(parent_u)->getLeftChild()->getNumber() ? left_[parent_u] : right_[parent_u];
    vid_t w = u == G.getNode(parent_u)->getLeftChild()->getNumber() ? right_[parent_u] : left_[parent_u];

    for (vid_t a = v; a != parent_u; a = G.getNode(a)->getParent()->getNumber())
    {
        P_[a] = NONE;
    }
    for (vid_t a = w; a != parent_u; a = G.getNode(a)->getParent()->getNumber())
    {
        P_[a] = P_[parent_u];
    }
    if (P_[parent_u] != NONE)
    {
        if (left_[P_[parent_u]] == parent_u)
        {
            left_[P_[parent_u]] = w;
        }
        else
        {
            right_[P_[parent_u]] = w;
        }
    }
    
    left_[parent_u] = NONE;
    right_[parent_u] = NONE;

    // update lambda and s_moves, and find the vertices with
    // new positions that are forced duplications.
    lambda_[parent_u] = lambda_[sibling_u];
    vid_t last_updated_vertex = parent_u;

    for (vid_t a = G.getNode(parent_u)->getParent()->getNumber();
         G.getNode(a) != G.getRootNode();
         a = G.getNode(a)->getParent()->getNumber()) //check root?
    {
        // Compute the new placement of a
        vid_t old_lambda = lambda_[a];

        Node *v1 = S.getNode(lambda_[G.getNode(a)->getLeftChild()->getNumber()]);
        Node *v2 = S.getNode(lambda_[G.getNode(a)->getRightChild()->getNumber()]);
        vid_t new_lambda = S.lca(v1,v2)->getNumber();

        if (is_transfer_edge(G.getNode(a)->getLeftChild()->getNumber()))
        {
            new_lambda = lambda_[G.getNode(a)->getRightChild()->getNumber()];
        }
        else if (is_transfer_edge(G.getNode(a)->getRightChild()->getNumber()))
        {
            //new_lambda = lambda_[G.left(a)];
            new_lambda = lambda_[G.getNode(a)->getLeftChild()->getNumber()];
        }
        
        lambda_[a] = new_lambda;

        if (old_lambda == new_lambda)
        {
            break;
        }
        
        last_updated_vertex = a;

        // If a is not a transfer vertex and not a duplication
        if (left_[a] != NONE && !duplications_[a])
        {
            // Is 'a' a forced duplication?
            if (S.getNode(new_lambda)->isLeaf() ||
                    (lambda_[left_[a]] == new_lambda &&
                     duplications_[left_[a]]) ||
                    (lambda_[right_[a]] == new_lambda &&
                     duplications_[right_[a]]))
            {
                duplications_.set(a);
                cost_ += Phyltr::g_input.duplication_cost;
            }
            else// Otherwise its children are potential s-moves
            {
                s_moves_.push_back(left_[a]);
                s_moves_.push_back(right_[a]);
            }
        }
    }

    if (P_[last_updated_vertex] != NONE)
    {
        s_moves_.push_back(P_[last_updated_vertex]);
    }
}

void
Candidate::set_duplication(vid_t u)
{
    // u must not be a duplication or transfer vertex
    if (is_duplication(u) || left_[u] == NONE)
    {
        throw bad_duplication_exception();
    }

    duplications_.set(u);
    cost_ += Phyltr::g_input.duplication_cost;

    // Find any forced duplications as a result of u becoming a duplication.
    for (vid_t v = P_[u]; v != NONE; v = P_[v])
    {
        if (!duplications_[v] && lambda_[v] == lambda_[u])
        {
            duplications_.set(v);
            cost_ += Phyltr::g_input.duplication_cost;
        }
        else
        {
            break;
        }
    }
}

bool
Candidate::is_transfer_edge(vid_t u) const
{
    return transfer_edges_.test(u);
}



bool
Candidate::is_duplication(vid_t u) const
{
    return duplications_.test(u);
}

double
Candidate::cost() const
{
    return cost_;
}

bool
Candidate::is_elegant() const
{
    const TreeExtended &G = *Phyltr::g_input.gene_tree;
    const TreeExtended &S = *Phyltr::g_input.species_tree;
    
    vector<vid_t> highest(G.getNumberOfNodes());

    compute_highest_mapping_(highest);
    
    // Check that the children of duplications are mapped by lambda to
    // comparable species tree vertices. Otherwise, the duplication is
    // unnecessary.
    for (dynamic_bitset<>::size_type dd = duplications_.find_first();
         dd != duplications_.npos;
         dd = duplications_.find_next(dd))
    {
        vid_t d = dd;
        vid_t v = G.getNode(d)->getLeftChild()->getNumber();
        vid_t w = G.getNode(d)->getRightChild()->getNumber();
        if (!S.descendant(S.getNode(lambda_[v]),S.getNode(lambda_[w])) &&
                !S.descendant(S.getNode(lambda_[w]),S.getNode(lambda_[v])))
        {
            return false;
        }
    }

    // Check if the parents of transfer vertices can be mapped high
    // enough so that the transfer can be converted to a
    // speciation. If so, the transfer is unnecessary.
    for (dynamic_bitset<>::size_type vv = transfer_edges_.find_first();
         vv != transfer_edges_.npos;
         vv = transfer_edges_.find_next(vv))
    {
        vid_t v = vv;
        // Let (u, v) be the transfer edge we are considering, let
        // pu = p(u), and x = lca{lambda_[u], lambda_[v]}
        vid_t u = G.getNode(v)->getParent()->getNumber();
        vid_t pu = G.getNode(u)->getParent()->getNumber();
        vid_t x = S.lca(S.getNode(lambda_[u]),S.getNode(lambda_[v]))->getNumber();

        // The root of G is always an unnecessary transfer vertex.
        if (u == G.getRootNode()->getNumber())
        {
            return false;
        }

        // If p(u) is a speciation and x is a proper descendant of
        // highest[p(u)] = lambda_[p(u)], then the transfer is
        // unnecessary.
        if (!is_duplication(pu) &&
                !is_transfer_edge(G.getNode(pu)->getLeftChild()->getNumber()) &&
                !is_transfer_edge(G.getNode(pu)->getRightChild()->getNumber()) &&
                S.descendant(S.getNode(x),S.getNode(lambda_[pu])) &&
                x != lambda_[pu])
        {
            return false;
        }

        // If p(u) is not a speciation, then it is enough for x to
        // be a descendant of highest[p(u)] for the transfer to be
        // unnecessary.
        if ((is_duplication(pu) ||
             is_transfer_edge(G.getNode(pu)->getLeftChild()->getNumber()) ||
             is_transfer_edge(G.getNode(pu)->getRightChild()->getNumber())) &&
                S.descendant(S.getNode(x),S.getNode(highest[pu])))
        {
            return false;
        }
    }

    return true;
}



vid_t
Candidate::get_s_move() const
{
    // Check vertices in s_move and find one that really is an s-move.
    while (!s_moves_.empty() && !is_s_move_(s_moves_.back()))
    {
        s_moves_.pop_back();
    }
    
    return s_moves_.empty() ? NONE : s_moves_.back();
}

vid_t
Candidate::lambda(vid_t u) const
{
    return lambda_[u];
}

std::vector<vid_t> Candidate::getLambda() const
{
    return lambda_;
}

vid_t
Candidate::parent(vid_t u) const
{
    return P_[u];
}

vid_t
Candidate::left(vid_t u) const
{
    return left_[u];
}

vid_t
Candidate::right(vid_t u) const
{
    return right_[u];
}

void Candidate::copy(Candidate *cp)
{

    this->duplications_ = cp->duplications_;
    this->transfer_edges_ = cp->transfer_edges_;
    this->cost_ = cp->cost_;
    this->lambda_ =  cp->lambda_;
    this->P_ = cp->P_;
    this->left_ = cp->left_;
    this->right_ = cp->right_;
}

bool
Candidate::is_s_move_(vid_t u) const
{
    return
            !Phyltr::g_input.gene_tree->getNode(u)->isLeaf() &&
            !is_duplication(u) &&
            P_[u] != NONE &&
            lambda_[Phyltr::g_input.gene_tree->getNode(u)->getLeftChild()->getNumber()] != lambda_[u] &&
            lambda_[Phyltr::g_input.gene_tree->getNode(u)->getRightChild()->getNumber()] != lambda_[u] &&
            lambda_[P_[u]] == lambda_[u] &&
            !is_duplication(P_[u]);
}

void 
Candidate::compute_highest_mapping_(vector<vid_t> &highest) const
{

    const TreeExtended &G = *Phyltr::g_input.gene_tree;
    const TreeExtended &S = *Phyltr::g_input.species_tree;
    const vector<vid_t> &sigma = Phyltr::g_input.sigma;

    highest.resize(G.getNumberOfNodes());

    // We define a function C(x, y) : V(S) x V(S) -> V(S). y must be a
    // proper descendant of x in the species tree. The function
    // returns the unique child of x that is an ancestor of y.
    struct
    {
        vid_t operator()(vid_t x, vid_t y)
        {
            vid_t left = Phyltr::g_input.species_tree->getNode(x)->getLeftChild()->getNumber();
            vid_t right = Phyltr::g_input.species_tree->getNode(x)->getRightChild()->getNumber();
            return  Phyltr::g_input.species_tree->descendant(Phyltr::g_input.species_tree->getNode(y),
                                                             Phyltr::g_input.species_tree->getNode(left)) ? left : right;
        }
    } C;

    // First, take care of the root of G.

    if (!is_duplication(G.getRootNode()->getNumber()) &&
            !is_transfer_edge(G.getRootNode()->getLeftChild()->getNumber()) &&
            !is_transfer_edge(G.getRootNode()->getRightChild()->getNumber())) // if root is a speciation
    {
        highest[G.getRootNode()->getNumber()] = lambda_[G.getRootNode()->getNumber()];
    }
    else if (is_duplication(G.getRootNode()->getNumber()))
    {

        highest[G.getRootNode()->getNumber()] = S.getRootNode()->getNumber();
    }
    else // If the root is a transfer vertex.
    {
        // Let v be the transfered child of the root.
        vid_t v = is_transfer_edge(G.getRootNode()->getLeftChild()->getNumber()) ?
                    G.getRootNode()->getLeftChild()->getNumber() : G.getRootNode()->getRightChild()->getNumber();
        highest[G.getRootNode()->getNumber()] = C((S.lca(S.getNode(lambda_[G.getRootNode()->getNumber()]),
                                                   S.getNode(lambda_[v])))->getNumber(), lambda_[G.getRootNode()->getNumber()]);
    }

    // Next, take care of the rest of the vertices from the root and down.g
    for (Node *u = G.preorder_next(G.getRootNode());
         u != NULL; u = G.preorder_next(u)) //check it iterates well
    {
        if (u == G.getRootNode())
        {
            continue;
        }

        vid_t pu = u->getParent()->getNumber();
        vid_t x = lambda_[pu];
        vid_t y = lambda_[u->getNumber()];

        if (u->isLeaf())
        {
            highest[u->getNumber()] = sigma[u->getNumber()];
        }
        else if (
                 !is_duplication(u->getNumber()) &&
                 !is_transfer_edge(u->getLeftChild()->getNumber()) &&
                 !is_transfer_edge(u->getRightChild()->getNumber()))
        {
            highest[u->getNumber()] = lambda_[u->getNumber()];
        }
        else // If u is a duplication or a transfer vertex.
        {
            // Let z be the highest possible mapping of u when
            // considering only p(u). If p(u) is a duplication
            // or if p(u) is a transfer but u is not the
            // transfered vertex, z = highest[pu]. Otherwise,
            // if p(u) is a speciation, z = C(x, y), and if
            // u is the transfered vertex, then z = C(lca(x, y), y)
            vid_t z = highest[pu];
            if (!is_duplication(pu) &&
                    !is_transfer_edge(G.getNode(pu)->getLeftChild()->getNumber()) &&
                    !is_transfer_edge(G.getNode(pu)->getRightChild()->getNumber())) // If pu is speciation.
            {
                z = C(x, y);
            }
            else if (is_transfer_edge(u->getNumber()))
            {
                z = C((S.lca(S.getNode(x),S.getNode(y)))->getNumber(), y);
            }
            // Let z_prime be the highest possible mapping of
            // u when considering its children only. z_prime
            // is the root of x unless u is a transfer. In
            // that case, if v is the transferred child,
            vid_t z_prime = S.getRootNode()->getNumber();
            if (is_transfer_edge(u->getLeftChild()->getNumber()) ||
                    is_transfer_edge(u->getRightChild()->getNumber()))
            {
                // Let v be the transferred child of u.
                vid_t v = is_transfer_edge(u->getLeftChild()->getNumber()) ? u->getLeftChild()->getNumber() : u->getRightChild()->getNumber();
                z_prime = C((S.lca(S.getNode(y), S.getNode(lambda_[v])))->getNumber(), y);
            }
            // Since z and z_prime are both ancestors of
            // lambda_[u], we know that they are
            // comparable. The one that is minimal in S is
            // then the highest possible mapping of u.
            highest[u->getNumber()] = S.descendant(S.getNode(z),S.getNode(z_prime)) ? z : z_prime;
        }
    }
}

void compute_lambda(const TreeExtended &S,
                    const TreeExtended &G,
                    const std::vector<unsigned> &sigma,
                    const boost::dynamic_bitset<> &transfer_edges,
                    std::vector<unsigned> &lambda)
{

    lambda.resize(G.getNumberOfNodes());
    
    for (Node *u = G.getPostOderBegin();
         u != NULL;
         u = G.postorder_next(u))
    {
        /* Take care of gene tree leaves and continue. */
        if (u->isLeaf())
        {
            lambda[u->getNumber()] = sigma[u->getNumber()];
            continue;
        }
        
        Node *v = u->getLeftChild();
        Node *w = u->getRightChild();
        
        if (transfer_edges[v->getNumber()])
        {
            lambda[u->getNumber()] = lambda[w->getNumber()];
        }
        else if (transfer_edges[w->getNumber()])
        {
            lambda[u->getNumber()] = lambda[v->getNumber()];
        }
        else
        {
            lambda[u->getNumber()] = (S.lca(S.getNode(lambda[w->getNumber()]),S.getNode(lambda[v->getNumber()])))->getNumber();
        }
    }
}

int count_losses(const TreeExtended &S,
                 const TreeExtended &G,
                 const std::vector<unsigned> &sigma,
                 const boost::dynamic_bitset<> &transfer_edges)
{


    // Compute lambda
    std::vector<unsigned> lambda;
    compute_lambda(S, G, sigma, transfer_edges, lambda);

    // For each non-transfer edge (u, v) in G, count the number of
    // speciations that we pass from lambda(u) to lambda(v).  A loss
    // is also incurred if u is a duplication and lambda(u) !=
    // lambda(v).
    int losses = 0;
    for (unsigned u = 0; u < G.getNumberOfNodes(); ++u)
    {
        if(G.getNode(u) == G.getRootNode())
        {
            break;
        }
        
        Node *p = G.getNode(u)->getParent();

        if (transfer_edges[u] || lambda[p->getNumber()] == lambda[u])
        {
            continue;
        }

        Node *x = S.getNode(lambda[u])->getParent();

        Node *u_sibling = p->getLeftChild()->getNumber() == u ? p->getRightChild() : p->getLeftChild();

        if (lambda[u_sibling->getNumber()] == lambda[p->getNumber()]) // we know that lampda(u) != lambda(p)!
        {
            losses += 1;
        }

        while (x->getNumber() != lambda[p->getNumber()])
        {
            losses += 1;
            x = x->getParent();
        }
    }
    
    return losses;
}

void
create_gene_species_map(const TreeExtended &species_tree,const TreeExtended &gene_tree, 
                        const std::string& map_filename,std::vector<unsigned> &sigma)
{

    using namespace std;
    vector<string> map_file_content;
    
    ifstream map_file(map_filename.c_str());
    
    copy(istream_iterator<string>(map_file), istream_iterator<string>(),
         back_inserter(map_file_content));

    /* Make sure there are even number of strings in map file. */
    if (map_file_content.empty() || map_file_content.size() % 2 != 0)
    {
        throw logic_error("error reading map file.");
    }

    /* Create a map from gene name to species name. */
    map<string, string> str_sigma;
    for (unsigned i = 0; i < map_file_content.size(); i += 2)
    {
        str_sigma[map_file_content[i]] = map_file_content[i+1];
    }
    
    /* Create the final map called sigma mapping vid_t to vid_t */
    for (unsigned v = 0; v < gene_tree.getNumberOfNodes(); ++v) //check it iterates well
    {

        if (!gene_tree.getNode(v)->isLeaf())
        {
            continue;
        }
        string gene_label = gene_tree.getNode(v)->getName();
        string species_label = str_sigma[gene_label];
        if (species_label == "")
        {
            string message =
                    "gene label '" + gene_label + "' "
                    "is missing in map file.";
            throw logic_error(message);
        }
        if (species_tree.findNode(species_label) == NULL)
        {
            string message =
                    "species label '" + species_label + "' "
                    "which occurs in map file "
                    "does not exist in species tree.";
            throw logic_error(message);
        }
        sigma[v] = species_tree.findNode(species_label)->getNumber();
    }
}

void
create_gene_species_map(const TreeExtended &species_tree,const TreeExtended &gene_tree, 
                        map<string, string> &str_sigma,std::vector<unsigned> &sigma)
{

    using namespace std;
    
    /* Create the final map called sigma mapping vid_t to vid_t */
    for (unsigned v = 0; v < gene_tree.getNumberOfNodes(); ++v) //check it iterates well
    {

        if (!gene_tree.getNode(v)->isLeaf())
        {
            continue;
        }
        string gene_label = gene_tree.getNode(v)->getName();
        string species_label = str_sigma[gene_label];
        if (species_label == "")
        {
            string message =
                    "gene label '" + gene_label + "' "
                    "is missing in map file.";
            throw logic_error(message);
        }
        if (species_tree.findNode(species_label) == NULL)
        {
            string message =
                    "species label '" + species_label + "' "
                    "which occurs in map file "
                    "does not exist in species tree.";
            throw logic_error(message);
        }
        sigma[v] = species_tree.findNode(species_label)->getNumber();
    }
}

void
Phyltr::dp_algorithm()
{
    const TreeExtended &S = *Phyltr::g_input.species_tree;
    const TreeExtended &G = *Phyltr::g_input.gene_tree;
    const bool do_backtrack = true;

    // Allocate memory for the matrices.
    g_below.resize(boost::extents[G.getNumberOfNodes()][S.getNumberOfNodes()]);
    g_outside.resize(boost::extents[G.getNumberOfNodes()][S.getNumberOfNodes()]);
    
    if (do_backtrack)
    {
        g_backtrack_matrix.resize(boost::extents[G.getNumberOfNodes()][S.getNumberOfNodes()]);
    }

    // Initialize below and outside to infinity.
    for (vid_t u = 0; u < G.getNumberOfNodes(); ++u)
    {
        for (vid_t x = 0; x < S.getNumberOfNodes(); ++x)
        {
            g_below[u][x] = COST_INF;
            g_outside[u][x] = COST_INF;
        }
    }

    // The algorithm itself is described in a published paper.
    for (Node *u = G.getPostOderBegin();
         u != NULL;
         u = G.postorder_next(u))
    {
        // First compute g_below[u][*].
        for (Node *x = S.getPostOderBegin();
             x != NULL;
             x = S.postorder_next(x))
        {
            compute_below(u->getNumber(), x->getNumber());
        }
        // Compute g_outside[u][*]
        for (Node *x = S.preorder_begin();
             x != NULL;
             x = S.preorder_next(x))
        {
            compute_outside(u->getNumber(), x->getNumber());
        }

    }
}

void
Phyltr::compute_below(vid_t u, vid_t x)
{
    const TreeExtended &S = *Phyltr::g_input.species_tree;
    const TreeExtended &G = *Phyltr::g_input.gene_tree;
    const std::vector<vid_t>  &sigma = Phyltr:: g_input.sigma;
    const cost_type tcost = Phyltr::g_input.transfer_cost;
    const cost_type dcost = Phyltr::g_input.duplication_cost;
    const bool do_backtrack = true;

    if (G.getNode(u)->isLeaf())
    {
        if (S.descendant(S.getNode(sigma[u]), S.getNode(x)))
        {
            g_below[u][x] = G.getRootNode()->getNumber();
        }
    }
    else
    {
        vector<cost_type> costs(BacktrackElement::N_EVENTS, COST_INF);

        costs[BacktrackElement::D] =
                dcost + g_below[G.getNode(u)->getLeftChild()->getNumber()][x] + g_below[G.getNode(u)->getRightChild()->getNumber()][x];
        costs[BacktrackElement::T_LEFT] =
                tcost + g_outside[G.getNode(u)->getLeftChild()->getNumber()][x] + g_below[G.getNode(u)->getRightChild()->getNumber()][x];
        costs[BacktrackElement::T_RIGHT] =
                tcost + g_outside[G.getNode(u)->getRightChild()->getNumber()][x] + g_below[G.getNode(u)->getLeftChild()->getNumber()][x];

        if (!S.getNode(x)->isLeaf())
        {
            costs[BacktrackElement::S] =
                    g_below[G.getNode(u)->getLeftChild()->getNumber()][S.getNode(x)->getLeftChild()->getNumber()] +
                    g_below[G.getNode(u)->getRightChild()->getNumber()][S.getNode(x)->getRightChild()->getNumber()];
            costs[BacktrackElement::S_REV] =
                    g_below[G.getNode(u)->getLeftChild()->getNumber()][S.getNode(x)->getRightChild()->getNumber()] +
                    g_below[G.getNode(u)->getRightChild()->getNumber()][S.getNode(x)->getLeftChild()->getNumber()];

            costs[BacktrackElement::BELOW_LEFT] = g_below[u][S.getNode(x)->getLeftChild()->getNumber()];
            costs[BacktrackElement::BELOW_RIGHT] = g_below[u][S.getNode(x)->getRightChild()->getNumber()];
        }

        cost_type min_cost = *min_element(costs.begin(), costs.end());
        g_below[u][x] = min_cost;

        // Save the optimal events for backtracking.
        if (do_backtrack)
        {
            bitset<BacktrackElement::N_EVENTS> &events =
                    g_backtrack_matrix[u][x].below_events;
            for (unsigned e = 0; e < BacktrackElement::N_EVENTS; ++e)
            {
                if (min_cost != COST_INF && costs[e] == min_cost)
                {
                    events.set(BacktrackElement::Event(e));
                }
            }
        }
    }
}

void
Phyltr::compute_outside(vid_t u, vid_t x)
{
    const TreeExtended &S = *Phyltr::g_input.species_tree;
    const bool do_backtrack = true;

    // Cannot place u outside the root of S.
    if (x == S.getRootNode()->getNumber())
    {
        return;
    }
    vid_t x_parent = S.getNode(x)->getParent()->getNumber();
    vid_t x_sibling =
            S.getNode(x_parent)->getLeftChild()->getNumber() == x ? S.getNode(x_parent)->getRightChild()->getNumber() :
                                                                    S.getNode(x_parent)->getLeftChild()->getNumber();

    cost_type min_cost = min(g_below[u][x_sibling], g_outside[u][x_parent]);
    g_outside[u][x] = min_cost;

    // Save info for backtracking.
    if (do_backtrack)
    {
        if (g_below[u][x_sibling] == min_cost)
        {
            g_backtrack_matrix[u][x].outside_sibling = x_sibling;
        }

        if (g_outside[u][x_parent] == min_cost)
        {
            if (g_backtrack_matrix[u][x_parent].outside_sibling != NONE)
            {
                g_backtrack_matrix[u][x].outside_ancestor = x_parent;
            }
            else
            {
                g_backtrack_matrix[u][x].outside_ancestor =
                        g_backtrack_matrix[u][x_parent].outside_ancestor;
            }
        }
    }
}


void
Phyltr::backtrack()
{
    const TreeExtended &S = *Phyltr::g_input.species_tree;
    const TreeExtended &G = *Phyltr::g_input.gene_tree;
    const std::vector<vid_t> &sigma = Phyltr::g_input.sigma;
    multi_array<BacktrackElement, 2> &matrix = Phyltr::g_backtrack_matrix;

    // Backtrack the placements for each u and x.
    for (Node *u = G.getPostOderBegin(); u != NULL; u = G.postorder_next(u))
    {
        for (Node *x = S.getPostOderBegin(); x != NULL; x = S.postorder_next(x))
        {
            Phyltr::backtrack_below_placements(u->getNumber(), x->getNumber());
        }
    }

    // Mark the sets of scenarios that we need to compute.
    matrix[G.getRootNode()->getNumber()][S.getRootNode()->getNumber()].scenarios_below_needed = true; //ROOT??
    
    for (Node *u = G.preorder_begin(); u != NULL; u = G.preorder_next(u))
    {
        for (vid_t x = 0; x < S.getNumberOfNodes(); ++x)
        {
            if (matrix[u->getNumber()][x].scenarios_below_needed)
            {
                BOOST_FOREACH (vid_t y, matrix[u->getNumber()][x].below_placements)
                {
                    matrix[u->getNumber()][y].scenarios_at_needed = true;
                }
            }
        }

        if (u->isLeaf())
        {
            continue;
        }
        
        for (vid_t x = 0; x < S.getNumberOfNodes(); ++x)
        {
            if (matrix[u->getNumber()][x].scenarios_at_needed)
            {
                Phyltr::backtrack_mark_needed_scenarios_below(u->getNumber(), x);
            }
        }
    }

    // Compute the minimum number of transfer events for each u and x.
    for (Node *u = G.getPostOderBegin(); u != NULL; u = G.postorder_next(u))
    {
        for (Node *x = S.getPostOderBegin(); x != NULL; x = S.postorder_next(x))
        {
            Phyltr::backtrack_min_transfers(u->getNumber(), x->getNumber());
        }
    }

    // Backtrack the needed scenarios.
    for (Node *u = G.getPostOderBegin(); u != NULL && u->getNumber() < G.getNumberOfNodes(); u = G.postorder_next(u))  // CHECK CHECK
    {
        for (vid_t x = 0; x < S.getNumberOfNodes(); ++x)
        {
            if (matrix[u->getNumber()][x].scenarios_at_needed)
            {
                Phyltr::backtrack_scenarios_at(u->getNumber(), x);
            }
        }

        // Remove the unneeded sets of scenarios to conserve memory.
        if (!u->isLeaf())
        {
            for (vid_t x = 0; x < S.getNumberOfNodes(); ++x)
            {
                vector<Scenario>().swap(matrix[u->getLeftChild()->getNumber()][x].scenarios_at);
                vector<Scenario>().swap(matrix[u->getRightChild()->getNumber()][x].scenarios_at);
            }
        }
    }

    // Find the minimum number of losses of placing root of G below
    // root of S.
    int max_losses = numeric_limits<int>::max();
    if (Phyltr::g_input.print_only_minimal_loss_scenarios)
    {
        BOOST_FOREACH(vid_t x, matrix[G.getRootNode()->getNumber()][S.getRootNode()->getNumber()].below_placements)
        {
            BOOST_FOREACH(Scenario &sc, matrix[G.getRootNode()->getNumber()][x].scenarios_at)
            {
                max_losses = min(max_losses,count_losses(S, G, sigma, sc.transfer_edges));
            }
        }
    }

    // Find the final sets of scenarios.
    BOOST_FOREACH(vid_t x, matrix[G.getRootNode()->getNumber()][S.getRootNode()->getNumber()].below_placements)
    {
        // Take only scenarios with minimal transfers if the flag is set.
        if (Phyltr::g_input.print_only_minimal_transfer_scenarios &&
                !matrix[G.getRootNode()->getNumber()][x].scenarios_at.empty() &&
                matrix[G.getRootNode()->getNumber()][x].scenarios_at[0].transfer_edges.count() >
                matrix[G.getRootNode()->getNumber()][S.getRootNode()->getNumber()].min_transfers)
        {
            vector<Scenario>().swap(matrix[G.getRootNode()->getNumber()][x].scenarios_at);
            continue;
        }
        // Take only scenarios with minimal losses if the flag is set.
        BOOST_FOREACH(Scenario &sc, matrix[G.getRootNode()->getNumber()][x].scenarios_at)
        {
            if (count_losses(S, G, sigma, sc.transfer_edges) <= max_losses)
                Phyltr::scenarios.push_back(sc);
        }
        vector<Scenario>().swap(matrix[G.getRootNode()->getNumber()][x].scenarios_at);
    }

    return;
}



void
Phyltr::backtrack_below_placements(vid_t u, vid_t x)
{
    const TreeExtended &S = *Phyltr::g_input.species_tree;
    const TreeExtended &G = *Phyltr::g_input.gene_tree;

    BacktrackElement &elem = Phyltr::g_backtrack_matrix[u][x];

    if (g_below[u][x] == COST_INF) // If no solutions exist
    {
        return;
    }
    if (G.getNode(u)->isLeaf())
    {
        elem.below_placements.push_back(g_input.sigma[u]);
    }
    else if (S.getNode(x)->isLeaf())
    {
        elem.below_placements.push_back(x);
    }
    else
    {
        BacktrackElement &left_elem = g_backtrack_matrix[G.getNode(u)->getLeftChild()->getNumber()][x];
        BacktrackElement &right_elem = g_backtrack_matrix[G.getNode(u)->getRightChild()->getNumber()][x];
        const bitset<BacktrackElement::N_EVENTS> &e = elem.below_events;

        // First, determine if u is placed _at_ x.
        if (e[BacktrackElement::S] || e[BacktrackElement::S_REV] ||
                (e[BacktrackElement::T_LEFT] && right_elem.below_placements[0] == x) ||
                (e[BacktrackElement::T_RIGHT] && left_elem.below_placements[0] == x) ||
                (e[BacktrackElement::D] && right_elem.below_placements[0] == x) ||
                (e[BacktrackElement::D] && left_elem.below_placements[0] == x))
        {
            elem.below_placements.push_back(x);
        }

        // Then, see where u is placed below x.
        if (elem.below_events[BacktrackElement::BELOW_LEFT])
        {
            vector<vid_t> &left_placements =
                    g_backtrack_matrix[u][S.getNode(x)->getLeftChild()->getNumber()].below_placements;
            copy(left_placements.begin(), left_placements.end(),
                 back_inserter(elem.below_placements));
        }
        if (elem.below_events[BacktrackElement::BELOW_RIGHT])
        {
            vector<vid_t> &right_placements =
                    g_backtrack_matrix[u][S.getNode(x)->getRightChild()->getNumber()].below_placements;
            copy(right_placements.begin(), right_placements.end(),
                 back_inserter(elem.below_placements));
        }
    }
}



void
Phyltr::backtrack_mark_needed_scenarios_below(vid_t u, vid_t x)
{
    const TreeExtended &S = *Phyltr::g_input.species_tree;
    const TreeExtended &G = *Phyltr::g_input.gene_tree;
    multi_array<BacktrackElement, 2> &matrix = Phyltr::g_backtrack_matrix;

    const bitset<BacktrackElement::N_EVENTS> &e = matrix[u][x].below_events;
    if (e[BacktrackElement::S])
    {
        matrix[G.getNode(u)->getLeftChild()->getNumber()][S.getNode(x)->getLeftChild()->getNumber()].scenarios_below_needed = true;
        matrix[G.getNode(u)->getRightChild()->getNumber()][S.getNode(x)->getRightChild()->getNumber()].scenarios_below_needed = true;
    }
    if (e[BacktrackElement::S_REV])
    {
        matrix[G.getNode(u)->getLeftChild()->getNumber()][S.getNode(x)->getRightChild()->getNumber()].scenarios_below_needed = true;
        matrix[G.getNode(u)->getRightChild()->getNumber()][S.getNode(x)->getLeftChild()->getNumber()].scenarios_below_needed = true;
    }
    if (e[BacktrackElement::D])
    {
        // This is the only time we need to set a scenarios_at_needed.
        if (matrix[G.getNode(u)->getRightChild()->getNumber()][x].below_placements[0] == x)
        {
            matrix[G.getNode(u)->getRightChild()->getNumber()][x].scenarios_at_needed = true;
            matrix[G.getNode(u)->getLeftChild()->getNumber()][x].scenarios_below_needed = true;
        }

        if (matrix[G.getNode(u)->getLeftChild()->getNumber()][x].below_placements[0] == x)
        {
            matrix[G.getNode(u)->getLeftChild()->getNumber()][x].scenarios_at_needed = true;
            matrix[G.getNode(u)->getRightChild()->getNumber()][x].scenarios_below_needed = true;
        }
    }
    if (e[BacktrackElement::T_LEFT])
    {
        matrix[G.getNode(u)->getRightChild()->getNumber()][x].scenarios_below_needed = true;

        vector<vid_t> outside_placements;
        backtrack_outside_placements(G.getNode(u)->getLeftChild()->getNumber(), x, outside_placements);
        BOOST_FOREACH (vid_t y, outside_placements)
        {
            matrix[G.getNode(u)->getLeftChild()->getNumber()][y].scenarios_below_needed = true;
        }
    }
    if (e[BacktrackElement::T_RIGHT])
    {
        matrix[G.getNode(u)->getLeftChild()->getNumber()][x].scenarios_below_needed = true;

        vector<vid_t> outside_placements;
        backtrack_outside_placements(G.getNode(u)->getRightChild()->getNumber(), x, outside_placements);
        BOOST_FOREACH (vid_t y, outside_placements)
        {
            matrix[G.getNode(u)->getRightChild()->getNumber()][y].scenarios_below_needed = true;
        }
    }
}

void
Phyltr::backtrack_scenarios_at(vid_t u, vid_t x)
{
    const TreeExtended &G = *Phyltr::g_input.gene_tree;
    const TreeExtended &S = *Phyltr::g_input.species_tree;
    multi_array<BacktrackElement, 2> &matrix = Phyltr::g_backtrack_matrix;

    if (G.getNode(u)->isLeaf())
    {
        // it must be the case that sigma(u) = x, otherwise the
        // algorithm is corrupt.
        matrix[u][x].scenarios_at.push_back(Scenario(G.getNumberOfNodes()));
        return;
    }

    const bitset<BacktrackElement::N_EVENTS> &events = matrix[u][x].below_events;

    if (events[BacktrackElement::S])
    {
        BOOST_FOREACH (vid_t y1, matrix[G.getNode(u)->getLeftChild()->getNumber()][S.getNode(x)->getLeftChild()->getNumber()].below_placements)
        {
            BOOST_FOREACH (vid_t y2, matrix[G.getNode(u)->getRightChild()->getNumber()][S.getNode(x)->getRightChild()->getNumber()].below_placements)
            {
                combine_scenarios(matrix[G.getNode(u)->getLeftChild()->getNumber()][y1].scenarios_at,
                        matrix[G.getNode(u)->getRightChild()->getNumber()][y2].scenarios_at,
                        u, x, BacktrackElement::S);
            }
        }
    }
    if (events[BacktrackElement::S_REV])
    {
        BOOST_FOREACH (vid_t y1, matrix[G.getNode(u)->getLeftChild()->getNumber()][S.getNode(x)->getRightChild()->getNumber()].below_placements)
        {
            BOOST_FOREACH (vid_t y2, matrix[G.getNode(u)->getRightChild()->getNumber()][S.getNode(x)->getLeftChild()->getNumber()].below_placements)
            {
                combine_scenarios(matrix[G.getNode(u)->getLeftChild()->getNumber()][y1].scenarios_at,
                        matrix[G.getNode(u)->getRightChild()->getNumber()][y2].scenarios_at,
                        u, x, BacktrackElement::S_REV);
            }
        }
    }
    if (events[BacktrackElement::D])
    {
        // Here we have to perform more work to ensure we do not
        // get duplicate scenarios. The only way that u is mapped
        // _at_ x is if at least one of the children of u is also
        // placed _at_ x.
        if (matrix[G.getNode(u)->getLeftChild()->getNumber()][x].below_placements[0] == x &&
                matrix[G.getNode(u)->getRightChild()->getNumber()][x].below_placements[0] == x)
        {
            combine_scenarios(matrix[G.getNode(u)->getLeftChild()->getNumber()][x].scenarios_at,
                    matrix[G.getNode(u)->getRightChild()->getNumber()][x].scenarios_at,
                    u, x, BacktrackElement::D);
        }

        if (matrix[G.getNode(u)->getLeftChild()->getNumber()][x].below_placements[0] == x)
        {
            BOOST_FOREACH (vid_t y, matrix[G.getNode(u)->getRightChild()->getNumber()][x].below_placements)
            {
                if (y == x)
                {
                    continue;
                }
                combine_scenarios(matrix[G.getNode(u)->getRightChild()->getNumber()][y].scenarios_at,
                        matrix[G.getNode(u)->getLeftChild()->getNumber()][x].scenarios_at,
                        u, x, BacktrackElement::D);
            }
        }
        if (matrix[G.getNode(u)->getRightChild()->getNumber()][x].below_placements[0] == x)
        {
            BOOST_FOREACH (vid_t y, matrix[G.getNode(u)->getLeftChild()->getNumber()][x].below_placements)
            {
                if (y == x)
                {
                    continue;
                }
                combine_scenarios(matrix[G.getNode(u)->getLeftChild()->getNumber()][y].scenarios_at,
                        matrix[G.getNode(u)->getRightChild()->getNumber()][x].scenarios_at,
                        u, x, BacktrackElement::D);
            }
        }
    }
    if (events[BacktrackElement::T_LEFT])
    {
        vector<vid_t> outside_placements;
        backtrack_outside_placements(G.getNode(u)->getLeftChild()->getNumber(), x, outside_placements);

        BOOST_FOREACH (vid_t y, outside_placements)
        {
            BOOST_FOREACH (vid_t y1, matrix[G.getNode(u)->getLeftChild()->getNumber()][y].below_placements)
            {
                combine_scenarios(matrix[G.getNode(u)->getLeftChild()->getNumber()][y1].scenarios_at,
                        matrix[G.getNode(u)->getRightChild()->getNumber()][x].scenarios_at,
                        u, x, BacktrackElement::T_LEFT);
            }
        }
    }
    if (events[BacktrackElement::T_RIGHT])
    {
        vector<vid_t> placements;
        backtrack_outside_placements(G.getNode(u)->getRightChild()->getNumber(), x, placements);

        BOOST_FOREACH (vid_t y, placements)
        {
            BOOST_FOREACH (vid_t y1, matrix[G.getNode(u)->getRightChild()->getNumber()][y].below_placements)
            {
                combine_scenarios(matrix[G.getNode(u)->getRightChild()->getNumber()][y1].scenarios_at,
                        matrix[G.getNode(u)->getLeftChild()->getNumber()][x].scenarios_at,
                        u, x, BacktrackElement::T_RIGHT);
            }
        }
    }
}

void
Phyltr::backtrack_min_transfers(vid_t u, vid_t x)
{
    const TreeExtended &G = *Phyltr::g_input.gene_tree;
    const TreeExtended &S = *Phyltr::g_input.species_tree;
    const bitset<BacktrackElement::N_EVENTS> &events = g_backtrack_matrix[u][x].below_events;

    // The base case when u is a leaf.
    if (G.getNode(u)->isLeaf() && g_below[u][x] != COST_INF)
    {
        g_backtrack_matrix[u][x].min_transfers = 0;
        return;
    }

    unsigned min_transfers = G.getNumberOfNodes() + 1; // Max possible number of transfers.
    if (events[BacktrackElement::D])
    {
        min_transfers = min(min_transfers,
                            g_backtrack_matrix[G.getNode(u)->getLeftChild()->getNumber()][x].min_transfers +
                g_backtrack_matrix[G.getNode(u)->getRightChild()->getNumber()][x].min_transfers);
    }
    
    if (events[BacktrackElement::T_LEFT])
    {
        vector<vid_t> placements;
        backtrack_outside_placements(G.getNode(u)->getLeftChild()->getNumber(), x, placements);
        unsigned transfers = G.getNumberOfNodes() + 1;
        BOOST_FOREACH (vid_t y, placements)
        {
            transfers = min(transfers, g_backtrack_matrix[G.getNode(u)->getLeftChild()->getNumber()][y].min_transfers);
        }
        transfers += 1 + g_backtrack_matrix[G.getNode(u)->getRightChild()->getNumber()][x].min_transfers;
        min_transfers = min(min_transfers, transfers);
    }
    if (events[BacktrackElement::T_RIGHT])
    {
        vector<vid_t> placements;
        backtrack_outside_placements(G.getNode(u)->getRightChild()->getNumber(), x, placements);
        unsigned transfers = G.getNumberOfNodes() + 1;
        BOOST_FOREACH (vid_t y, placements)
        {
            transfers = min(transfers, g_backtrack_matrix[G.getNode(u)->getRightChild()->getNumber()][y].min_transfers);
        }
        transfers += 1 + g_backtrack_matrix[G.getNode(u)->getLeftChild()->getNumber()][x].min_transfers;
        min_transfers = min(min_transfers, transfers);
    }
    if (events[BacktrackElement::S])
    {
        min_transfers = min(min_transfers,
                            g_backtrack_matrix[G.getNode(u)->getLeftChild()->getNumber()][S.getNode(x)->getLeftChild()->getNumber()].min_transfers +
                g_backtrack_matrix[G.getNode(u)->getRightChild()->getNumber()][S.getNode(x)->getRightChild()->getNumber()].min_transfers);
    }
    if (events[BacktrackElement::S_REV])
    {
        min_transfers = min(min_transfers,
                            g_backtrack_matrix[G.getNode(u)->getLeftChild()->getNumber()][S.getNode(x)->getRightChild()->getNumber()].min_transfers +
                g_backtrack_matrix[G.getNode(u)->getRightChild()->getNumber()][S.getNode(x)->getLeftChild()->getNumber()].min_transfers);
    }
    if (events[BacktrackElement::BELOW_LEFT])
    {
        min_transfers = min(min_transfers,
                            g_backtrack_matrix[u][S.getNode(x)->getLeftChild()->getNumber()].min_transfers);
    }
    if (events[BacktrackElement::BELOW_RIGHT])
    {
        min_transfers = min(min_transfers,
                            g_backtrack_matrix[u][S.getNode(x)->getRightChild()->getNumber()].min_transfers);
    }
    g_backtrack_matrix[u][x].min_transfers = min_transfers;

}

void
Phyltr::backtrack_outside_placements(vid_t u, vid_t x, vector<vid_t> &placements)
{
    vid_t outside_sibling = Phyltr::g_backtrack_matrix[u][x].outside_sibling;
    if (outside_sibling != NONE)
    {
        placements.push_back(outside_sibling);
    }

    for(vid_t cur = Phyltr::g_backtrack_matrix[u][x].outside_ancestor;
        cur != NONE;
        cur = Phyltr::g_backtrack_matrix[u][cur].outside_ancestor)
    {
        placements.push_back(Phyltr::g_backtrack_matrix[u][cur].outside_sibling);
    }
}

void
Phyltr::combine_scenarios(const vector<Scenario> &vec1,
                          const vector<Scenario> &vec2,
                          vid_t u, vid_t x, BacktrackElement::Event e)
{
    const TreeExtended &G = *Phyltr::g_input.gene_tree;

    if (vec1.empty() || vec2.empty())
    {
        return;
    }
    
    if (Phyltr::g_input.print_only_minimal_transfer_scenarios)
    {
        unsigned transfers =  //root??
                vec1[0].transfer_edges.count() +
                vec2[0].transfer_edges.count();
        if (e == BacktrackElement::T_LEFT || e == BacktrackElement::T_RIGHT)
        {
            transfers += 1;
        }
        if (transfers > g_backtrack_matrix[u][x].min_transfers)
        {
            return;
        }
    }


    BOOST_FOREACH (const Scenario &sc1, vec1)
    {
        BOOST_FOREACH (const Scenario &sc2, vec2)
        {
            Scenario new_sc(g_input.gene_tree->getNumberOfNodes());
            new_sc.transfer_edges =
                    sc1.transfer_edges | sc2.transfer_edges;
            new_sc.duplications =
                    sc1.duplications | sc2.duplications;
            if (e == BacktrackElement::D)
            {
                new_sc.duplications.set(u);
            }
            if (e == BacktrackElement::T_LEFT)
            {
                new_sc.transfer_edges.set(G.getNode(u)->getLeftChild()->getNumber());
            }
            if (e == BacktrackElement::T_RIGHT)
            {
                new_sc.transfer_edges.set(G.getNode(u)->getRightChild()->getNumber());
            }
            g_backtrack_matrix[u][x].scenarios_at.push_back(new_sc);
        }
    }
}

BacktrackElement::BacktrackElement() :
    outside_sibling(NONE),
    outside_ancestor(NONE),
    min_transfers(0),
    scenarios_below_needed(false),
    scenarios_at_needed(false)
{
}
