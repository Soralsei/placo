Tasks and constraints
=====================

Tasks
-----

As you saw in the :doc:`introduction example <getting_started>`, 
a task defines something that the robot should do, like "reaching a given target with the effector".
The objective of the task is internally defined as a *task error* that the solver tries to take to zero.

PlaCo's kinematics solver is solving for :math:`\Delta q`, a variation of the robot's configuration,
according to your tasks. 

Tasks have a ``priority``, and optionally a ``weight`` to define the way they are taken into account by the solver.


+------------------+------------------------------------------------------------------------+
| Priority         | Description                                                            | 
+==================+========================================================================+
| ``hard``         | Task that must be satisfied by the solver (error should be brought     |
|                  | to zero).                                                              |
|                  | If it is not possible, the solver will fail.                           |
+------------------+------------------------------------------------------------------------+
| ``soft``         | Task that can be violated by the solver, but it                        |
|                  | will try to minimize the error.                                        |
|                  | The relative importance of ``soft`` tasks is defined by their          |
|                  | ``weight``.                                                            |
+------------------+------------------------------------------------------------------------+
| ``scaled``       | That that must be satisfied by the solver, with a scaling factor that  |
|                  | should be as close as possible to ``1``. This scaling factor is itself |
|                  | optimized by the solver, and available in ``solver.scale``.            |
|                  | All the ``scaled`` tasks will "scale together", so that the *direction |
|                  | of the task* is somehow preserved.                                     |
|                  | If it is not possible, the solver will fail.                           |
+------------------+------------------------------------------------------------------------+

Note that the ``weight`` parameter only affects ``soft`` tasks, and has no effect on ``hard`` 
or ``scaled`` tasks.

.. admonition:: Math details

    A task being an error function of the robot configuration, we can denote it :math:`e(q)`. We then
    search for :math:`e(q + \Delta q) = 0`. Since the geometry is highly non-linear, the solver
    will use first order approximation :math:`e(q+\Delta q) \approx e(q) + J(q) \Delta q`, where
    :math:`J(q)` is the Jacobian of the task.

    * For a task with ``hard`` priority level, the equality constraint :math:`J(q) \Delta q = -e(q)` is enforced.
    * For a task with ``soft`` priority level, the quantity :math:`w \lVert J(q) \Delta q + e(q) \rVert^2`,
      where :math:`w` is the task weight, is added to the solver's objective function.
    * For a task with ``scaled`` priority level, the equality constraint :math:`J(q) \Delta q = -\lambda e(q)`
      is enforced, with :math:`\lambda` being the scaling factor, itself optimized by the solver,
      enforcing :math:`0 \leq \lambda \leq 1`, and adding :math:`\lVert \lambda - 1 \rVert^2` to the objective function
      to encourage the scaling factor to be close to 1.

Constraints
-----------

Constraints are specific limits that you want your robot to respect. While joint limits and velocity constraints are
implemented in the solver itself, other specific constraints can be added to the solver.

Those can be prioritized and weighted in the exact same way as tasks, as explained above.

Removing tasks and constraints
------------------------------

Tasks and constraints can be removed from the solver by using
:func:`remove_task <placo.KinematicsSolver.remove_task>` and :func:`remove_constraint <placo.KinematicsSolver.remove_constraint>`:

.. code-block:: python

    # Removing a task
    solver.remove_task(some_task)

    # Removing a constraint
    solver.remove_constraint(some_constraint)