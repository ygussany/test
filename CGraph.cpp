#include "CGraph.h"



// ****************************** CGraphAdj ****************************** //

CGraphAdj::CGraphAdj(unsigned long n)
  : num_nodes(n), num_edges(0), id_rest(NULL),
    reserve(((n / GRAPH_RESERVE_UNIT) + 1) * GRAPH_RESERVE_UNIT)
{
  unsigned long v;
  CList *p;

  adj = MatrixXi::Zero(reserve, reserve);
  node_id = -VectorXi::Ones(reserve);
  rc_index = -VectorXi::Ones(reserve);

  for (v = 0; v < n; v++) {
    node_id(v) = v;
    rc_index(v) = v;
  }
  id_rest = new CList((void*)v++);
  for (p = id_rest; v < reserve; v++) p = new CList((void*)v, p);
}

CGraphAdj::CGraphAdj(MatrixXi ad, VectorXi id)
  : num_nodes(ad.rows()), num_edges(0), node_id(id), id_rest(NULL),
    reserve(((ad.rows() / GRAPH_RESERVE_UNIT) + 1) * GRAPH_RESERVE_UNIT)
{
  unsigned long i, j, k, v;
  CList *p;

  adj = MatrixXi::Zero(reserve, reserve);
  adj.tLC(num_nodes, num_nodes) = ad;
  node_id.conservativeResize(reserve);
  rc_index = -VectorXi::Ones(reserve);

  if (id == -VectorXi::Ones(0)) {
    for (v = 0; v < num_nodes; v++) {
      node_id(v) = v;
      rc_index(v) = v;
    }
    id_rest = new CList((void*)v++);
    for (p = id_rest; v < reserve; v++) p = new CList((void*)v, p);
  } else {
    for (k = 0; k < num_nodes; k++) rc_index(node_id(k)) = k;
    for (v = 0; v < reserve; v++) {
      if (rc_index(v) == -1) {
	id_rest = new CList((void*)v++);
	break;
      }
    }
    for (p = id_rest; v < reserve; v++) p = new CList((void*)v, p);
  }

  for (i = 0; i < num_nodes; i++) {
    for (j = i; j < num_nodes; j++) num_edges += adj(i, j);
  }
}

CGraphAdj::~CGraphAdj() {}



MatrixXi CGraphAdj::get_adj()
{ return adj.tLC(num_nodes, num_nodes); }

VectorXi CGraphAdj::get_node_ids()
{ return node_id.head(num_nodes); }

VectorXi CGraphAdj::get_rc_indices()
{ return rc_index; }

unsigned long CGraphAdj::get_num_edges_between(unsigned long u, unsigned long v)
{ return adj(u, v); }

unsigned long CGraphAdj::get_node_id(unsigned long k)
{ return (k < num_nodes)? node_id[k]: ULONG_MAX; }

unsigned long CGraphAdj::get_rc_index(unsigned long i)
{ return (i < reserve)? rc_index[i]: ULONG_MAX; }

unsigned long CGraphAdj::get_num_nodes()
{ return num_nodes; }

unsigned long CGraphAdj::get_num_edges()
{ return num_edges; }



unsigned long CGraphAdj::add_node()
{
  unsigned long v;
  CList *p;

  if (num_nodes + 1 >= reserve) {
    reserve += GRAPH_RESERVE_UNIT;
    adj.conservativeResize(reserve, reserve);
    node_id.conservativeResize(reserve);
    rc_index.conservativeResize(reserve);
    rc_index.tail(GRAPH_RESERVE_UNIT) = -VectorXi::Ones(GRAPH_RESERVE_UNIT);

    for (v = reserve - GRAPH_RESERVE_UNIT, p = id_rest; v < reserve; v++)
      p = new CList((void*)v, p);
  }

  v = (unsigned long)id_rest->get_data();
  id_rest = id_rest->get_next();
  delete id_rest->get_prev();

  node_id(num_nodes) = v;
  rc_index(v) = num_nodes++;
  adj.col(num_nodes - 1).head(num_nodes) = VectorXi::Zero(num_nodes);
  adj.row(num_nodes - 1) = adj.col(num_nodes - 1).transpose();
  return v;
}

CList* CGraphAdj::add_nodes(unsigned long n)
{
  unsigned long k, v;
  CList *p;

  if (num_nodes + n >= reserve) {
    unsigned long add_res = (num_nodes + n - reserve) / GRAPH_RESERVE_UNIT + 1;
    reserve += GRAPH_RESERVE_UNIT * add_res;
    adj.conservativeResize(reserve, reserve);
    node_id.conservativeResize(reserve);
    rc_index.conservativeResize(reserve);
    rc_index.tail(GRAPH_RESERVE_UNIT) = -VectorXi::Ones(GRAPH_RESERVE_UNIT);

    for (v = reserve - GRAPH_RESERVE_UNIT * add_res, p = id_rest; v < reserve; v++)
      p = new CList((void*)v, p);
  }

  for (k = 0, p = id_rest; k < n; k++, id_rest = id_rest->get_next()) {
    v = (unsigned long)id_rest->get_data();
    node_id(num_nodes + k) = v;
    rc_index(v) = num_nodes + k;
  }

  num_nodes += n;
  adj.tLC(num_nodes, num_nodes).bRC(n, num_nodes) = MatrixXi::Zero(n, num_nodes);
  adj.tLC(num_nodes, num_nodes).bRC(num_nodes, n) = MatrixXi::Zero(num_nodes, n);
  id_rest->get_prev()->cut();
  return p;
}

void CGraphAdj::remove_node(unsigned long v)
{
  if (v >= reserve) return;

  num_edges -= adj.col(rc_index(v)).sum();
  num_edges -= adj.row(rc_index(v)).sum();
  num_edges += adj(rc_index(v), rc_index(v));
  adj.col(rc_index(v)) = adj.col(--num_nodes);
  adj.row(rc_index(v)) = adj.row(num_nodes);

  rc_index(node_id(num_nodes)) = rc_index(v);
  node_id(rc_index(v)) = node_id(num_nodes);
  rc_index(v) = -1;
  node_id(num_nodes) = -1;
  id_rest = new CList((void*)v, NULL, id_rest);
}

void CGraphAdj::remove_nodes(CList* v_list)
{
  unsigned long k, l, n = 0, v, tmp;
  CList *p, *q;
  VectorXi y = VectorXi::Zero(num_nodes), one = VectorXi::Ones(num_nodes);
  for (p = v_list; p != NULL; p = q) {
    q = p->get_next();
    v = (unsigned long)p->get_data();
    if (v < reserve && rc_index(v) >= 0 && y(rc_index(v)) == 0) {
      y(rc_index(v)) = 1;
      n++;
    } else {
      if (p == v_list) v_list = p->get_next();
      delete p;
    }
  }
  num_edges -= one.transpose() * adj.tLC(num_nodes, num_nodes) * y;
  num_edges -= y.transpose() * adj.tLC(num_nodes, num_nodes) * (one - y);

  MatrixXi X = MatrixXi::Zero(num_nodes - n, num_nodes);
  VectorXi z(num_nodes);
  for (k = 0, l = num_nodes - 1; k < num_nodes; k++) {
    if (y(k) == 1) {
      for (; y(l) == 1; l--);
      z(k) = l;
      rc_index(node_id(k)) = -1;
      rc_index(node_id(l)) = k;
      node_id(k) = node_id(l--);
    } else z(k) = k;
    if (k < num_nodes - n) X(k, z(k)) = 1;
  }
  adj.tLC(num_nodes - n, num_nodes - n) = X * adj.tLC(num_nodes, num_nodes) * X.transpose();
  num_nodes -= n;

  if (v_list != NULL) {
    for (p = v_list; p->get_next() != NULL; p = p->get_next());
    p->append(id_rest);
    id_rest = v_list;
  }
}

void CGraphAdj::add_edge(unsigned long u, unsigned long v, bool directed)
{
  if (rc_index(u) == -1 || rc_index(v) == -1) return;
  adj(rc_index(u), rc_index(v))++;
  num_edges++;
  if (directed == false && u != v) {
    adj(rc_index(v), rc_index(u))++;
    num_edges++;
  }
}

bool CGraphAdj::remove_edge(unsigned long u, unsigned long v, bool directed)
{
  if (rc_index(u) == -1 || rc_index(v) == -1) return false;
  if (adj(rc_index(u), rc_index(v)) > 0) {
    adj(rc_index(u), rc_index(v))--;
    num_edges--;
  } else return false;

  if (directed == false && u != v) {
    if (adj(rc_index(v), rc_index(u)) > 0) {
      adj(rc_index(v), rc_index(u))--;
      num_edges--;
    } else {
      adj(rc_index(u), rc_index(v))++;
      num_edges++;
      return false;
    }
  }

  return true;
}

