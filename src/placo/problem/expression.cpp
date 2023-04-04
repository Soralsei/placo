#include <sstream>
#include <iostream>
#include <algorithm>
#include "placo/problem/expression.h"
#include "placo/problem/constraint.h"

namespace placo
{
Expression::Expression()
{
}

Expression::Expression(const double& value)
{
  A = Eigen::MatrixXd(1, 0);
  b = Eigen::VectorXd(1);
  b(0, 0) = value;
}

Expression::Expression(const Eigen::VectorXd& v)
{
  A = Eigen::MatrixXd(v.rows(), 0);
  b = v;
}

Expression::Expression(const Expression& other)
{
  A = other.A;
  b = other.b;
}

bool Expression::is_scalar() const
{
  return rows() == 1 && cols() == 0;
}

int Expression::cols() const
{
  return A.cols();
}

int Expression::rows() const
{
  return A.rows();
}

Expression Expression::piecewise_add(double f) const
{
  // Assuming a scalar piece-wise sum
  Expression e;
  e.A = A;
  e.b = b;

  for (int k = 0; k < e.b.rows(); k++)
  {
    e.b(k, 0) += f;
  }

  return e;
}

Expression Expression::operator+(const Expression& other) const
{
  if (is_scalar())
  {
    return other.piecewise_add(b(0, 0));
  }
  else if (other.is_scalar())
  {
    return piecewise_add(other.b(0, 0));
  }

  if (rows() != other.rows())
  {
    std::ostringstream oss;
    oss << "Trying to add expressions with different # of rows (" << rows() << " vs " << other.rows() << ")";
    throw std::runtime_error(oss.str());
  }

  Expression e;

  e.A = Eigen::MatrixXd(rows(), std::max(cols(), other.cols()));
  e.A.setZero();
  e.b = Eigen::VectorXd(rows());
  e.b.setZero();

  e.A.block(0, 0, rows(), cols()) = A.block(0, 0, rows(), cols());
  e.A.block(0, 0, rows(), other.cols()) += other.A.block(0, 0, rows(), other.cols());
  e.b = b;
  e.b += other.b;

  return e;
}

Expression Expression::operator-(const Expression& other) const
{
  return (*this) + (other * (-1.));
}

Expression Expression::operator-() const
{
  return (*this) * (-1.);
}

Expression operator*(double f, const Expression& e)
{
  return e * f;
}

Expression Expression::operator*(double f) const
{
  Expression e(*this);
  e.A *= f;
  e.b *= f;

  return e;
}

Expression Expression::operator+(const Eigen::VectorXd v) const
{
  Expression e(*this);
  e.b += v;

  return e;
}

Expression operator+(const Eigen::VectorXd v, const Expression& e)
{
  return e + v;
}

Expression Expression::operator-(const Eigen::VectorXd v) const
{
  Expression e(*this);
  e.b -= v;
  return e;
}

Expression operator-(const Eigen::VectorXd v, const Expression& e)
{
  return e - v;
}

Expression operator*(const Eigen::MatrixXd M, const Expression& e_)
{
  Expression e(e_);
  e.A = M * e.A;
  e.b = M * e.b;

  return e;
}

Expression Expression::multiply(const Eigen::MatrixXd M)
{
  return M * (*this);
}

Expression Expression::sum()
{
  Expression e;
  e.A = Eigen::MatrixXd(1, cols());
  e.A.setZero();
  e.b = Eigen::VectorXd(1);
  e.b.setZero();

  for (int k = 0; k < rows(); k++)
  {
    e.A.block(0, 0, 1, cols()) += A.block(k, 0, 1, cols());
    e.b.block(0, 0, 1, 1) += b.block(k, 0, 1, 1);
  }

  return e;
}

Expression Expression::mean()
{
  Expression e = sum();
  e.A /= (double)cols();
  e.b /= (double)cols();

  return e;
}

Expression Expression::operator<<(const Expression& other)
{
  Expression e;

  e.A = Eigen::MatrixXd(rows() + other.rows(), std::max(cols(), other.cols()));
  e.A.setZero();
  e.b = Eigen::VectorXd(rows() + other.rows());
  e.b.setZero();

  e.A.block(0, 0, rows(), cols()) = A;
  e.A.block(rows(), 0, other.rows(), other.cols()) = other.A;

  e.b.block(0, 0, rows(), 1) = b;
  e.b.block(rows(), 0, other.rows(), 1) = other.b;

  return e;
}

ProblemConstraint Expression::operator>=(const Expression& other) const
{
  ProblemConstraint constraint;

  constraint.expression = *this - other;
  constraint.inequality = true;

  return constraint;
}

ProblemConstraint Expression::operator<=(const Expression& other) const
{
  ProblemConstraint constraint;

  constraint.expression = -(*this - other);
  constraint.inequality = true;

  return constraint;
}

ProblemConstraint Expression::operator>=(double f) const
{
  return (*this) >= Expression(f);
}

ProblemConstraint operator>=(double f, const Expression& e)
{
  return Expression(f) >= e;
}

ProblemConstraint Expression::operator<=(double f) const
{
  return (*this) <= Expression(f);
}

ProblemConstraint operator<=(double f, const Expression& e)
{
  return Expression(f) <= e;
}

ProblemConstraint Expression::operator>=(Eigen::VectorXd v) const
{
  return (*this) >= Expression(v);
}

ProblemConstraint operator>=(Eigen::VectorXd v, const Expression& e)
{
  return Expression(v) >= e;
}

ProblemConstraint Expression::operator<=(Eigen::VectorXd v) const
{
  return (*this) <= Expression(v);
}

ProblemConstraint operator<=(Eigen::VectorXd v, const Expression& e)
{
  return Expression(v) <= e;
}

ProblemConstraint Expression::operator==(const Expression& other) const
{
  ProblemConstraint constraint;
  constraint.expression = (*this) - other;

  return constraint;
}

ProblemConstraint Expression::operator==(Eigen::VectorXd v) const
{
  return (*this) == Expression(v);
}

ProblemConstraint operator==(Eigen::VectorXd v, const Expression& e)
{
  return e == v;
}

ProblemConstraint Expression::operator==(double f) const
{
  return (*this) == Expression(f);
}

ProblemConstraint operator==(double f, const Expression& e)
{
  return e == f;
}

};  // namespace placo