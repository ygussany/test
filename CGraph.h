#ifndef _CGraph_20140215
#define _CGraph_20140215

#include <iostream>
#include "CList.h"
#include "Eigen/Core"
using namespace std;
using namespace Eigen;

#define GRAPH_RESERVE_UNIT 0x100
#define tLC(a, b) topLeftCorner(a, b)
#define bRC(a, b) bottomRightCorner(a, b)

class CGraphAdj;
class CGraphInc;


class CGraphAdj {
  MatrixXi adj;
  VectorXi node_id, rc_index;
  unsigned long num_nodes, num_edges, reserve;
  CList *id_rest;

public:
  CGraphAdj(unsigned long n);
  CGraphAdj(MatrixXi ad, VectorXi id = VectorXi::Zero(0));
  ~CGraphAdj();

  MatrixXi get_adj();
  VectorXi get_node_ids();
  VectorXi get_rc_indices();
  unsigned long get_num_edges_between(unsigned long u, unsigned long v);
  unsigned long get_node_id(unsigned long k);
  unsigned long get_rc_index(unsigned long v);
  unsigned long get_num_nodes();
  unsigned long get_num_edges();

  unsigned long add_node();
  CList* add_nodes(unsigned long n);
  void remove_node(unsigned long v);
  void remove_nodes(CList* v_list);
  void add_edge(unsigned long u, unsigned long v, bool directed = false);
  bool remove_edge(unsigned long u, unsigned long v, bool directed = false);
};


class CGraphInc {
};


#endif
