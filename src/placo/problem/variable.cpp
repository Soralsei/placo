#include "placo/problem/variable.h"

namespace placo
{
Expression Variable::expr(int start, int rows)
{
  if (start == -1)
  {
    start = k_start;
  }
  if (rows == -1)
  {
    rows = k_start + size() - start;
  }

  Expression e;
  e.A = Eigen::MatrixXd(rows, k_end);
  e.A.setZero();
  e.b = Eigen::VectorXd(rows);
  e.b.setZero();

  for (int k = 0; k < rows; k++)
  {
    e.A(k, start + k) = 1;
  }

  return e;
}

int Variable::size()
{
  return k_end - k_start;
}
};  // namespace placo