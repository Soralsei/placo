#include "placo/problem/problem.h"
#include "eiquadprog/eiquadprog.hpp"

namespace placo
{
Problem::Problem()
{
}

Problem::~Problem()
{
  for (auto variable : variables)
  {
    delete variable;
  }
  for (auto constraint : constraints)
  {
    delete constraint;
  }
}

Variable& Problem::add_variable(std::string name, int size)
{
  Variable* variable = new Variable;
  variable->name = name;
  variable->k_start = n_variables;
  variable->k_end = n_variables + size;
  n_variables += size;

  variables.push_back(variable);
  return *variable;
}

ProblemConstraint& Problem::add_equality_zero(Expression expression)
{
  ProblemConstraint* constraint = new ProblemConstraint;

  constraint->expression = expression;
  constraints.push_back(constraint);

  return *constraint;
}

ProblemConstraint& Problem::add_equality(Expression expression, Eigen::VectorXd target)
{
  return add_equality_zero(expression - target);
}

ProblemConstraint& Problem::add_greater_than_zero(Expression expression)
{
  ProblemConstraint& constraint = add_equality_zero(expression);
  constraint.inequality = true;

  return constraint;
}

ProblemConstraint& Problem::add_greater_than(Expression expression, Eigen::VectorXd target)
{
  return add_greater_than_zero(expression - target);
}

ProblemConstraint& Problem::add_lower_than_zero(Expression expression)
{
  ProblemConstraint& constraint = add_equality_zero(-expression);
  constraint.inequality = true;

  return constraint;
}

ProblemConstraint& Problem::add_lower_than(Expression expression, Eigen::VectorXd target)
{
  return add_greater_than_zero(-expression + target);
}

void Problem::add_limit(Expression expression, Eigen::VectorXd target)
{
  // -target <= expression <= target
  add_greater_than(expression, -target);
  add_lower_than(expression, target);
}

ProblemConstraint& Problem::add_constraint(ProblemConstraint& constraint_)
{
  ProblemConstraint* constraint = new ProblemConstraint;
  *constraint = constraint_;
  constraints.push_back(constraint);

  return *constraint;
}

void Problem::solve()
{
  int n_equalities = 0;
  int n_inequalities = 0;
  int slack_variables = 0;

  for (auto constraint : constraints)
  {
    if (constraint->inequality && !constraint->hard)
    {
      slack_variables += constraint->expression.rows();
    }
  }

  Eigen::MatrixXd P(n_variables + slack_variables, n_variables + slack_variables);
  Eigen::VectorXd q(n_variables + slack_variables);

  P.setZero();
  q.setZero();

  // Adding regularization
  // XXX: The user variables should maybe not be regularized by default?
  P.setIdentity();
  P *= 1e-8;

  // Scanning the constraints (counting inequalities and equalities, building objectif function)
  for (auto constraint : constraints)
  {
    if (constraint->inequality)
    {
      // If the constraint is hard, this will be the true inequality, else, this will be the inequality
      // enforcing the slack variable to be >= 0
      n_inequalities += constraint->expression.rows();
    }
    else
    {
      if (constraint->hard)
      {
        n_equalities += constraint->expression.rows();
      }
      else
      {
        // Adding the soft constraint to the objective function
        P.noalias() += constraint->weight * (constraint->expression.A.transpose() * constraint->expression.A);
        q.noalias() += constraint->weight * (constraint->expression.A.transpose() * constraint->expression.b);
      }
    }
  }

  // Equality constraints
  Eigen::MatrixXd A(n_equalities, n_variables + slack_variables);
  Eigen::VectorXd b(n_equalities);
  A.setZero();
  b.setZero();

  // Inequality constraints
  Eigen::MatrixXd G(n_inequalities, n_variables + slack_variables);
  Eigen::VectorXd h(n_inequalities);
  G.setZero();
  h.setZero();

  int k_equality = 0;
  int k_inequality = 0;
  int k_slack = 0;

  // Slack variables should be positive
  for (int slack = 0; slack < slack_variables; slack += 1)
  {
    // s_i >= 0
    G(k_inequality, n_variables + slack) = 1;
    k_inequality += 1;
  }

  for (auto constraint : constraints)
  {
    if (constraint->inequality)
    {
      if (constraint->hard)
      {
        // Ax + b >= 0
        G.block(k_inequality, 0, constraint->expression.rows(), constraint->expression.cols()) =
            constraint->expression.A;
        h.block(k_inequality, 0, constraint->expression.rows(), 1) = constraint->expression.b;
        k_inequality += constraint->expression.rows();
      }
      else
      {
        // min(Ax + b - s)
        // A slack variable is assigend with all "soft" inequality and a minimization is added to the problem
        Eigen::MatrixXd As(constraint->expression.rows(), n_variables + slack_variables);
        As.setZero();
        As.block(0, 0, As.rows(), n_variables) = constraint->expression.A;

        for (int k = 0; k < constraint->expression.rows(); k++)
        {
          As(k, n_variables + k_slack) = -1;
          k_slack += 1;
        }

        P.noalias() += constraint->weight * (As.transpose() * As);
        q.noalias() += constraint->weight * (As.transpose() * constraint->expression.b);
      }
    }
    else if (constraint->hard)
    {
      // Ax + b = 0
      A.block(k_equality, 0, constraint->expression.rows(), constraint->expression.cols()) = constraint->expression.A;
      b.block(k_equality, 0, constraint->expression.rows(), 1) = constraint->expression.b;
      k_equality += constraint->expression.rows();
    }
  }

  Eigen::VectorXi activeSet;
  size_t activeSetSize;

  Eigen::VectorXd x(n_variables + slack_variables);
  x.setZero();
  double result =
      eiquadprog::solvers::solve_quadprog(P, q, A.transpose(), b, G.transpose(), h, x, activeSet, activeSetSize);

  if (result == std::numeric_limits<double>::infinity())
  {
    throw std::runtime_error("Problem: Infeasible QP (check your hard equality and inequality constraints)");
  }

  slacks = x.block(n_variables, 0, slack_variables, 1);

  for (auto variable : variables)
  {
    variable->version += 1;
    variable->value = Eigen::VectorXd(variable->size());
    variable->value = x.block(variable->k_start, 0, variable->size(), 1);
  }
}

};  // namespace placo